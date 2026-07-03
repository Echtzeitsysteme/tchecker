/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_STRATEGY_HH
#define TCHECKER_ALGORITHMS_STRATEGY_HH

#include "tchecker/strong-timed-bisim/non_bisim_cache.hh"
#include "tchecker/strong-timed-bisim/visited_map.hh"
#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace strategy {

/*!
  \brief A helping class that represents symbolic states of the product that shall be checked
*/
class state_to_check_t {
 public:

  state_to_check_t(const std::string & vloc_str, const tchecker::intval_t & intval,
                   tchecker::intrusive_shared_ptr_t<const tchecker::zg::shared_zone_t> zone);

  state_to_check_t();
  state_to_check_t(state_to_check_t const & other);
  state_to_check_t & operator=(state_to_check_t const & other);
  state_to_check_t(state_to_check_t&& other) noexcept;
  state_to_check_t & operator=(state_to_check_t&& other) noexcept;
  ~state_to_check_t();
    
  std::string vloc() {return _vloc;}
  tchecker::intval_t * intval_ptr() {return _intval_ptr;}
  tchecker::zg::zone_t *zone_ptr() {return _zone_ptr;}
 private:
  std::string _vloc;
  tchecker::intval_t * _intval_ptr;
  tchecker::zg::zone_t *_zone_ptr;
};

/*!
 \class strategy_t
 \brief This class replaces the certificates in case a set of symbolic states is given that should be checked.
 */
class strategy_t {
 public:
  /*!
   \brief Constructor
   \param _A : the first vcg
   \param _B : the second vcg
   \param symbolic_states_to_check : the symbolic states that should be checked
   */
  strategy_t(std::shared_ptr<tchecker::vcg::vcg_t> A, 
             std::shared_ptr<tchecker::vcg::vcg_t> B,
             std::vector<std::shared_ptr<tchecker::strong_timed_bisim::strategy::state_to_check_t>> & symbolic_states_to_check,
             tchecker::clock_id_t first_vloc_size,
             tchecker::clock_id_t second_vloc_size,
             unsigned short first_intval_size,
             unsigned short second_intval_size);

  /*!
   \brief Adds the already checked symbolic states to the strategy
   \param non_bisim_cache : The cache that contains all checked symbolic states that are not bisimilar
   \param visited_map : The cache that contains all checked symbolic states that are bisimilar
   \post All checked symbolic states are part of this.
   */
  void insert_symb_states(std::shared_ptr<non_bisim_cache::non_bisim_cache_t> non_bisim_cache, std::shared_ptr<visited_map_t> visited_map);

  /*!
   \brief Get a pair of symbolic states that is not contained by the strategy yet
   \param a reference to a pair, where the result is stored into
   \return true, if there exists such a pair. False, otherwise.
   */
  std::shared_ptr<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>> get_non_contained_states();

  /*!
   \brief prints the strategy
   \param os : output stream
   \post Strategy has been output to os
  */
  std::ostream & strategy_output(std::ostream & os);

 private:

  class entry_t {
   public:

    entry_t(std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> loc_pair, 
           tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & container)
     : _loc_pair(loc_pair)
    { 
      _container = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(container);
    }
    entry_t(std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> loc_pair,
          tchecker::clock_id_t dim)
     : _loc_pair(loc_pair)
    { 
      _container = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(dim);
    }
    std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> loc_pair() {return _loc_pair;}
    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> container() {return _container;}

    void append_vc(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc) {_container->append_zone(vc);}

   private:
    std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> _loc_pair;
    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> _container;
  };

  std::shared_ptr<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>> 
  get_non_contained_states(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair, 
                           std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
  find_non_contained(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_be_contained, 
                     tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & container,
                     std::size_t idx);

  std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> 
  extract_location_pair(std::string vloc, tchecker::intval_t * intval);

  void output_non_bisim(std::ostream & os);

  void output_bisim(std::ostream & os);

  std::shared_ptr<tchecker::vcg::vcg_t> _A;
  std::shared_ptr<tchecker::vcg::vcg_t> _B;

  // states to check contains a pair of states and a zone_container that contains the virtual constraints to check.
  std::vector<std::shared_ptr<entry_t>> _states_to_check;
  std::shared_ptr<non_bisim_cache::non_bisim_cache_t> _non_bisim_cache;
  std::shared_ptr<visited_map_t> _visited_map;
  tchecker::clock_id_t _first_vloc_size;
  tchecker::clock_id_t _second_vloc_size;
  unsigned short _first_intval_size;
  unsigned short _second_intval_size;
};

} // end of namespace strategy

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif