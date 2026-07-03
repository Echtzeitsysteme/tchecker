/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
*/

#ifndef TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_MATRIX
#define TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_MATRIX

#include <vector>
#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/strong-timed-bisim/contradiction.hh"

namespace tchecker{

namespace strong_timed_bisim {

  /*
 \brief a matrix of container for all subtypes of zone
 */
class contradiction_matrix_t {
 public:

  /*!
   \brief Constructor
   \param no_of_rows : number of rows in matrix
   \param no_of_columns : number of columns in matrix
   \param dim : the dimension of the zones
   */
  contradiction_matrix_t(size_t no_of_rows, size_t no_of_columns, tchecker::clock_id_t dim);

  /*!
   \brief Getter for matrix element
   \param row : row of the element
   \param column : column of the element
   \return pointer to the element
  */
  std::shared_ptr<contradiction_t> get(size_t row, size_t column);

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

  /*!
   \brief Accessor for a specific row
   \param row : the row to return
   \return the requested row
   */
  std::shared_ptr<std::vector<std::shared_ptr<contradiction_t>>>  get_row(size_t row);

  /*!
   \brief Accessor for a specific column
   \param row : the column to return
   \return the requested column
   */
  std::shared_ptr<std::vector<std::shared_ptr<contradiction_t>>> get_column(size_t column);

  /*!
   \brief Adds a contradiction
   \param row : row of the element where the contradiction should be added
   \param column : column of the element where the contradiction should be added
   \param con : the contradiction to add
   \post con is added to the matrix at position (row, column)
   */
  void add_contradiction(size_t row, size_t column, contradiction_t & con);

  private:

    const tchecker::clock_id_t _dim;

    const size_t _no_of_rows, _no_of_columns;
    std::vector<std::shared_ptr<contradiction_t>> _matrix;

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
