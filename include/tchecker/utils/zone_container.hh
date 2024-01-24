/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_ZONE_CONTAINER_HH
#define TCHECKER_ZG_ZONE_CONTAINER_HH

#include <iterator>
#include <memory>

#include "tchecker/zg/zone.hh"
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
  zone_container_t<T>(tchecker::clock_id_t dim) : _dim(dim){ }

  /*!
   \brief Copy Constructor
   \param container : the container to copy
   */
  zone_container_t<T>(zone_container_t<T> &t) : _dim(t.dim()), _storage(0)
  {
    for(auto iter = t.begin(); iter < t.end(); iter++) {
      this->append_zone(*(*iter));
    }
  }

  /*!
   \brief Accessor
   */
  tchecker::clock_id_t dim() {return this->_dim;}

  /*!
   \brief factory functions to be used by the append functions
   \note need for specialisation here!
  */
  std::shared_ptr<T> create_element();

  std::shared_ptr<T> create_element(T const &zone);

  /*!
   \brief destructor of zone. Calling the destructor of tchecker::zg::zone_t
   \note If T extends tchecker::zg::zone_t by a datastructure, there is a need for 
   specialisation here!
  */
  void destruct_element(std::shared_ptr<T> zone)
  {
    tchecker::zg::zone_destruct_and_deallocate(&(*zone));
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
    if(zone.dim() != _dim) {
      std::ostringstream msg;
      msg << "appending a zone with wrong dimension. Is " << zone.dim() << " should be " << _dim;
      throw std::runtime_error(msg.str());
    }
    _storage.emplace_back(create_element(zone));
  }


  /*!
   \brief adds the pointer to the container
   \pre *zone must be initialised
   \post zone is part of the container
   */
  void append_zone(std::shared_ptr<T> zone)
  {
    if(zone->dim() != _dim) {
      std::ostringstream msg;
      msg << "appending a zone with wrong dimension. Is " << zone->dim() << " should be " << _dim;
      throw std::runtime_error(msg.str());
    }
    _storage.emplace_back(zone);
  }

  /*!
   \brief adds the elements of other to this
   \param other: the container to append
   \post other is appended to the container
   */
  void append_container(std::shared_ptr<zone_container_t<T>> other)
  {
    for(auto iter = other->begin(); iter < other->end(); iter++) {
      this->append_zone(*iter);
    }
  }

  /*!
   \brief removes the last zone and deletes the content
   */
  void remove_first()
  {
    destruct_element(*(_storage.begin()));
    _storage.erase(_storage.begin());
  }

  /*!
   \brief removes all empty zones
   */
  void remove_empty()
  {
    for(auto iter = this->begin(); iter < this->end(); iter++) {
      if(iter->empty()) {
        _storage.erase(iter);
      }
    }
  }

  /*!
   \brief gets the the last zone
   \return the pointer to that zone
   */
  std::shared_ptr<T> back()
  {
    return _storage.back();
  }

  /*!
   \brief Accessor of the ith element
   \param i : the index to access
   \return a ptr to the ith element
   */
  std::shared_ptr<T> operator[](typename std::vector<std::shared_ptr<T>>::size_type i)
  {
    return _storage[i];
  }

  /*!
   \brief Accessor
   \return the number of elements stored in this container
   */
  typename std::vector<std::shared_ptr<T>>::size_type size()
  {
    return _storage.size();
  }

  /*!
   \brief returns an iterator, starting the begining, iterating over ptrs of zone_t
   */
  typename std::vector<std::shared_ptr<T>>::iterator begin()
  {
    return _storage.begin();
  }

  /*!
   \brief returns an iterator, starting after the end, iterating over ptrs of zone_t
   */
  typename std::vector<std::shared_ptr<T>>::iterator end()
  {
    return _storage.end();
  }

  /*!
   \brief Destructor
   */
  ~zone_container_t<T>()
  {
    for(auto iter = begin(); iter < end(); ++iter) {
      destruct_element(*iter);
    }
  }

private:
  const tchecker::clock_id_t _dim;
  std::vector<std::shared_ptr<T>> _storage;

};

// specializations
template<>
std::shared_ptr<tchecker::zg::zone_t> zone_container_t<tchecker::zg::zone_t>::create_element();

template<>
std::shared_ptr<tchecker::zg::zone_t> zone_container_t<tchecker::zg::zone_t>::create_element(tchecker::zg::zone_t const & zone);

template<>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element();

template<>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element(tchecker::virtual_constraint::virtual_constraint_t const & zone);


} // end of namespace tchecker

#endif
