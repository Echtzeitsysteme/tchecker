/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_ZONE_CONTAINER_HH
#define TCHECKER_ZG_ZONE_CONTAINER_HH

#include <iterator>

#include "tchecker/vcg/zone.hh"

namespace tchecker {

namespace vcg {

class zone_container_t {

public:

  /*!
   \brief Constructor
   \param dim : dimension
   */
  zone_container_t(tchecker::clock_id_t dim);

  /*
   \brief check for emptiness of the container
   \return true if and only if the container is empty
   */

  bool is_empty();

  /*!
   \brief adds an universal zone to the container
   */
  void append_zone();

  /*!
   \brief adds a copy of the zone to the container
   */
  void append_zone(zone_t const & zone);

  /*!
   \brief adds the pointer to the container
   \pre *zone must be initialised
   \post zone is part of the container
   */
  void append_zone(zone_t *zone);

  /*!
   \brief removes the last zone and deletes the content
   */
  void remove_back();

  /*!
   \brief gets the the last zone
   \return the pointer to that zone
   */
  zone_t * back();

  /*!
   \brief Accessor of the ith element
   \param i : the index to access
   \return a ptr to the ith element
   */
  zone_t * operator[](std::vector<zone_t>::size_type i);

  /*!
   \brief Accessor
   \return the number of elements stored in this container
   */
  std::vector<zone_t>::size_type size();

  /*!
   \brief Destructor
   */
  ~zone_container_t();

  /*!
   \brief returns an iterator, starting the begining, iterating over ptrs of zone_t
   */
  std::vector<zone_t *>::iterator begin();

  /*!
   \brief returns an iterator, starting after the end, iterating over ptrs of zone_t
   */
  std::vector<zone_t *>::iterator end();

private:
  tchecker::clock_id_t _dim;
  std::vector<zone_t *> _storage;

};

} // end of namespace vcg

} // end of namespace tchecker

#endif
