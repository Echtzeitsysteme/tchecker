/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_ZONE_CONTAINER_HH
#define TCHECKER_ZG_ZONE_CONTAINER_HH

// exactly one of WITHOUT_SUBSETS, SUBSETS_WITHOUT_COMPRESS, SUBSETS_WITH_COMPRESS and SUBSETS_WITH_INTERSECTIONS must be defined
#define WITHOUT_SUBSETS

#include <iterator>
#include <memory>

#include "tchecker/dbm/dbm.hh"

namespace tchecker {

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
  zone_container_t(tchecker::clock_id_t dim) : _dim(dim), _storage(std::make_shared<std::vector<std::shared_ptr<T>>>(0)) { }

  /*!
   \brief Constructor
   \param zone : the only zone of the resulting container
   */
  zone_container_t(T const &zone) : _dim(zone.dim()), _storage(std::make_shared<std::vector<std::shared_ptr<T>>>(0)) {
    this->append_zone(zone);
  }

  /*!
   \brief Copy Constructor
   \param container : the container to copy
   */
  zone_container_t(zone_container_t<T> &t) : _dim(t.dim()), _storage(std::make_shared<std::vector<std::shared_ptr<T>>>(0))
  {
    for(auto iter = t.begin(); iter < t.end(); iter++) {
      this->append_zone(*(*iter));
    }
  }

  /*!
   \brief Accessor
   */
  tchecker::clock_id_t dim() const {return this->_dim;}

  /*!
   \brief factory functions to be used by the append functions
   \note need for specialisation here!
  */
  std::shared_ptr<T> create_element();

  std::shared_ptr<T> create_element(T const &zone);

  /*
   \brief check for emptiness of the container
   \return true if and only if the container is empty
   */
  bool is_empty() const
  {
    return 0 == _storage->size();
  }

  /*!
   \brief adds an universal zone to the container
   */
  void append_zone()
  {
    _storage->emplace_back(create_element());
  }

  /*!
   \brief adds a copy of the zone to the container
   */
  void append_zone(T const & zone)
  {
    assert(_dim == zone.dim());
    _storage->emplace_back(create_element(zone));
  }


  /*!
   \brief adds the pointer to the container
   \pre *zone must be initialised
   \post zone is part of the container
   */
  void append_zone(std::shared_ptr<T> zone)
  {
    assert(_dim == zone->dim());
    _storage->emplace_back(zone);
  }

  /*!
   \brief adds the elements of other to this
   \param other: the container to append
   \post other is appended to the container
   */
  void append_container(std::shared_ptr<zone_container_t<T>> other)
  {
    assert(nullptr != other);
    assert(other->_dim == this->_dim);
    for(auto iter = other->begin(); iter < other->end(); iter++) {
      this->append_zone(*iter);
    }
  }

  /*!
   \brief adds the elements of other to this. The elements that can be accessed are the same!
   \param other: the container to append
   \post other is appended to the container
   */
  void append_container(zone_container_t<T> &other)
  {
    assert(other._dim == this->_dim);
    for(auto iter = other.begin(); iter < other.end(); iter++) {
      this->append_zone(*iter);
    }
  }

  /*!
   \brief removes the last zone and deletes the content
   */
  void remove_first()
  {
    destruct_element(*(_storage.begin()));
    _storage->erase(_storage.begin());
  }

  /*!
   \brief removes all empty zones
   */
  void remove_empty()
  {
    for(auto iter = this->begin(); iter < this->end(); iter++) {
      if((*iter)->is_empty()) {
        _storage->erase(iter);
       this->remove_empty();
       return;
      }
    }
  }

  /*!
   \brief gets the last zone
   \return the pointer to that zone
   */
  std::shared_ptr<T> back()
  {
    return _storage->back();
  }

  /*!
   \brief Accessor of the ith element
   \param i : the index to access
   \return a ptr to the ith element
   */
  std::shared_ptr<T> operator[](typename std::vector<std::shared_ptr<T>>::size_type i)
  {
    return (*_storage)[i];
  }

  /*!
   \brief Accessor
   \return the number of elements stored in this container
   */
  typename std::vector<std::shared_ptr<T>>::size_type size()
  {
    return _storage->size();
  }

  /*!
   \brief returns an iterator, starting the begining, iterating over ptrs of zone_t
   */
  typename std::vector<std::shared_ptr<T>>::iterator begin()
  {
    return _storage->begin();
  }

