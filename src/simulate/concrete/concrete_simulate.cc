/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/concrete/concrete_simulate.hh"
#include "tchecker/simulate/concrete/concrete_display.hh"
#include "tchecker/simulate/concrete/concrete_graph.hh"

#include "tchecker/operational-semantics/helping_functions.hh"
#include "tchecker/operational-semantics/zone_valuation_converter.hh"

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

static std::pair<concrete_trans_type_t, tchecker::clock_rational_value_t> 
concrete_interactive_select(concrete_display_t & display, tchecker::zg::const_state_sptr_t const & s,
                             std::vector<tchecker::zg::zg_t::sst_t> const & v, bool finite_max_delay,
                             tchecker::clock_rational_value_t max_delay)
{
  assert(v.size() < tchecker::simulate::NO_SELECTION);

  if (v.size() == 0 && 0 == max_delay)
    return std::make_pair(ACTION, tchecker::simulate::NO_SELECTION);

  if (s.ptr() == nullptr) { // initial simulation step
    display.output_initial(v);
  }
  else {
    display.output_next(s, v, max_delay);
  }

  do {
    if(nullptr != s.ptr()) {
      std::cout << "Select d <delay> (<delay> must have the form int or int/int) or " << std::endl;
    }

    std::cout << "Select 0-" << v.size() - 1 << " (q: quit, r: random)? ";
    std::string input;
    std::cin >> input;

    if (std::cin.eof() || input == "q")
      return std::make_pair(ACTION, tchecker::simulate::NO_SELECTION);
    else if (input == "r")
      return std::make_pair(ACTION, tchecker::simulate::randomized_select(v));
    else if (input == "d" && nullptr != s.ptr()) {
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
        tchecker::clock_rational_value_t val(num, den);
        if(finite_max_delay && val > max_delay) {
          std::cout << "delay to high" << std::endl;
          continue;
        }
        return std::make_pair(DELAY, tchecker::clock_rational_value_t(num, den));
      } catch (const std::exception& e) {
          std::cout << "invalid delay" << std::endl;
          continue;
      }
    }
    else {
      char * end = nullptr;
      errno = 0;
      unsigned long k = std::strtoul(input.c_str(), &end, 10);
      if (*end != 0) {
        std::cerr << "Invalid input" << std::endl;
        continue;
      }

      if (errno == ERANGE || k > std::numeric_limits<std::size_t>::max() || k >= v.size()) {
        std::cerr << "Out-of-range input" << std::endl;
        continue;
      }
      return std::make_pair(ACTION, static_cast<std::size_t>(k));
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

  
  if (select.second == tchecker::simulate::NO_SELECTION)
     return state_space;

  tchecker::zg::state_sptr_t previous_symb = std::get<1>(v[select.second.numerator()]);
  // for some reason, the invariant of a tchecker::ta::state_t is not stored within the state, but within the transition. 
  // Therefore, we need to store it explicitly.
  tchecker::clock_constraint_container_t & previous_node_inv = std::get<2>(v[select.second.numerator()])->tgt_invariant_container();

  // Now follows an ugly hack.
  // Due to the fact that zg states inherit from ta states (I believe this is a bad decision, but ...), 
  // we have to convert the state_sptr_t to a std::shared_ptr<tchecker::ta::state_t>. Uff...
  // To do so, we take the raw pointer to the chosen_symb and delete the destructor.
  std::shared_ptr<tchecker::ta::state_t> ta_ptr(previous_symb.ptr(), [](tchecker::ta::state_t*){}); 
  previous_node = g.add_node(ta_ptr, tchecker::operational_semantics::convert(previous_symb->zone()), true, false);
 
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

    select = concrete_interactive_select(*display, previous_symb_const, v, max_delay != 0, max_delay);
    if (select.second == tchecker::simulate::NO_SELECTION)
      break;

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


} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker