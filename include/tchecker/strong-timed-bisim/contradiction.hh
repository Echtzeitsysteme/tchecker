/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
*/

#ifndef TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION
#define TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION


#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/strong-timed-bisim/visited_map.hh"

namespace tchecker{

namespace strong_timed_bisim {

/*!
 \class algorithm_return_value
 \brief the data structure that is returned by the virtual clock algorithm
 */
class contradiction_t {

public:

  /*!
   \brief Default Constructor, initiates the data structure with empty contradictions and empty check set
   \param no_of_virt_clocks : the number of virtual clocks
   \param min_steps_to_cont : initial _min_steps_to_cont value
  */
  contradiction_t(tchecker::clock_id_t no_of_virt_clocks, tchecker::integer_t min_steps_to_cont = tchecker::int_maxval);

  /*!
   \brief Copy Constructor
  */
  contradiction_t(contradiction_t const & other);

  /*!
   \brief Constructor
   \param contradictions : the set of contradictions
   \param min_steps_to_cont : the minimum number of steps to a contradiction
   */
  contradiction_t(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions, tchecker::integer_t min_steps_to_cont);

  /*!
   \brief Constructor
   \param contradiction : a contradiction
   \param min_steps_to_cont : the minimum number of steps to a contradiction
   */
  contradiction_t(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> contradiction, tchecker::integer_t min_steps_to_cont);

  /*!
   \brief checks whether the contradiction set is empty
   \return true if and only if the contradiction set is empty
   */
  bool contradiction_free();

  /*!
   \brief returns a copy of the set of contradictions
   \return the contradiction set
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> const
  get_contradictions() const;

  /*!
   \brief adds an element to the set of contradictions
   \param to_add : the element to add
  */
  void add_contradiction(contradiction_t const & to_add);

  /*!
   \brief setter
   \param min_steps_to_cont : the new value of min_steps_to_cont
   */
  void set_min_steps_to_cont(tchecker::integer_t min_steps_to_cont) {_min_steps_to_cont = min_steps_to_cont;}

  /*!
   \brief Getter
   \return _min_steps_to_cont
   */
  tchecker::integer_t min_steps_to_cont() {return _min_steps_to_cont;}

private:

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> _contradictions;
  tchecker::integer_t _min_steps_to_cont;
};


} // end of namespace strong_timed_bisim

} // end of namespace tchecker


#endif