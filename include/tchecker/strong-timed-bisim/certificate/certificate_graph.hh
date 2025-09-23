/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_GRAPH_HH
#define TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_GRAPH_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_node.hh"
#include "tchecker/strong-timed-bisim/certificate/certificate_edge.hh"

#include "tchecker/graph/output.hh"

#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace certificate {

/*!
 \class graph_t
 \brief Graph of the certificates of a strong timed bisimulation check
 */
template<typename node, typename edge>
class graph_t {
public:

  static_assert(
    std::is_base_of<tchecker::strong_timed_bisim::certificate::node_t, node>::value, 
      "node must derive from the certificate node");


  static_assert(
    std::is_base_of<tchecker::strong_timed_bisim::certificate::edge_t<node>, edge>::value, 
      "edge must derive from the certificate edge");

  /*!
   \brief Constructor
   \param vcg1 : first vcg
   \param vcg1 : second vcg
   \note this keeps a pointer on the vcg
  */
  graph_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
    : _vcg1(vcg1), _vcg2(vcg2), _nodes(std::make_shared<std::vector<std::shared_ptr<node>>>()),
      _edges(std::make_shared<std::vector<std::shared_ptr<edge>>>()), _nodes_id_counter(0)
  { }

  /*!
   \brief Accessor
   \return pointer to first vcg
  */
  std::shared_ptr<tchecker::vcg::vcg_t> vcg1_ptr() const { return _vcg1; }

  /*!
   \brief Accessor
   \return first vcg
  */
  tchecker::vcg::vcg_t const & vcg1() const { return *_vcg1; }

  /*!
   \brief Accessor
   \return pointer to the second vcg
  */
  std::shared_ptr<tchecker::vcg::vcg_t> vcg2_ptr() const { return _vcg2; }

  /*!
   \brief Accessor
   \return second vcg
  */
  tchecker::vcg::vcg_t const & vcg2() const { return *_vcg2; }

  /*!
   \brief Accessor
   \return the vector of all contained nodes
  */
  std::shared_ptr<std::vector<std::shared_ptr<node>>> nodes() const { return _nodes; }

 /*!
   \brief Accessor
   \return the vector of all contained edges
  */
  std::shared_ptr<std::vector<std::shared_ptr<edge>>> edges() const { return _edges; }

  /*!
   \brief a printer for the attributes without header and footer
  */
  std::ostream & dot_output_helper(std::ostream & os)
  {
    std::map<std::string, std::string> attr;

    for (std::shared_ptr<node> cur : *(nodes())) {
       attr.clear();
       cur->attributes(attr, _vcg1, _vcg2);
       tchecker::graph::dot_output_node(os, std::to_string(cur->id()), attr);
    }

    for (std::shared_ptr<edge> cur : *(edges())) {
      attr.clear();
      cur->attributes(attr, _vcg1, _vcg2);
      tchecker::graph::dot_output_edge(os, std::to_string(cur->src()->id()), std::to_string(cur->tgt()->id()), attr);
    }

    return os;
  }

  /*!
   \brief Graph output
   \param os : output stream
   \param name : graph name
   \post graph with name has been output to os
  */
  virtual std::ostream & dot_output(std::ostream & os, std::string const & name)
  {
    tchecker::graph::dot_output_header(os, name);

    dot_output_helper(os);

    return tchecker::graph::dot_output_footer(os);
  }

protected:

  /*!
   \brief returns the node and nullptr in case there exists none.
   \param to_find : the node to find
  */
  std::shared_ptr<node> find_node(std::shared_ptr<node> to_find) const
  {
    for (auto cur : *_nodes) {
      if (*cur == *to_find) {
        return cur;
      }
    }

    return nullptr;
  }

  /*!
   \brief returns the node and nullptr in case there exists none.
   \param args : the arguments for the constructor of the node to find
  */
  template<typename... Args>
  std::shared_ptr<node> find_node(Args&&... args) const
  {
    auto to_find = std::make_shared<node>(std::forward<Args>(args)...);
    return find_node(to_find);
  }

  /*!
   \brief defines what happens in case a node shall be added that already exists
   */
  virtual void add_node_that_already_exists(std::shared_ptr<node> to_add) = 0;

  /*!
   \brief Add a node
   \param to_add : the node to add
   */
  std::shared_ptr<node> add_node(std::shared_ptr<node> to_add)
  {
    if (nullptr == find_node(to_add)) {
      to_add->set_id(_nodes_id_counter++);
      _nodes->push_back(to_add);
      return to_add;
    }
    else {
      add_node_that_already_exists(to_add);
      return to_add;
    }
  }

  /*!
   \brief Add a node
   \param args : the arguments for the constructor of the node to add
  */
  template <typename... Args>
  std::shared_ptr<node> add_node(Args&&... args)
  {
    auto to_add = std::make_shared<node>(std::forward<Args>(args)...);
    add_node(to_add);
    return to_add;
  }

  /*!
   \brief Add an edge
   \param to_add : the edge to add
   */
  std::shared_ptr<edge> add_edge(std::shared_ptr<edge> to_add)
  {
    _edges->push_back(to_add);
    return to_add;
  }

  /*!
   \brief Add an edge
   \param args : the arguments for the constructor of the edge to add
  */
  template <typename... Args>
  std::shared_ptr<edge> add_edge(Args&&... args)
  {
    auto to_add = std::make_shared<edge>(std::forward<Args>(args)...);
    add_edge(to_add);
    return to_add;
  }

  const std::shared_ptr<tchecker::vcg::vcg_t> _vcg1;
  const std::shared_ptr<tchecker::vcg::vcg_t> _vcg2;

  std::shared_ptr<std::vector<std::shared_ptr<node>>> _nodes;
  std::shared_ptr<std::vector<std::shared_ptr<edge>>> _edges;

 private:
  // any added nodes gets an unique id. This is the corresponding counter.
  std::size_t _nodes_id_counter;
};

} // end of namespace certificate

} // end of namespace strong_timed_bisim

} // end of namespace tchecker


#endif