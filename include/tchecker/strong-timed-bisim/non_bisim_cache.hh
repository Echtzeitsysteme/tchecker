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
  already_cached(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second);

 private:
  
  // we would like to use a pair of ta states but we are unable to extract them from symbolic states :(
  using map_key_t = std::pair<std::pair<tchecker::intval_sptr_t, tchecker::vloc_sptr_t>, std::pair<tchecker::intval_sptr_t, tchecker::vloc_sptr_t>>;

  struct custom_hash {
    size_t operator()(const map_key_t &to_hash) const {
        size_t h = TCHECKER_STRONG_TIMED_BISIM_NON_BISIM_CACHE_HH_SEED;
        boost::hash_combine(h, *to_hash.first.first);
        boost::hash_combine(h, *to_hash.first.second);
        boost::hash_combine(h, *to_hash.second.first);
        boost::hash_combine(h, *to_hash.second.second);
        return h;
    }
  };

  struct custom_equal {
    bool operator() (const map_key_t &p1, const map_key_t &p2) const {
      return *p1.first.first == *p2.first.first
              && *p1.first.second == *p2.first.second
              && *p1.second.first == *p2.second.first
              && *p1.second.second == *p2.second.second;
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

}; // end of class non_bisim_cache

} // end of namespace strong_timed_bisim


 } // end of namespace tchecker

 #endif