  /*!
   \brief returns an iterator, starting after the end, iterating over ptrs of zone_t
   */
  typename std::vector<std::shared_ptr<T>>::iterator end()
  {
    return _storage->end();
  }

  /*!
   \brief compresses the zone container if possible
   \post let zc_prev be the zone container before the call and zc_after the zone container after. The following conditions hold:
           - zc_prev._dim = zc_after._dim
           - for all zone_prev in zc_prev : for all u in zone_prev : exists zone_after in zc_after : u in zone_after
           - for all zone_after in zc_after : for all u in zone_after : exists zone_prev in zc_prev : u in zone_prev
   \note see chapter 7.6 of Rokicki "Representing and modeling digital circuits" (PhD thesis)
   */

  void compress()
  {

    this->remove_empty();

    std::shared_ptr<std::vector<std::shared_ptr<T>>> result = _storage;

    bool reduced;

    do {
      auto prev = result->size();
      result = find_union_partner(*result);
      reduced = (result->size() < prev);
    } while (reduced);

    assert(result->size() <= _storage->size());

    _storage = result;

  }

  void print_zone_container(std::ostream & os)
  {
    for(auto cur : *_storage) {
      tchecker::dbm::output_matrix(os, cur->dbm(), _dim);
    }
  }

  /*!
   \brief checks whether the given zone is a subset of the union of all zones in the zone container (approximately)
   \note only returns true if container is superset but might return false incorrectly. Always returns true if given zone is a subset of a single element in the zone container
   */
  bool is_superset(T const & zone)
  { 
    #if defined(SUBSETS_WITH_INTERSECTIONS)
      assert(this->dim() == zone.dim());
      auto intersections = std::make_shared<zone_container_t<T>>(_dim);

      for(auto current_zone = _storage->begin(); current_zone < _storage->end(); ++current_zone) {
        tchecker::dbm::db_t intersection[this->dim() * this->dim()];
        if(tchecker::dbm::NON_EMPTY == tchecker::dbm::intersection(intersection, (*current_zone)->dbm(), zone.dbm(), _dim)) {
          intersections->append_zone();
          tchecker::dbm::copy(intersections->back()->dbm(), intersection, this->dim());
        } 
      }

      intersections->compress();

      // since all elements of intersections are subsets of zone, we know that if zone is an element of intersections, it must be the only (and therefore first) element of intersections after compression
      if(intersections->size() > 0) 
        return(zone == **intersections->begin());
    
      return false;

    #elif defined(SUBSETS_WITH_COMPRESS) || defined(SUBSETS_WITHOUT_COMPRESS)
      for(auto current_zone = _storage->begin(); current_zone < _storage->end(); ++current_zone) {
        if (zone <= **current_zone)
          return true;
      }
      return false;

    #elif defined(WITHOUT_SUBSETS)
      for(auto current_zone = _storage->begin(); current_zone < _storage->end(); ++current_zone) {
        if (zone == **current_zone) 
          return true;
      }
      return false;

    #else
      #error "Define SUBSETS_WITH_INTERSECTIONS, SUBSETS_WITH_COMPRESS, SUBSETS_WITHOUT_COMPRESS or WITHOUT_SUBSETS."
    #endif
  }

private:

  std::shared_ptr<std::vector<std::shared_ptr<T>>> find_union_partner(std::vector<std::shared_ptr<T>> const cur)
  {
    std::shared_ptr<std::vector<std::shared_ptr<T>>> result = std::make_shared<std::vector<std::shared_ptr<T>>>();

    for(auto to_add = cur.begin(); to_add < cur.end(); ++to_add) {
      bool found = false;
      for(auto in_result = result->begin(); in_result < result->end(); ++in_result) {
        tchecker::dbm::db_t cur_union[this->dim() * this->dim()];
        if(tchecker::dbm::union_convex_t::UNION_IS_CONVEX == tchecker::dbm::convex_union(cur_union, (*to_add)->dbm(), (*in_result)->dbm(), this->dim())) {
          found = true;
          tchecker::dbm::copy((*in_result)->dbm(), cur_union, this->dim());
          break;
        }
      }

      if(!found) {
        result->emplace_back(*to_add);
      }
    }

    return result;
  }

  const tchecker::clock_id_t _dim;
  std::shared_ptr<std::vector<std::shared_ptr<T>>> _storage;

};

template<typename T>
std::shared_ptr<zone_container_t<T>> logical_and_container(zone_container_t<T> & first, zone_container_t<T> & second,
                                                           std::shared_ptr<T> (*factory)(tchecker::clock_id_t))
{
  assert(first.dim() == second.dim());

  first.compress();
  second.compress();
  auto result = std::make_shared<zone_container_t<T>>(first.dim());

  for(auto cur_first : first) {
    assert(tchecker::dbm::is_tight(cur_first->dbm(), cur_first->dim()));
    for(auto cur_second : second) {
      assert(tchecker::dbm::is_tight(cur_second->dbm(), cur_second->dim()));
      auto inter = factory(first.dim());
      cur_first->logic_and(inter, *cur_second);
      result->append_zone(inter);
    }
    result->compress();
  }

  return result;

}

template<typename T>
std::shared_ptr<zone_container_t<T>> logical_and_container(std::vector<std::shared_ptr<zone_container_t<T>>> & lo_container,
                                                           std::shared_ptr<T> (*factory)(tchecker::clock_id_t))
{
  assert(!lo_container.empty());

  auto iter = lo_container.begin();
  auto result = std::make_shared<zone_container_t<T>>(**iter);
  iter++;
  for( ; iter < lo_container.end(); iter++) {
    result = logical_and_container<T>(*result, **iter, factory);
  }
  return result;
}

/*
 \brief a matrix of container for all subtypes of zone
 */
template <typename T>
class zone_matrix_t {

public:

  
  /*!
   \brief Constructor
   \param no_of_rows : number of rows in matrix
   \param no_of_columns : number of columns in matrix
   \param dim : the dimension of the zones
   */
  zone_matrix_t(size_t no_of_rows, size_t no_of_columns, tchecker::clock_id_t dim) :
    _dim(dim), _no_of_rows(no_of_rows), _no_of_columns(no_of_columns), _matrix(std::vector<std::shared_ptr<zone_container_t<T>>>(no_of_rows * no_of_columns)) {

    for(std::size_t i = 0; i < no_of_rows*no_of_columns; ++i) {
      _matrix[i] = std::make_shared<zone_container_t<T>>(dim);
    }
  };

  /*!
   \brief Getter for matrix element
   \param row : row of the element
   \param column : column of the element
   \return pointer to the element
  */
  std::shared_ptr<zone_container_t<T>> get(size_t row, size_t column) {
    assert(row < _no_of_rows);
    assert(column < _no_of_columns);

    return _matrix[row*_no_of_columns + column];
  }

  /*!
   \brief Accessor for the row size
   \return the row size
  */
  size_t get_no_of_rows() const { return _no_of_rows; }

  /*!
   \brief Accessor for the column size
   \return the column size
  */
  size_t get_no_of_columns() const { return _no_of_columns; }

  /*!
   \brief Accessor for the dim
   \return the dimension of the virtual constraints
  */
  tchecker::clock_id_t get_dim() const { return _dim; }

  std::shared_ptr<std::vector<std::shared_ptr<zone_container_t<T>>>>  get_row(size_t row)
  {
    auto result = std::make_shared<std::vector<std::shared_ptr<zone_container_t<T>>>>();
    for(size_t i = 0; i < this->get_no_of_columns(); i++) {
      result->emplace_back(this->get(row, i));
    }
    return result;
  }

  std::shared_ptr<std::vector<std::shared_ptr<zone_container_t<T>>>> get_column(size_t column)
  {
    auto result = std::make_shared<std::vector<std::shared_ptr<zone_container_t<T>>>>();
    for(size_t i = 0; i < this->get_no_of_rows(); i++) {
      result->emplace_back(this->get(i, column));
    }
    return result;
  }

  void print_zone_matrix(std::ostream & os)
  {
    for(size_t i = 0; i < _no_of_rows; ++i) {
      auto row = get_row(i);
      for(size_t j = 0; j < row->size(); ++j) {
        os << "matrix element [" << i << ", " << j << "]:" << std::endl;
        (*row)[i]->print_zone_container(os);
      }
    }
  }

  private:

    const tchecker::clock_id_t _dim;

    const size_t _no_of_rows, _no_of_columns;
    std::vector<std::shared_ptr<zone_container_t<T>>> _matrix;

};
} // end of namespace tchecker

#endif
