/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_STATE_TEMPLATE_HH
#define TCHECKER_ZG_STATE_TEMPLATE_HH

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/ta/state.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/clockbounds/clockbounds.hh"

/*!
 \file state_template.hh
 \brief State of a zone graph
 */

namespace tchecker {

namespace zg {

/*!
\brief Type of shared zone, DBM implementation
 */
template<typename Tused_zone_t>
using shared_zone_helper_t = tchecker::make_shared_t<Tused_zone_t>;

/*!
 \brief Type of shared pointer to zone, DBM implementation
 */
template<typename Tused_zone_t>
using zone_sptr_helper_t = tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t>>;

/*!
 \class state_help_t
 \brief state of a zone graph
 \note to instantiate a symbolic state of a usual zg, set
 * zone_t = tchecker::zg::zone_t
 * to instatiate a symbolic state of a vcg, set
 * zone_t = tchecker::vcg::zone_t
 */
template<typename Tused_zone_t>
class state_help_t : public tchecker::ta::state_t {
public:

  /*!
   \brief Constructor
   \param vloc : tuple of locations
   \param intval : integer variables valuation
   \param zone : zone of clock valuations
   \pre vloc, intval and zone must not point to nullptr (checked by assertion)
   */
  explicit state_help_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                 tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t>> const & zone)
    : tchecker::ta::state_t(vloc, intval), _zone(zone)
  {
    assert(_zone.ptr() != nullptr);
  }

  /*!
   \brief Partial copy constructor
   \param state : a state
   \param vloc : tuple of locations
   \param intval : integer variables valuation
   \param zone : zone of clock valuations
   \pre vloc, intval and zone must not point to nullptr (checked by assertion)
   \note the state is copied from s, except the tuple of locations which is
   initialized from vloc, and the valuation of bounded integer variables which
   is initialized from intval, and the zone which is initialized from zone
   */
  state_help_t(tchecker::ta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                 tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t>> const & zone)
    : tchecker::ta::state_t(s, vloc, intval), _zone(zone)
  {
    assert(_zone.ptr() != nullptr);
  }

  /*!
   \brief Copy constructor (deleted)
   */
  state_help_t(state_help_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  state_help_t(state_help_t &&) = delete;

  /*!
   \brief Destructor
   */
  ~state_help_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::zg::state_help_t<Tused_zone_t> & operator=(tchecker::zg::state_help_t<Tused_zone_t> const &) = default;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::zg::state_help_t<Tused_zone_t> & operator=(tchecker::zg::state_help_t<Tused_zone_t> &&) = default;

  /*!
  \brief Accessor
  \return zone in this state
  */
  constexpr inline Tused_zone_t const & zone() const { return *_zone; }

  /*!
  \brief Accessor
  \return reference to pointer to the zone in this state
  */
  inline tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t>> & zone_ptr() { return _zone; }

  /*!
  \brief Accessor
  \return pointer to const zone in this state
  */
  inline tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t> const> zone_ptr() const
  {
    return tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t> const>(_zone.ptr());
  }

private:
  tchecker::intrusive_shared_ptr_t<shared_zone_helper_t<Tused_zone_t> > _zone; /*!< Zone over clock valuations */
};

/*!
 \brief Equality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same zone, same tuple of locations and same
 valuation of bounded integer variables, false otherwise
 */
template<typename Tused_zone_t>
bool operator==(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2)
{
  return tchecker::ta::operator==(s1, s2) && (s1.zone() == s2.zone());
}

/*!
 \brief Disequality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have different zones, or different tuples of locations or different valuations of bounded integer
 variables, false otherwise
 */
template<typename Tused_zone_t>
bool operator!=(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2)
{
  return !(s1 == s2);
}


/*!
 \brief Equality check for shared states
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same zone, same tuple of locations and same
 valuation of bounded integer variables, false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointer equality (not values equality)
 */
template<typename Tused_zone_t>
bool shared_equal_to(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2)
{
  return tchecker::ta::shared_equal_to(s1, s2) && (s1.zone_ptr() == s2.zone_ptr());
}

/*!
 \brief Covering check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is included in the zone in s2, false
 otherwise
*/
template<typename Tused_zone_t>
bool operator<=(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2)
{
  return tchecker::ta::operator==(s1, s2) && (s1.zone() <= s2.zone());
}

/*!
 \brief Covering check for shared states
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is included in the zone in s2, false
 otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointers instead of values when relevant
*/
template<typename Tused_zone_t>
bool shared_is_le(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2)
{
  return tchecker::ta::shared_equal_to(s1, s2) && (s1.zone_ptr() == s2.zone_ptr() || s1.zone() <= s2.zone());
}

/*!
 \brief aLU subsumption check
 \param s1 : state
 \param s2 : state
 \param l : clock lower bounds
 \param u : clock upper bounds
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is included in aLU-abstraction of the
 zone in s2, false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointers instead of values when relevant
*/
template<typename Tused_zone_t>
bool is_alu_le(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2, tchecker::clockbounds::map_t const & l,
               tchecker::clockbounds::map_t const & u)
{
  return tchecker::ta::operator==(s1, s2) && s1.zone().is_alu_le(s2.zone(), l, u);
}

/*!
 \brief aLU subsumption check for shared states
 \param s1 : state
 \param s2 : state
 \param l : clock lower bounds
 \param u : clock upper bounds
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is included in aLU-abstraction of the
 zone in s2, false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointers instead of values when relevant
*/
template<typename Tused_zone_t>
bool shared_is_alu_le(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2,
                      tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u)
{
  return tchecker::ta::shared_equal_to(s1, s2) && (s1.zone_ptr() == s2.zone_ptr() || s1.zone().is_alu_le(s2.zone(), l, u));
}

/*!
 \brief Hash
 \param s : state
 \return Hash value for state s
 */
template<typename Tused_zone_t>
std::size_t hash_value(tchecker::zg::state_help_t<Tused_zone_t> const & s)
{
  std::size_t h = tchecker::ta::hash_value(s);
  boost::hash_combine(h, s.zone());
  return h;
}

/*!
 \brief Hash for shared states
 \param s : state
 \return Hash value for state s
 \note this should only be used on states that have shared internal components: this function
 hashes the pointers (not the values)
 */
template<typename Tused_zone_t>
std::size_t shared_hash_value(tchecker::zg::state_help_t<Tused_zone_t> const & s)
{
  std::size_t h = tchecker::ta::shared_hash_value(s);
  boost::hash_combine(h, s.zone_ptr());
  return h;
}

/*!
 \brief Lexical ordering on states of the zone graph
 \param s1 : a state
 \param s2 : a state
 \return 0 if s1 and s2 are equal, a negative value if s1 is smaller than s2
 w.r.t. lexical ordering on zone,tuple of locations and valuation of bounded
 integer variables, a positive value otherwise
 */
template<typename Tused_zone_t>
int lexical_cmp(tchecker::zg::state_help_t<Tused_zone_t> const & s1, tchecker::zg::state_help_t<Tused_zone_t> const & s2)
{
  int ta_cmp = tchecker::ta::lexical_cmp(s1, s2);
  if (ta_cmp != 0)
    return ta_cmp;
  return s1.zone().lexical_cmp(s2.zone());
}


/*!
 \brief Type of shared state
 */
template<typename Tused_zone_t>
using shared_state_help_t = tchecker::make_shared_t<tchecker::zg::state_help_t<Tused_zone_t>>;

/*!
\brief Type of pointer to shared state
*/
template<typename Tused_zone_t>
using state_sptr_help_t = tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_state_help_t<Tused_zone_t>>;

/*!
\brief Type of pointer to shared const state
*/
template<typename Tused_zone_t>
using const_state_sptr_help_t = tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_state_help_t<Tused_zone_t> const>;

} // end of namespace zg

} // end of namepsace tchecker
#endif
