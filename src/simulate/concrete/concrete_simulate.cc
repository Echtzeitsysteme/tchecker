/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/concrete/concrete_simulate.hh"
#include "tchecker/simulate/concrete/concrete_display.hh"
#include "tchecker/simulate/concrete/concrete_graph.hh"

#include "tchecker/operational-semantics/max_delay.hh"
#include "tchecker/operational-semantics/zone_valuation_converter.hh"
#include "tchecker/operational-semantics/attributes_to_valuation.hh"

namespace tchecker {

namespace simulate {

namespace concrete {

/*!
 \brief Concrete Transition Types
*/
enum concrete_trans_type_t {
  ACTION = 0,
  DELAY
};

static int64_t const NO_SELECTION = std::numeric_limits<int64_t>::max();


static std::pair<concrete_trans_type_t, tchecker::clock_rational_value_t> 
concrete_interactive_select(concrete_display_t & display, tchecker::zg::const_state_sptr_t const & s,
                             std::vector<tchecker::zg::zg_t::sst_t> const & v, bool finite_max_delay,
                             tchecker::clock_rational_value_t max_delay)
{
  assert(v.size() < NO_SELECTION);
  assert(max_delay.denominator() == 1 || max_delay.denominator() == 2);

  if (v.size() == 0 && 0 == max_delay) {
    std::cout << "We have reached a dead end. No continuation possible." << std::endl;
    return std::make_pair(ACTION, 
            tchecker::clock_rational_value_t(NO_SELECTION, 1));
  }

  if (s.ptr() == nullptr) { // initial simulation step
    display.output_initial(v);
  }
  else {
    display.output_next(s, v, finite_max_delay, max_delay);
  }

  do {
    if(nullptr != s.ptr()) {
      std::cout << "Select d <delay> (<delay> must have the form int or int/int) or " << std::endl;
    }
    if(v.size() > 0) {
      std::cout << "Select 0-" << v.size() - 1;
    }
    std::cout << " (q: quit, r: random)? ";
    
    std::string input;
    std::getline(std::cin, input);  // Reads the entire line, including spaces

    if (std::cin.eof() || input[0] == 'q')
      return std::make_pair(ACTION, 
              tchecker::clock_rational_value_t(NO_SELECTION, 1));
    else if (input[0] == 'r') {
      int use_delay = std::rand() % 2;
      if(v.size() == 0) {
        use_delay = 0;
      }
      if(0 == use_delay && nullptr != s.ptr()) {
        int64_t random_delay = ( static_cast<int64_t>(std::rand())) % 
                                ((max_delay.denominator() == 2) ? (max_delay.numerator() + 1) : 2*max_delay.numerator() + 1);
        std::cout << "Randomly choosen delay: " << tchecker::clock_rational_value_t(random_delay, 2) << std::endl;
        return std::make_pair(DELAY, tchecker::clock_rational_value_t(random_delay, 2));
      } else {
        if(v.size() == 0) {
          throw std::runtime_error("We need an initial state");
        }
        tchecker::clock_rational_value_t used( 
          static_cast<int64_t>(tchecker::simulate::randomized_select(v)), 1);
        std::cout << "Randomly choosen action: " << used << std::endl;
        return std::make_pair(ACTION, used);
      }
    }
    else if (input[0] == 'd' && nullptr != s.ptr()) {
      if(input.length() < 2) {
        std::cout << "invalid delay" << std::endl;
        continue;
      }
      input=input.substr(2);

      try{
        size_t slash_pos = input.find('/');
        long int num;
        long int den;

        if (slash_pos == std::string::npos) {
          // No slash: treat as numerator/1
          num = std::stol(input);
          den = 1;
        } else {
          // Split into numerator and denominator
          num = std::stol(input.substr(0, slash_pos));
          den = std::stol(input.substr(slash_pos + 1));
          if (den == 0) {
            std::cerr << "denominator cannot be zero." << std::endl;
            continue;
          }
          if(den > 2) {
            std::cout << "denominator cannot be larger than two" << std::endl;
            continue;
          }
        }
        if(num > INT64_MAX || den > INT64_MAX) {
          std::cout << "invalid values for delay" << std::endl;
          continue;
        }
        tchecker::clock_rational_value_t val(static_cast<int64_t>(num), static_cast<int64_t>(den));
        if(finite_max_delay && val > max_delay) {
          std::cout << "delay to high" << std::endl;
          continue;
        }
        return std::make_pair(DELAY, val);
      } catch (const std::exception& e) {
          std::cout << "invalid delay" << std::endl;
          continue;
      }
    }
    else {
      try {
        long int k = std::stol(input);

        if (v.size() > INT64_MAX || k >= static_cast<int64_t>(v.size())) {
          std::cerr << "Out-of-range input" << std::endl;
          continue;
        }
        return std::make_pair(ACTION, static_cast<std::int64_t>(k));
      } catch(const std::exception& e) {
        std::cout << "Invalid input" << std::endl;
      }
    }
  } while (1);
}

  // Concrete Simulation
std::shared_ptr<tchecker::simulate::state_space_t>
interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os)
{
  std::size_t const block_size = 1000;
  std::size_t const table_size = 65536;

  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{sysdecl}};
  std::shared_ptr<tchecker::zg::zg_t> zg{tchecker::zg::factory(system, tchecker::ts::NO_SHARING,
                                                                tchecker::zg::DISTINGUISHED_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION,
                                                                block_size, table_size)};

  std::shared_ptr<state_space_t> state_space = std::make_shared<state_space_t>(system);

  graph_t & g = state_space->graph();

