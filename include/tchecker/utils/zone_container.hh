/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_ZONE_CONTAINER_HH
#define TCHECKER_ZG_ZONE_CONTAINER_HH

#include <iterator>

#include "tchecker/zg/zone.hh"
#include "tchecker/vcg/zone.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

// forward declaration
namespace virtual_constraint {

class virtual_constraint_t;

}

/*
 \brief a container for all subtypes of zone
 */
template <typename T>
class zone_container_t {

public:

  /*!
   \brief Constructor
   \param dim : dimension
   */
  zone_container_t(tchecker::clock_id_t dim) : _dim(dim){ };

  /*!
   \brief factory functions to be used by the append functions
   \note need for specialisation here!
  */
  T * create_element() {};

  T * create_element(T const &zone) {};

  /*!
   \brief destructor of zone. Calling the destructor of tchecker::zg::zone_t
   \note If T extends tchecker::zg::zone_t by a datastructure, there is a need for 
   specialisation here!
  */
  void destruct_element(T *zone)
  {
    tchecker::zg::zone_destruct_and_deallocate(zone);
  }

  /*
   \brief check for emptiness of the container
   \return true if and only if the container is empty
   */
  bool is_empty()
  {
    return 0 == _storage.size();
  }

  /*!
   \brief adds an universal zone to the container
   */
  void append_zone()
  {
    _storage.emplace_back(create_element());
  }

  /*!
   \brief adds a copy of the zone to the container
   */
  void append_zone(T const & zone)
  {
    _storage.emplace_back(create_element(zone));
  }


  /*!
   \brief adds the pointer to the container
   \pre *zone must be initialised
   \post zone is part of the container
   */
  void append_zone(T *zone)
  {
    _storage.emplace_back(zone);
  }

  /*!
   \brief removes the last zone and deletes the content
   */
  void remove_back()
  {
    destruct_element(*(_storage.begin()));
    _storage.erase(_storage.begin());
  }

  /*!
   \brief gets the the last zone
   \return the pointer to that zone
   */
  T * back()
  {
    return _storage.back();
  }

  /*!
   \brief Accessor of the ith element
   \param i : the index to access
   \return a ptr to the ith element
   */
  T * operator[](typename std::vector<T *>::size_type i)
  {
    return _storage[i];
  }

  /*!
   \brief Accessor
   \return the number of elements stored in this container
   */
  typename std::vector<T *>::size_type size()
  {
    return _storage.size();
  }

  /*!
   \brief returns an iterator, starting the begining, iterating over ptrs of zone_t
   */
  typename std::vector<T *>::iterator begin()
  {
    return _storage.begin();
  }

  /*!
   \brief returns an iterator, starting after the end, iterating over ptrs of zone_t
   */
  typename std::vector<T *>::iterator end()
  {
    return _storage.end();
  }

  /*!
   \brief Destructor
   */
  ~zone_container_t()
  {
    for(auto iter = begin(); iter < end(); ++iter) {
      destruct_element(*iter);
    }
  }

private:
  tchecker::clock_id_t _dim;
  std::vector<T *> _storage;

};

// specializations
template<>
tchecker::zg::zone_t * zone_container_t<tchecker::zg::zone_t>::create_element();

template<>
tchecker::zg::zone_t * zone_container_t<tchecker::zg::zone_t>::create_element(tchecker::zg::zone_t const & zone);

template<>
tchecker::vcg::zone_t * zone_container_t<tchecker::vcg::zone_t>::create_element();

template<>
tchecker::vcg::zone_t * zone_container_t<tchecker::vcg::zone_t>::create_element(tchecker::vcg::zone_t const & zone);

template<>
tchecker::virtual_constraint::virtual_constraint_t * zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element();

template<>
tchecker::virtual_constraint::virtual_constraint_t * zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element(tchecker::virtual_constraint::virtual_constraint_t const & zone);

/*!
 \brief contained-in-all function (see the TR of Lieb et al.)
 \param a vector of vector of zones
 \return a vector of zones
 */
zone_container_t<tchecker::vcg::zone_t> contained_in_all(std::vector<zone_container_t<tchecker::vcg::zone_t>> & zones, tchecker::clock_id_t dim);


} // end of namespace tchecker

#endif
