/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OPERATIONAL_SEMANTICS_MAX_DELAY_HH
#define TCHECKER_OPERATIONAL_SEMANTICS_MAX_DELAY_HH

#include "tchecker/variables/clocks.hh"
#include "tchecker/zg/zone.hh"

namespace tchecker {

namespace operational_semantics {

enum cmp_t {
  LE = 0,
  L
};

class max_delay_t {
 public:
  max_delay_t(tchecker::clock_rational_value_t value, cmp_t cmp);

  tchecker::clock_rational_value_t value() {return _value;}
  cmp_t cmp() {return _cmp;}

  bool operator<(const max_delay_t& other) const {
    return (this->_value < other._value) ||
           (this->_value == other._value && cmp_t::L == this->_cmp && cmp_t::LE == other._cmp);
  }

  bool operator==(const max_delay_t& other) const {
    return (this->_value == other._value && this->_cmp == other._cmp);
  }

  bool operator!=(const max_delay_t& other) const {
    return !(*this == other);
  }

 private:
  tchecker::clock_rational_value_t _value;
  cmp_t _cmp;
};

/*!
 \brief find the maximum delay that stays in the zone
 \param zone : the zone that should not be left
 \param valuation : the valuation the delay should be added to
 \param max_delay_value : the maximum delay that should not be exceeded.
 \param min_delay_value : the minimum delay that should not be exceeded.
 \return the maximum delay that is lower or equals max_delay value and valuation + delay \in zone
 \note If valuation + max_delay_value \not\in zone and valuation + min_delay_value \not\in zone, 0 is returned.
 */
max_delay_t
max_delay(tchecker::zg::zone_t & zone, std::shared_ptr<tchecker::clockval_t> valuation, std::size_t max_delay_value, std::size_t min_delay_value);

} // end of namespace operational_semantics

} // end of namespace tchecker

#endif