  std::vector<tchecker::zg::zg_t::sst_t> v;

  std::unique_ptr<concrete_display_t> display{concrete_display_factory(display_type, os, zg)};

  srand(time(NULL)); // needed if user chooses randomize selection

  std::shared_ptr<node_t> previous_node = nullptr;
  auto highest_delay = zg->extrapolation_max() + 1; // if this delay is possible, any delay is possible.


  // start simulation from initial states
  zg->initial(v);
  // for the initial statest, max_delay is not relevant, as the initial node must be chosen, first.
  auto select = concrete_interactive_select(*display, tchecker::zg::const_state_sptr_t{nullptr}, v, clock_rational_value_t(highest_delay, 1), true);
  
  if (select.second.numerator() > 0 && 
      select.second.numerator() ==  NO_SELECTION && 
      select.second.denominator() == 1) {
    return state_space;
  }

  tchecker::zg::state_sptr_t previous_symb = std::get<1>(v[select.second.numerator()]);
  // for some reason, the invariant of a tchecker::ta::state_t is not stored within the state, but within the transition. 
  // Therefore, we need to store it explicitly.
  tchecker::clock_constraint_container_t & previous_node_inv = std::get<2>(v[select.second.numerator()])->tgt_invariant_container();

  previous_node = g.add_node(previous_symb);
 
  v.clear();

  do {

    tchecker::clock_rational_value_t max_delay = 0;
    if (tchecker::ta::delay_allowed(*system, previous_node->ta_state()->vloc())) {
      tchecker::zg::state_sptr_t eps = zg->clone_state(previous_symb);
      zg->semantics()->delay(eps->zone_ptr()->dbm(), eps->zone_ptr()->dim(), previous_node_inv);
      max_delay = tchecker::operational_semantics::max_delay(eps->zone(), previous_node->valuation(), highest_delay, 0);
    }
  
    tchecker::zg::const_state_sptr_t previous_symb_const{previous_symb};
    zg->next(previous_symb_const, v);

    select = concrete_interactive_select(*display, previous_symb_const, v, max_delay != highest_delay, max_delay);
    if (select.second.numerator() > 0 && 
        select.second.numerator() ==  NO_SELECTION && 
        select.second.denominator() == 1) {
      break;
    }

    if (DELAY == select.first) {
      std::shared_ptr<tchecker::ta::state_t> ta_ptr(previous_symb.ptr(), [](tchecker::ta::state_t*){}); 
      auto new_node = g.add_node(previous_node, select.second);
      g.add_delay_edge(select.second, *previous_node, *new_node);
      previous_node = new_node;
      std::shared_ptr<tchecker::zg::zone_t> new_zone = tchecker::operational_semantics::convert(previous_node->valuation());
      previous_symb->replace_zone(*new_zone);
    } else if (ACTION == select.first) {
      auto new_symb = std::get<1>(v[select.second.numerator()]);
      auto new_node = g.add_node(new_symb);
      auto transition = std::get<2>(v[select.second.numerator()]);
      g.add_action_edge(transition, *previous_node, *new_node);
      previous_node = new_node;
      previous_symb = new_symb;
      previous_node_inv = std::get<2>(v[select.second.numerator()])->tgt_invariant_container();
    }

    v.clear();
    
  } while (1);

  return state_space;
}


void onestep_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os,
                        std::map<std::string, std::string> const & starting_state_attributes)
{
  std::size_t const block_size = 1000;
  std::size_t const table_size = 65536;

  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{sysdecl}};
  std::shared_ptr<tchecker::zg::zg_t> zg{tchecker::zg::factory(system, tchecker::ts::NO_SHARING,
                                                               tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION,
                                                               block_size, table_size)};
  std::vector<tchecker::zg::zg_t::sst_t> v;

  std::unique_ptr<concrete_display_t> display{concrete_display_factory(display_type, os, zg)};

  if (starting_state_attributes.empty()) {
    // start simulation from initial states (interactive selection)
    zg->initial(v);
    display->output_initial(v);
    v.clear();
  }
  else {

    std::shared_ptr<tchecker::clockval_t> clockval = tchecker::operational_semantics::build(starting_state_attributes);
    std::shared_ptr<node_t> previous_node = nullptr;
    auto highest_delay = zg->extrapolation_max() + 1; // if this delay is possible, any delay is possible.

    std::map<std::string, std::string> attr(starting_state_attributes);
    attr["zone"] = tchecker::to_string(*tchecker::operational_semantics::convert(clockval), 
                                       system->clock_variables().flattened().index());

    zg->build(attr, v);

    // start simulation from specified state
    assert(v.size() <= 1);
    if (v.size() == 0) {
      std::cerr << "No valid state to start simulation" << std::endl;
      return;
    }

    tchecker::zg::state_sptr_t s{zg->state(v[0])};
    tchecker::clock_constraint_container_t & previous_node_inv = std::get<2>(v[0])->tgt_invariant_container();
    v.clear();

    tchecker::clock_rational_value_t max_delay = 0;
    if (tchecker::ta::delay_allowed(*system, s->vloc())) {
      tchecker::zg::state_sptr_t eps = zg->clone_state(s);
      zg->semantics()->delay(eps->zone_ptr()->dbm(), eps->zone_ptr()->dim(), previous_node_inv);
      max_delay = tchecker::operational_semantics::max_delay(eps->zone(), clockval, highest_delay, 0);
    }
  
    tchecker::zg::const_state_sptr_t s_const{s};
    zg->next(s_const, v);

    display->output_next(s_const, v, max_delay != highest_delay, max_delay);
  }
}


} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker