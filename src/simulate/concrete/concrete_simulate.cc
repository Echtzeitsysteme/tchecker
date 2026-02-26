/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/concrete/concrete_simulate.hh"
#include "tchecker/simulate/concrete/concrete_display.hh"

#include "tchecker/operational-semantics/max_delay.hh"
#include "tchecker/operational-semantics/zone_valuation_converter.hh"
#include "tchecker/operational-semantics/attributes_to_valuation.hh"

namespace tchecker {

namespace simulate {

namespace concrete {

static int64_t const NO_SELECTION = std::numeric_limits<int64_t>::max();

concrete_simulator_t::concrete_simulator_t(tchecker::parsing::system_declaration_t const & sysdecl,
                                           enum tchecker::simulate::display_type_t display_type,
                                           std::ostream & os)
  : _system(new tchecker::ta::system_t{sysdecl}),
    _zg(tchecker::zg::factory(_system, tchecker::ts::NO_SHARING, 
                              tchecker::zg::DISTINGUISHED_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION,
                              1000, 65536)),
    _state_space(std::make_shared<state_space_t>(_system)),
    _g(_state_space->graph()),
    _display(std::unique_ptr<tchecker::simulate::concrete::concrete_display_t>(concrete_display_factory(display_type, os, _zg))),
    _highest_delay(_zg->extrapolation_max() + 1),
    _os(os)
{
  srand(time(NULL)); // needed if user chooses randomize selection
}

// Concrete Simulation
std::shared_ptr<tchecker::simulate::concrete::state_space_t>
concrete_simulator_t::interactive_simulation()
{

  // During the interactive select, decision should be done by user input.
  auto read_input = []() -> std::string {
    std::string input;
    std::getline(std::cin, input);  // Reads the entire line, including spaces
    return input;
  };
  
  auto select = initial_select(read_input, true);
  if(ERROR == select.first) {
    return _state_space;
  }

  do {
    select = next_select(read_input, true);
    if (ERROR == select.first) {
      break;
    }
  } while (1);

  return _state_space;
}

void concrete_simulator_t::onestep_simulation(std::map<std::string, std::string> const & starting_state_attributes)
{
  if (starting_state_attributes.empty()) {
    _zg->initial(_v);
    _display->output_initial(_v);
  }
  else {

    parse_starting_state_attributes(starting_state_attributes);

    tchecker::clock_rational_value_t max_delay = calculate_max_delay();
    tchecker::zg::const_state_sptr_t previous_symb_const{_previous_symb};

    _v.clear();
    _zg->next(previous_symb_const, _v);

    _display->output_next(previous_symb_const, _v, max_delay != _highest_delay, max_delay);
  }
}

std::shared_ptr<tchecker::simulate::concrete::state_space_t>
concrete_simulator_t::randomized_simulation(std::map<std::string, std::string> const & starting_state_attributes,
                                            std::size_t nsteps)
{

  auto give_r = []() -> std::string {
    return std::string("r");
  };

  if (starting_state_attributes.empty()) { // start simulation from initial state
    auto select = initial_select(give_r, false);
    if(ERROR == select.first) {
      return _state_space;
    }
  } else { // start simulation from specified starting state
    parse_starting_state_attributes(starting_state_attributes);
  }
  for (std::size_t i = 0; i < nsteps; ++i) {

    auto select = next_select(give_r, false);
    if (ERROR == select.first) {
      break;
    }
  }
  tchecker::zg::const_state_sptr_t previous_symb_const{_previous_symb};
  _display->output_state(previous_symb_const);
  return _state_space;
}

std::pair<concrete_trans_type_t, tchecker::clock_rational_value_t> 
concrete_simulator_t::concrete_interactive_select(std::ostream & s_out,
                                                  std::function<std::string()> input_func, 
                                                  concrete_display_t & display, 
                                                  tchecker::zg::const_state_sptr_t const & s,
                                                  std::vector<tchecker::zg::zg_t::sst_t> const & v,
                                                  bool finite_max_delay,
                                                  tchecker::clock_rational_value_t max_delay,
                                                  bool print_interaction)
{
  assert(v.size() < NO_SELECTION);
  assert(max_delay.denominator() == 1 || max_delay.denominator() == 2);

  if (v.size() == 0 && 0 == max_delay) {
    s_out << "We have reached a dead end. No continuation possible." << std::endl;
    return std::make_pair(ACTION, 
            tchecker::clock_rational_value_t(NO_SELECTION, 1));
  }

  if (print_interaction && s.ptr() == nullptr) { // initial simulation step
    display.output_initial(v);
  }
  else if (print_interaction) {
    display.output_next(s, v, finite_max_delay, max_delay);
  }

  do {
    if(print_interaction && nullptr != s.ptr()) {
      s_out << "Select d <delay> (<delay> must have the form int or int/int) or " << std::endl;
    }
    if(print_interaction && v.size() > 0) {
      s_out << "Select 0-" << v.size() - 1;
    }
    if(print_interaction) {
      s_out << " (q: quit, r: random)? ";
    }

    std::string input = input_func();

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
        s_out << "Randomly choosen delay: " << tchecker::clock_rational_value_t(random_delay, 2) << std::endl;
        return std::make_pair(DELAY, tchecker::clock_rational_value_t(random_delay, 2));
      } else {
        if(v.size() == 0) {
          throw std::runtime_error("We need an initial state");
        }
        tchecker::clock_rational_value_t used( 
          static_cast<int64_t>(tchecker::simulate::randomized_select(v)), 1);
        s_out << "Randomly choosen action transition no.: " << used << std::endl;
        return std::make_pair(ACTION, used);
      }
    }
    else if (input[0] == 'd' && nullptr != s.ptr()) {
      if(input.length() < 2) {
        s_out << "invalid delay" << std::endl;
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
            s_out << "denominator cannot be zero." << std::endl;
            continue;
          }
          if(den > 2) {
            s_out << "denominator cannot be larger than two" << std::endl;
            continue;
          }
        }
        if(num > INT64_MAX || den > INT64_MAX) {
          s_out << "invalid values for delay" << std::endl;
          continue;
        }
        tchecker::clock_rational_value_t val(static_cast<int64_t>(num), static_cast<int64_t>(den));
        if(finite_max_delay && val > max_delay) {
          s_out << "delay to high" << std::endl;
          continue;
        }
        return std::make_pair(DELAY, val);
      } catch (const std::exception& e) {
          s_out << "invalid delay" << std::endl;
          continue;
      }
    }
    else {
      try {
        long int k = std::stol(input);

        if (v.size() > INT64_MAX || k >= static_cast<int64_t>(v.size())) {
          s_out << "Out-of-range input" << std::endl;
          continue;
        }
        return std::make_pair(ACTION, static_cast<std::int64_t>(k));
      } catch(const std::exception& e) {
        s_out << "Invalid input" << std::endl;
      }
    }
  } while (1);
}


