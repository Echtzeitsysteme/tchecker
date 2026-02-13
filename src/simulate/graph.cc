/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/graph.hh"

#include "tchecker/variables/clocks.hh"

namespace tchecker {

namespace simulate {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

/* concrete_node_t */

// concrete_node_t::concrete_node_t(tchecker::zg::state_sptr_t const & s, std::shared_ptr<tchecker::clockval_t> valuation, bool initial, bool final)
//     : node_t(s, initial, final), _valuation(valuation)
// {
// }

// concrete_node_t::concrete_node_t(tchecker::zg::const_state_sptr_t const & s, std::shared_ptr<tchecker::clockval_t> valuation, bool initial, bool final)
//     : node_t(s, initial, final), _valuation(valuation)
// {
// }

// std::shared_ptr<tchecker::clockval_t> concrete_node_t::get_valuation() const
// {
//   return _valuation;
// }

/* edge_t */

edge_t::edge_t(tchecker::zg::transition_t const & t) : tchecker::graph::edge_vedge_t(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size)
    : tchecker::graph::reachability::multigraph_t<tchecker::simulate::node_t, tchecker::simulate::edge_t>::multigraph_t(
          block_size),
      _zg(zg)
{
}

graph_t::~graph_t()
{
  tchecker::graph::reachability::multigraph_t<tchecker::simulate::node_t, tchecker::simulate::edge_t>::clear();
}

void graph_t::attributes(tchecker::simulate::node_t const & n, std::map<std::string, std::string> & m) const
{
  _zg->attributes(n.state_ptr(), m);
  tchecker::graph::attributes(static_cast<tchecker::graph::node_flags_t const &>(n), m);
}

// void graph_t::attributes(tchecker::simulate::concrete_node_t const & n, std::map<std::string, std::string> & m) const
// {
//   attributes(static_cast<node_t>(n), m);

//   std::function<std::string (tchecker::clock_id_t)> clock_names =
//     [this](tchecker::clock_id_t id) {
//       if(id == 0) {
//         return std::string("Ref Clock");
//       }

//       auto counter = id-1;
//       auto const & clocks = this->_zg->system().clock_variables();
//       for(tchecker::clock_id_t base : clocks.identifiers(tchecker::VK_DECLARED)) {
//         tchecker::clock_id_t size_of_cur_decl_clk = clocks.info(base).size();
//         if(counter < size_of_cur_decl_clk) {
//           if(1 == size_of_cur_decl_clk) {
//             return clocks.name(base);
//           }
//           std::stringstream clk_name;
//           clk_name << clocks.name(base) << "[" << counter << "]";
//           return clk_name.str();
//         }
//         counter -= size_of_cur_decl_clk;
//       }

//       throw std::runtime_error(std::string(__FILE__) + std::string(": ") + std::to_string(__LINE__) + std::string(": ") +
//                              std::string("strange clock id: ") + std::to_string(id));
//     };



//   m["clockval"] = to_string(*(n.get_valuation()), clock_names);
// }

void graph_t::attributes(tchecker::simulate::edge_t const & e, std::map<std::string, std::string> & m) const
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
  bool operator()(tchecker::simulate::graph_t::node_sptr_t const & n1,
                  tchecker::simulate::graph_t::node_sptr_t const & n2) const
  {
    int state_cmp = tchecker::zg::lexical_cmp(n1->state(), n2->state());
    if (state_cmp != 0)
      return (state_cmp < 0);
    return (tchecker::graph::lexical_cmp(static_cast<tchecker::graph::node_flags_t const &>(*n1),
                                         static_cast<tchecker::graph::node_flags_t const &>(*n2)) < 0);
  }
};

// /*!
//  \class node_lexical_less_t
//  \brief Less-than order on nodes based on lexical ordering
// */
// class concrete_node_lexical_less_t {
// public:
//   /*!
//    \brief Less-than order on nodes based on lexical ordering
//    \param n1 : a node
//    \param n2 : a node
//    \return true if n1 is less-than n2 w.r.t. lexical ordering over the states in
//    the nodes
//   */
//   bool operator()(concrete_node_sptr_t const & n1,
//                   concrete_node_sptr_t const & n2) const
//   {
//     node_lexical_less_t node_cmp;
//     int node_res = node_cmp(n1, n2);
//     if(node_res != 0)
//       return node_res;

//     return tchecker::lexical_cmp(*(n1->valuation()), *(n2->valuation()));
//   }
// };

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
  bool operator()(tchecker::simulate::graph_t::edge_sptr_t const & e1,
                  tchecker::simulate::graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::simulate::graph_t const & g, std::string const & name)
{
  return tchecker::graph::reachability::dot_output<tchecker::simulate::graph_t, tchecker::simulate::node_lexical_less_t,
                                                   tchecker::simulate::edge_lexical_less_t>(os, g, name);
}

/* state_space_t */

state_space_t::state_space_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size) : _ss(zg, zg, block_size)
{
}

tchecker::zg::zg_t & state_space_t::zg() { return _ss.ts(); }

tchecker::simulate::graph_t & state_space_t::graph() { return _ss.state_space(); }

} // namespace simulate

} // namespace tchecker
