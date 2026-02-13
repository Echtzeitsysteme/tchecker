/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_EDGE_HH
#define TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_EDGE_HH

#include "tchecker/graph/edge.hh"
#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace certificate {

/*!
 \class edge_t
 \brief Type of edge of the certificates of a strong timed bisimulation check
*/
template<typename node>
class edge_t {
public:
  /*!
   \brief Constructor
   \param t1 : a transition of the first VCG
   \param t2 : a transition of the second VCG
   \param src : the source of this edge
   \param tgt : the target of this edge
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node> source,
              std::shared_ptr<node> target) :
    _edge_pair(std::make_shared<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>>(t1.vedge_ptr(), t2.vedge_ptr())),
    _src(source), _tgt(target)
  { }

  /*!
   \brief Constructor
   \param edge_pair : the pair of edges
   \param source : the source of this edge
   \param target : the target of this edge
   */
  edge_t(std::shared_ptr<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>> edge_pair, 
         std::shared_ptr<node> source, std::shared_ptr<node> target) :
    _edge_pair(edge_pair), _src(source), _tgt(target)
  {

  }

  /*!
   \brief Copy Constructor
   \param to_copy : the edge to copy
  */
  edge_t(edge_t<node> const & to_copy) :
    _edge_pair(to_copy._edge_pair),
    _src(to_copy.src()), _tgt(to_copy.tgt())
  { }

 /*!
  \brief Accessor
  \return shared pointer to the pair of edges
  */
  inline std::shared_ptr<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>> edge_pair_ptr() const { return _edge_pair; }

 /*!
  \brief Accessor
  \return the pair of edges
  */
  inline std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t> const & edge_pair() const { return *_edge_pair; }

 /*!
  \brief Accessor
  \return shared pointer to the source of this edge
  */
  inline std::shared_ptr<node> src() const { return _src; }

 /*!
  \brief Accessor
  \return shared pointer to the target of this edge
  */
  inline std::shared_ptr<node> tgt() const { return _tgt; }

 /*!
   \brief Less-than ordering on edges based on lexical ordering
   \param other : an edge
   \return true if lhs is less-than rhs w.r.t. the tuple of edges in e1 and e2
   \note _condition is not part of this comparison!
  */
  bool operator<(const edge_t & other) const
  {
    auto cmp = tchecker::lexical_cmp(this->edge_pair_ptr()->first.vedge(), other.edge_pair_ptr()->first.vedge());
    if (0 != cmp) {
      return cmp < 0;
    }

    cmp = tchecker::lexical_cmp(this->edge_pair_ptr()->second.vedge(), other.edge_pair_ptr()->second.vedge());
    if (0 != cmp) {
      return cmp < 0;
    }

    cmp = (this->_src < other._src);
    if (0 != cmp) {
      return cmp < 0;
    }

    cmp = (this->_tgt < other._tgt);

    return cmp < 0;
  }

  /*!
   \brief equal operator
   \param other : an edge type
   \return true if lhs is less-than rhs w.r.t. the tuple of edges in e1 and e2
   \note _condition is not part of this comparison!
  */
  bool operator==(const edge_t & other) const { return !(*this < other) && !(other < *this); }

  void attributes(std::map<std::string, std::string> & m, const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
  {
    m["first_vedge"] = tchecker::to_string(this->edge_pair_ptr()->first.vedge(), vcg1->system().as_system_system());
    m["second_vedge"] = tchecker::to_string(this->edge_pair_ptr()->second.vedge(), vcg2->system().as_system_system());

    auto generate_edge_attr = [&m](std::shared_ptr<tchecker::vcg::vcg_t> vcg, tchecker::graph::edge_vedge_t & edge,
                                   std::string prefix) {
      m[prefix + "_vedge"] = tchecker::to_string(edge.vedge(), vcg->system().as_system_system());
      bool first = true;

      std::string do_attr = prefix + "_vedge_do";
      std::string prov_attr = prefix + "_vedge_prov";

      for (auto cur_edge : edge.vedge()) {
        if (!first) {
          m[do_attr] += ";";
          m[prov_attr] += ";";
        }

        bool second_first = true;
        for (auto cur_do : vcg->system().as_system_system().edge(cur_edge)->attributes().range("do")) {
          if (!second_first) {
            m[do_attr] += ";";
          }
          m[do_attr] += cur_do.value();

          second_first = false;
        }

        second_first = true;
        for (auto cur_prov : vcg->system().as_system_system().edge(cur_edge)->attributes().range("provided")) {
          if (!second_first) {
            m[prov_attr] += ";";
          }
          m[prov_attr] += cur_prov.value();

          second_first = false;
        }

        first = false;
      }
    };

    generate_edge_attr(vcg1, this->edge_pair_ptr()->first, "first");
    generate_edge_attr(vcg2, this->edge_pair_ptr()->second, "second");
  }

  bool attributes_equivalent(const edge_t & other, const std::shared_ptr<tchecker::vcg::vcg_t> vcg1, const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
  {
    std::map<std::string, std::string> attr_this;
    attr_this.clear();
    this->attributes(attr_this, vcg1, vcg2);

    std::map<std::string, std::string> attr_other;
    attr_other.clear();
    other.attributes(attr_other, vcg1, vcg2);

    return attr_this == attr_other;
  }

protected:
  std::shared_ptr<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>> _edge_pair;
  std::shared_ptr<node> _src;
  std::shared_ptr<node> _tgt;
};


} // end of namespace certificate

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif