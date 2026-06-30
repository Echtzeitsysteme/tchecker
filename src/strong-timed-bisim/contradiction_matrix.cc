/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

 #include "tchecker/strong-timed-bisim/contradiction_matrix.hh"


namespace tchecker{

namespace strong_timed_bisim {

contradiction_matrix_t::contradiction_matrix_t(std::size_t no_of_rows, std::size_t no_of_columns, tchecker::clock_id_t dim) :
  _dim(dim), _no_of_rows(no_of_rows), _no_of_columns(no_of_columns), _matrix(std::vector<std::shared_ptr<contradiction_t>>(no_of_rows * no_of_columns)) {

  for(std::size_t i = 0; i < no_of_rows*no_of_columns; ++i) {
    _matrix[i] = std::make_shared<contradiction_t>(dim-1, 0);
  }
}

std::shared_ptr<contradiction_t> contradiction_matrix_t::get(size_t row, size_t column) {
  assert(row < _no_of_rows);
  assert(column < _no_of_columns);

  return _matrix[row*_no_of_columns + column];
}


std::shared_ptr<std::vector<std::shared_ptr<contradiction_t>>> contradiction_matrix_t::get_row(size_t row)
{
  auto result = std::make_shared<std::vector<std::shared_ptr<contradiction_t>>>();
  for(size_t i = 0; i < this->get_no_of_columns(); i++) {
    result->emplace_back(this->get(row, i));
  }
  return result;
}

std::shared_ptr<std::vector<std::shared_ptr<contradiction_t>>> contradiction_matrix_t::get_column(size_t column)
{
  auto result = std::make_shared<std::vector<std::shared_ptr<contradiction_t>>>();
  for(size_t i = 0; i < this->get_no_of_rows(); i++) {
    result->emplace_back(this->get(i, column));
  }
  return result;
}

void contradiction_matrix_t::add_contradiction(size_t row, size_t column, contradiction_t & con)
{
  std::shared_ptr<contradiction_t> add_here = get(row, column);
  tchecker::integer_t steps = add_here->min_steps_to_cont();
  steps = std::max(steps, con.min_steps_to_cont());
  add_here->add_contradiction(con);
  add_here->set_min_steps_to_cont(steps);
}


} // end of namespace strong_timed_bisim

} // end of namespace tchecker