/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_CONCRETE_SIMULATE_DISPLAY_HH
#define TCHECKER_TCK_CONCRETE_SIMULATE_DISPLAY_HH

/*!
 \file concrete_display.hh
 \brief State/transition display for concrete simulation
*/

#include <vector>
#include <ostream>

#if USE_BOOST_JSON
#include <boost/json.hpp>
#endif

#include "tchecker/simulate/simulate.hh"
#include "tchecker/zg/zg.hh"


namespace tchecker {

namespace simulate {

namespace concrete {

/*!
 \class display_t
 \brief Interface to state/transition display
*/
class concrete_display_t {
 public:

  concrete_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg);
  /*!
   \brief Destructor
  */
  virtual ~concrete_display_t() = default;

  /*!
   \brief Display initial simulation step
   \param v : collection of initial triples (status, state, transition)
  */
  virtual void output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v) = 0;

  /*!
   \brief Display simulation next step
   \param s : current state
   \param v : collection of next triples (status, state, transition)
   \param finite_max_delay : whether the max delay is finite
   \param max_delay : the max delay (only used if finite_max_delay = true)
   */
  virtual void output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v, bool finite_max_delay,    
                           tchecker::clock_rational_value_t max_delay) = 0;
      
  /*!
   \brief Display state
   \param s : state
   \post Attributes of state s in _zg have been output to _os
  */
  virtual void output_state(tchecker::zg::const_state_sptr_t const & s) = 0;
 
 protected:
  /*!
   \brief Get attributes of states
   \param s : state
   \param attr : the map to fill
   \post Attributes of state s in _zg have been added to attr
  */
  std::map<std::string, std::string> gen_attr_map(tchecker::zg::const_state_sptr_t const & s, std::map<std::string, std::string> & attr);

  std::ostream & _os;                      /*!< Output stream */
  std::shared_ptr<tchecker::zg::zg_t> _zg; /*!< Zone graph */
};

/*!
 \class concrete_hr_display_t
 \brief Human-readable display
*/
class concrete_hr_display_t : public concrete_display_t {

 public:

  /*!
   \brief Constructor
   \param os : output stream
   \param zg : zone graph
   \pre zg is not nullptr
   \post this keeps a reference on os and a pointer to zg
   \throw std::invalid_argument : if zg is nullptr
  */
  concrete_hr_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg);

  /*!
   \brief Copy constructor
  */
  concrete_hr_display_t(concrete_hr_display_t const &) = default;

  /*!
   \brief Move constructor
  */
  concrete_hr_display_t(concrete_hr_display_t &&) = default;

  /*!
   \brief Destructor
  */
  virtual ~concrete_hr_display_t() = default;

  /*!
   \brief Assignment operator
  */
  concrete_hr_display_t & operator=(concrete_hr_display_t const &) = delete;

  /*!
   \brief Move-assignment operator
  */
  concrete_hr_display_t & operator=(concrete_hr_display_t &&) = delete;

  void output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v) override;

  void output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v, 
                   bool finite_max_delay, tchecker::clock_rational_value_t max_delay) override;

  void output_state(tchecker::zg::const_state_sptr_t const & s) override;

private:

  /*!
   \brief Display transition
   \param t : transition
   \post Attributes of transition t in _zg have been output to _os
   */
  void output(tchecker::zg::const_transition_sptr_t const & t);

};

#if USE_BOOST_JSON

/*!
 \class concrete_json_display_t
 \brief JSON display for concrete simulation
*/
class concrete_json_display_t : public concrete_display_t {
 public:
  /*!
   \brief Constructor
   \param os : output stream
   \param zg : zone graph
   \pre zg is not nullptr
   \post this keeps a reference on os and a pointer to zg
   \throw std::invalid_argument : if zg is nullptr
  */
  concrete_json_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg);

  /*!
   \brief Copy constructor
  */
  concrete_json_display_t(concrete_json_display_t const &) = default;

  /*!
   \brief Move constructor
  */
  concrete_json_display_t(concrete_json_display_t &&) = default;

  /*!
   \brief Destructor
  */
  virtual ~concrete_json_display_t() = default;

  /*!
   \brief Assignment operator
  */
  concrete_json_display_t & operator=(concrete_json_display_t const &) = delete;

  /*!
   \brief Move-assignment operator
  */
  concrete_json_display_t & operator=(concrete_json_display_t &&) = delete;

  void output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v) override;

  void output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v, 
                   bool finite_max_delay, tchecker::clock_rational_value_t max_delay) override;

  void output_state(tchecker::zg::const_state_sptr_t const & s) override;

private:

  /*!
 \brief Display state
 \param s : state
 \post Attributes of state s in _zg have been output to _os
  */
  boost::json::value output(tchecker::zg::const_state_sptr_t const & s);

  /*!
   \brief Display transition
   \param t : transition
   \post Attributes of transition t in _zg have been output to _os
   */
  boost::json::value output(tchecker::zg::const_transition_sptr_t const & t);

};

/*!
 \brief Display factory
 \param display_type : type of display
 \param os : output stream
 \param zg : zone graph
 \return display of type display_type over output stream os using zone graph zg
*/
concrete_display_t * concrete_display_factory(enum tchecker::simulate::display_type_t display_type, std::ostream & os,
                                                    std::shared_ptr<tchecker::zg::zg_t> zg);
#endif

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker

#endif