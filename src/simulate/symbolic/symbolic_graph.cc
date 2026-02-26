/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/symbolic/symbolic_graph.hh"

namespace tchecker {

namespace simulate {

namespace symbolic {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

/* edge_t */

edge_t::edge_t(tchecker::zg::transition_t const & t) : tchecker::graph::edge_vedge_t(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size)
    : tchecker::graph::reachability::multigraph_t<node_t, edge_t>::multigraph_t(
          block_size),
      _zg(zg)
{
}

graph_t::~graph_t()
{
  tchecker::graph::reachability::multigraph_t<node_t, edge_t>::clear();
}

void graph_t::attributes(node_t const & n, std::map<std::string, std::string> & m) const
{
  _zg->attributes(n.state_ptr(), m);
  tchecker::graph::attributes(static_cast<tchecker::graph::node_flags_t const &>(n), m);
}

void graph_t::attributes(edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["vedge"] = tchecker::to_string(e.vedge(), _zg->system().as_system_system());
}

/* DOT output */

/*!
 \class node_lexical_less_t
 \brief Less-than order on nodes based on lexical ordering
*/
class node_lexical_less_t {
public:
  /*!
   \brief Less-than order on nodes based on lexical ordering
   \param n1 : a node
   \param n2 : a node
   \return true if n1 is less-than n2 w.r.t. lexical ordering over the states in
   the nodes
  */
  bool operator()(graph_t::node_sptr_t const & n1, graph_t::node_sptr_t const & n2) const
  {
    int state_cmp = tchecker::zg::lexical_cmp(n1->state(), n2->state());
    if (state_cmp != 0)
      return (state_cmp < 0);
    return (tchecker::graph::lexical_cmp(static_cast<tchecker::graph::node_flags_t const &>(*n1),
                                         static_cast<tchecker::graph::node_flags_t const &>(*n2)) < 0);
  }
};

/*!
 \class edge_lexical_less_t
 \brief Less-than ordering on edges based on lexical ordering
 */
class edge_lexical_less_t {
public:
  /*!
   \brief Less-than ordering on edges based on lexical ordering
   \param e1 : an edge
   \param e2 : an edge
   \return true if e1 is less-than  e2 w.r.t. the tuple of edges in e1 and e2
  */
  bool operator()(graph_t::edge_sptr_t const & e1, graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

/* state_space_t */

state_space_t::state_space_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size) : _ss(zg, zg, block_size)
{
}

tchecker::zg::zg_t & state_space_t::zg() { return _ss.ts(); }

graph_t & state_space_t::graph() { return _ss.state_space(); }

void state_space_t::dot_output(std::ostream & os, std::string const & name)
{
  tchecker::graph::reachability::dot_output<graph_t, node_lexical_less_t,
                                                   edge_lexical_less_t>(os, this->graph(), name);
}


} // end of namespace symbolic

} // end of namespace simulate

} // end of namespace tchecker
