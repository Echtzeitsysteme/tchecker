/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_SYMBOLIC_SYMBOLIC_DISPLAY_HH
#define TCHECKER_TCK_SIMULATE_SYMBOLIC_SYMBOLIC_DISPLAY_HH

/*!
 \file simbolic_display.hh
 \brief State/transition display
*/

#include <memory>
#include <ostream>
#include <vector>

#include "tchecker/config.hh"
#include "tchecker/zg/state.hh"
#include "tchecker/zg/transition.hh"
#include "tchecker/zg/zg.hh"
#include "tchecker/simulate/simulate.hh"

namespace tchecker {

namespace simulate {

namespace symbolic {

/*!
 \class display_t
 \brief Interface to state/transition display
*/
class display_t {
public:
  /*!
   \brief Destructor
  */
  virtual ~display_t() = default;

  /*!
   \brief Display initial simulation step
   \param v : collection of initial triples (status, state, transition)
  */
  virtual void output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v) = 0;

  /*!
   \brief Display simulation next step
   \param s : current state
   \param v : collection of next triples (status, state, transition)
   */
  virtual void output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v) = 0;

  /*!
   \brief Display state
   \param s : state
   \post Attributes of state s in _zg have been output to _os
  */
  virtual void output_state(tchecker::zg::const_state_sptr_t const & s) = 0;
};

/*!
 \class hr_display_t
 \brief Human-readable display
*/
class hr_display_t : public display_t {
public:
  /*!
   \brief Constructor
   \param os : output stream
   \param zg : zone graph
   \pre zg is not nullptr
   \post this keeps a reference on os and a pointer to zg
   \throw std::invalid_argument : if zg is nullptr
  */
  hr_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg);

  /*!
   \brief Copy constructor
  */
  hr_display_t(hr_display_t const &) = default;

  /*!
   \brief Move constructor
  */
  hr_display_t(hr_display_t &&) = default;

  /*!
   \brief Destructor
  */
  virtual ~hr_display_t() = default;

  /*!
   \brief Assignment operator
  */
  hr_display_t & operator=(hr_display_t const &) = delete;

  /*!
   \brief Move-assignment operator
  */
  hr_display_t & operator=(hr_display_t &&) = delete;

  /*!
   \brief Display initial simulation step
   \param v : collection of initial triples (status, state, transition)
   \post Attributes of all states and transitions in v have been output to _os
  */
  virtual void output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v);

  /*!
   \brief Display simulation next step
   \param s : current state
   \param v : collection of successor triples (status, state, transition)
   \post Attributes of state s,  and all states and transitions in v have been output to _os
   */
  virtual void output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v);

  void output_state(tchecker::zg::const_state_sptr_t const & s);

private:
  /*!
   \brief Display transition
   \param t : transition
   \post Attributes of transition t in _zg have been output to _os
   */
  void output(tchecker::zg::const_transition_sptr_t const & t);

  std::ostream & _os;                      /*!< Output stream */
  std::shared_ptr<tchecker::zg::zg_t> _zg; /*!< Zone graph */
};

#if USE_BOOST_JSON
/*!
 \class json_display_t
 \brief JSON display
*/
class json_display_t : public display_t {
public:
  /*!
   \brief Constructor
   \param os : output stream
   \param zg : zone graph
   \pre zg is not nullptr
   \post this keeps a reference on os and a pointer to zg
   \throw std::invalid_argument : if zg is nullptr
  */
  json_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg);

  /*!
   \brief Copy constructor
  */
  json_display_t(json_display_t const &) = default;

  /*!
   \brief Move constructor
  */
  json_display_t(json_display_t &&) = default;

  /*!
   \brief Destructor
  */
  virtual ~json_display_t() = default;

  /*!
   \brief Assignment operator
  */
  json_display_t & operator=(json_display_t const &) = delete;

  /*!
   \brief Move-assignment operator
  */
  json_display_t & operator=(json_display_t &&) = delete;

  /*!
   \brief Display initial simulation step
   \param v : collection of initial triples (status, state, transition)
   \post Attributes of all states and transitions in v have been output to _os
  */
  virtual void output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v);

  /*!
   \brief Display simulation next step
   \param s : current state
   \param v : collection of successor triples (status, state, transition)
   \post Attributes of state s as well as all states and transitions in v have been output to _os
   */
  virtual void output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v);

  void output_state(tchecker::zg::const_state_sptr_t const & s);

protected:
  std::ostream & _os;                      /*!< Output stream */
  std::shared_ptr<tchecker::zg::zg_t> _zg; /*!< Zone graph */
};

#endif

/*!
 \brief Display factory
 \param display_type : type of display
 \param os : output stream
 \param zg : zone graph
 \return display of type display_type over output stream os using zone graph zg
*/
display_t * display_factory(enum tchecker::simulate::display_type_t display_type, std::ostream & os,
                                                    std::shared_ptr<tchecker::zg::zg_t> zg);

} // end of namespace symbolic                                                    

} // end of  namespace simulate

} // end of namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_DISPLAY_HH