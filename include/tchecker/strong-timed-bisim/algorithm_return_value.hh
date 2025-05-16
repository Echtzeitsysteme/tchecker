/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
*/

#ifndef TCHECKER_STRONG_TIMED_BISIM_ALGORITHM_RETURN_VALUE
#define TCHECKER_STRONG_TIMED_BISIM_ALGORITHM_RETURN_VALUE


#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/strong-timed-bisim/visited_map.hh"

namespace tchecker{

namespace strong_timed_bisim {

/*!
 \class algorithm_return_value
 \brief the data structure that is returned by the virtual clock algorithm
 */
class algorithm_return_value_t {

public:

  /*!
   \brief Default Constructor, initiates the data structure with empty contradictions and empty check set
  */
  algorithm_return_value_t(tchecker::clock_id_t no_of_virt_clocks);

  /*!
   \brief Copy Constructor
  */
  algorithm_return_value_t(algorithm_return_value_t& other);

  /*!
   \brief Constructor
   \param contradictions : the set of contradictions
   */
  algorithm_return_value_t(
    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions);

  /*!
   \brief checks whether the contradiction set is empty
   \return true if and only if the contradiction set is empty
   */
  bool contradiction_free();

  /*!
   \brief returns a copy of the set of contradictions
   \return the contradiction set
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  get_contradictions();

  /*!
   \brief adds an element to the set of contradictions
   \param to_add : the element to add
  */
  void add_to_contradictions(tchecker::virtual_constraint::virtual_constraint_t& to_add);

  /*!
   \brief adds an element to the set of contradictions
   \param to_add : the element to add
  */
  void add_to_contradictions(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_add);

  /*!
   \brief adds an element to the set of contradictions
   \param to_add : the element to add
  */
  void add_to_contradictions(tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>& to_add);

  /*!
   \brief adds an element to the set of contradictions
   \param to_add : the element to add
  */
  void add_to_contradictions(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_add);

private:

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> _contradictions;
};
    
}
}


#endif