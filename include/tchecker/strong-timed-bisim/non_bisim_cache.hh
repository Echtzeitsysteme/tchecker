/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
*/

#ifndef TCHECKER_STRONG_TIMED_BISIM_NON_BISIM_CACHE_HH
#define TCHECKER_STRONG_TIMED_BISIM_NON_BISIM_CACHE_HH

#define TCHECKER_STRONG_TIMED_BISIM_NON_BISIM_CACHE_HH_SEED 0xDEADBEEF


#include "tchecker/basictypes.hh"
#include "tchecker/zg/state.hh"
#include "tchecker/zg/zone_container.hh"

namespace tchecker {

namespace strong_timed_bisim {

class non_bisim_cache_t {
 public:
  /*!
   \brief Constructor
   \param no_of_virtual_clocks : number of virtual clocks for the states stored in the visited map
  */
   non_bisim_cache_t(tchecker::clock_id_t no_of_virtual_clocks);

  /*!
   \brief inserts given pair of symbolic states with found contradiction
   \param first : first symbolic state
   \param second : second symbolic state
   \param con : found contradiction
  */
  void emplace(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second, std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> con);

  /*!
   \brief checks whether we have already stored a contradiction for this 
   \param first : first symbolic state
   \param second : second symbolic state
   \return a cached contradiction
  */  
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  already_cached(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second) const;

  /*!
   \brief Accessor
   \param first : the first TA state
   \param second : the second TA state;
   \return The entry corresponding to the given key
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  entry(tchecker::ta::state_t & first, tchecker::ta::state_t & second) const;

  /*!
   \brief Accessor
   \param loc_pair : the pair of TA states
   \return The entry corresponding to the given key
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  entry(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair) const;

  /*!
   \brief checks whether the location pair with this valuation for the virtual clock is element of a cached entry
   \param loc_pair : the pair of TA states
   \param clockval : the valuation of the virtual clocks
   */
  bool is_cached(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair, std::shared_ptr<tchecker::clockval_t> clockval, 
                 tchecker::clock_id_t no_of_orig_clks, tchecker::clock_id_t no_of_orig_clks_2, bool first_not_second = true) const;

  /*!
   \brief getter
   \return returns the number of calls of emplace
  */  
  uint64_t no_of_entries() {
    return _no_of_entries;
  }

 private:
  
  using map_key_t = std::pair<tchecker::ta::state_t, tchecker::ta::state_t>;

  struct custom_hash {
    size_t operator()(const map_key_t & to_hash) const
    {
      size_t h = TCHECKER_STRONG_TIMED_BISIM_NON_BISIM_CACHE_HH_SEED;
      boost::hash_combine(h, to_hash.first);
      boost::hash_combine(h, to_hash.second);
      return h;
    }
  };

  struct custom_equal {
    bool operator()(const map_key_t & p1, const map_key_t & p2) const
    {
      return p1.first == p2.first && p1.second == p2.second;
    }
  };

  /*
   \brief unordered map mapping a map_key_t to a of zone container of virtual constraints
  */
  using storage_t = std::unordered_map<map_key_t,
                                       std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>, 
                                       custom_hash, 
                                       custom_equal>;
                                                

  const tchecker::clock_id_t _no_of_virtual_clocks;
  std::shared_ptr<storage_t> _storage;

  uint64_t _no_of_entries;

}; // end of class non_bisim_cache

} // end of namespace strong_timed_bisim


 } // end of namespace tchecker

 #endif