std::pair<tchecker::simulate::concrete::concrete_trans_type_t, tchecker::clock_rational_value_t>
concrete_simulator_t::initial_select(std::function<std::string()> input_func, bool print_interaction)
{
  _v.clear();
    // start simulation from initial states
  _zg->initial(_v);
  auto select = concrete_interactive_select(_os, input_func, *_display, 
                                            tchecker::zg::const_state_sptr_t{nullptr}, _v, 
                                            clock_rational_value_t(_highest_delay, 1), false, print_interaction);
                                      
  if (select.second.numerator() > 0 && 
      select.second.numerator() ==  NO_SELECTION && 
      select.second.denominator() == 1) {
    return std::make_pair<tchecker::simulate::concrete::concrete_trans_type_t, tchecker::clock_rational_value_t>(ERROR, 0);
  }

  _previous_symb = std::get<1>(_v[select.second.numerator()]);
  // for some reason, the invariant of a tchecker::ta::state_t is not stored within the state, but within the transition. 
  // Therefore, we need to store it explicitly.
  _previous_node_inv = std::make_shared<tchecker::clock_constraint_container_t>(
                              std::get<2>(_v[select.second.numerator()])->tgt_invariant_container());

  _previous_node = _g.add_node(_previous_symb); 
  return select;
}

std::pair<tchecker::simulate::concrete::concrete_trans_type_t, tchecker::clock_rational_value_t>
concrete_simulator_t::next_select(std::function<std::string()> input_func, bool print_interaction)
{
  _v.clear();
  tchecker::clock_rational_value_t max_delay = calculate_max_delay();
  
  tchecker::zg::const_state_sptr_t previous_symb_const{_previous_symb};
  _zg->next(previous_symb_const, _v);

  auto select = concrete_interactive_select(std::cout, input_func, *_display, previous_symb_const, _v, max_delay != _highest_delay, max_delay, print_interaction);
  if (select.second.numerator() > 0 && 
      select.second.numerator() ==  NO_SELECTION && 
      select.second.denominator() == 1) {
    return std::make_pair<tchecker::simulate::concrete::concrete_trans_type_t, tchecker::clock_rational_value_t>(ERROR, 0);
  }

  if (DELAY == select.first) {
    std::shared_ptr<tchecker::ta::state_t> ta_ptr(_previous_symb.ptr(), [](tchecker::ta::state_t*){}); 
    auto new_node = _g.add_node(_previous_node, select.second);
    _g.add_delay_edge(select.second, *_previous_node, *new_node);
    _previous_node = new_node;
    std::shared_ptr<tchecker::zg::zone_t> new_zone = tchecker::operational_semantics::convert(_previous_node->valuation());
    _previous_symb->replace_zone(*new_zone);
  } else if (ACTION == select.first) {
    auto new_symb = std::get<1>(_v[select.second.numerator()]);
    auto new_node = _g.add_node(new_symb);
    auto transition = std::get<2>(_v[select.second.numerator()]);
    _g.add_action_edge(transition, *_previous_node, *new_node);
    _previous_node = new_node;
    _previous_symb = new_symb;
    _previous_node_inv = std::make_shared<tchecker::clock_constraint_container_t>(
                          std::get<2>(_v[select.second.numerator()])->tgt_invariant_container());
  }

  return select;
}

tchecker::clock_rational_value_t concrete_simulator_t::calculate_max_delay()
{
  tchecker::clock_rational_value_t max_delay = 0;
  if (tchecker::ta::delay_allowed(*_system, _previous_node->ta_state()->vloc())) {
    tchecker::zg::state_sptr_t eps = _zg->clone_state(_previous_symb);
    _zg->semantics()->delay(eps->zone_ptr()->dbm(), eps->zone_ptr()->dim(), *_previous_node_inv);
    max_delay = tchecker::operational_semantics::max_delay(eps->zone(), _previous_node->valuation(), _highest_delay, 0);
  }
  return max_delay;  
}

int concrete_simulator_t::parse_starting_state_attributes(std::map<std::string, std::string> const & starting_state_attributes)
{
  assert(!starting_state_attributes.empty());

  std::shared_ptr<tchecker::clockval_t> clockval = tchecker::operational_semantics::build(starting_state_attributes);

  std::map<std::string, std::string> attr(starting_state_attributes);
  attr["zone"] = tchecker::to_string(*tchecker::operational_semantics::convert(clockval), 
                                     _system->clock_variables().flattened().index());

  _v.clear();
  _zg->build(attr, _v);

  // start simulation from specified state
  assert(_v.size() <= 1);
  if (_v.size() == 0) {
    std::cerr << "No valid state to start simulation" << std::endl;
    throw std::runtime_error("No valid state to start simulation");
  }

  _previous_symb = _zg->state(_v[0]);
  _previous_node_inv = std::make_shared<tchecker::clock_constraint_container_t>(std::get<2>(_v[0])->tgt_invariant_container());
  _previous_node = _g.add_node(_previous_symb);

  return 0;
}

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker