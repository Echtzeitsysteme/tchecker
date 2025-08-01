/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_VISITED_MAP_HH
#define TCHECKER_STRONG_TIMED_BISIM_VISITED_MAP_HH

#define TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED 0xDEADBEEF

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/ta/state.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

/*
 \brief unordered map containing visited pairs of symbolic states
*/
class visited_map_t {

private:

    /*
     \brief key type for visited map. A key is a pair of two pairs consisting of one intval and one vloc each
     \note due to mismodeling we were unable to use a pair of tchecker::ta::state_t.
    */
    using visited_map_key_t = std::pair<std::pair<tchecker::intval_sptr_t, tchecker::vloc_sptr_t>, std::pair<tchecker::intval_sptr_t, tchecker::vloc_sptr_t>>;

    struct custom_hash {
    size_t operator()(const visited_map_key_t &to_hash) const {
        size_t h = TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED;
        boost::hash_combine(h, *to_hash.first.first);
        boost::hash_combine(h, *to_hash.first.second);
        boost::hash_combine(h, *to_hash.second.first);
        boost::hash_combine(h, *to_hash.second.second);
        return h;
    }
    };

    struct custom_equal {
    bool operator() (const visited_map_key_t &p1, const visited_map_key_t &p2) const {
        return *p1.first.first == *p2.first.first
                && *p1.first.second == *p2.first.second
                && *p1.second.first == *p2.second.first
                && *p1.second.second == *p2.second.second;
    }
    };

    /*
     \brief unordered map mapping a visited_map_key_t to a of zone container of virtual constraints
    */
    using visited_map_storage_t = std::unordered_map<visited_map_key_t,
                                                    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>, 
                                                    custom_hash, 
                                                    custom_equal>;

    const tchecker::clock_id_t _no_of_virtual_clocks;
    std::shared_ptr<visited_map_storage_t> _storage;
    

public:

    /*!
     \brief Constructor
     \param no_of_virtual_clocks : number of virtual clocks for the states stored in the visited map
    */
    visited_map_t(tchecker::clock_id_t no_of_virtual_clocks);

    /*!
     \brief Copy Constructor
     \param t : the visited map to copy
    */
    visited_map_t(visited_map_t &t);

    /*!
     \brief Accessor
    */
    tchecker::clock_id_t const no_of_virtual_clocks(); 

    /*!
     \brief returns an iterator, starting the beginning, iterating over pairs of one visited_map_key_t and one zone container ptr each
    */
    visited_map_storage_t::iterator begin();

    /*!
     \brief returns an iterator, starting after the end, iterating over pairs of one visited_map_key_t and one zone container ptr each
    */
    visited_map_storage_t::iterator end();

    /*!
     \brief inserts given pair of symbolic states into visited map
     \param first : first symbolic state
     \param second : second symbolic state
    */
    void emplace(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second);

    /*!
     \brief inserts another visited map into this
     \param other : the map to insert
    */
    void emplace(visited_map_t &other);

    /*!
    \brief checks whether the two given symbolic states form a subset of the symbolic states saved in visited map 
    \param first : first symbolic state
    \param second : second symbolic state
    \note only returns true if visited map contains superset but might return false incorrectly. Always returns true if the given pair of symbolic states form a subset of a single pair of symbolic states in visited map
    */
    bool contains_superset(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second);

private:

    void emplace(visited_map_key_t key, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc);

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif