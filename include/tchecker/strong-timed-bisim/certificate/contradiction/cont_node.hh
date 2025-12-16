/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_CONT_NODE_HH
#define TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_CONT_NODE_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_node.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

 /*!
 \class node_t
 \brief Node of the contradiction dag of a strong timed bisimulation check
 */
class node_t : public tchecker::strong_timed_bisim::certificate::node_t {

public:
  /*!
    \brief Constructor
    \param location_pair : the pair of locations to add
    \param valuation_1 : the clock valuation of vcg1
    \param valuation_2 : the clock valuation of vcg2
    \param id : the id of this node within the witness graph
    \param urgent : whether an urgent clk exists
    \param initial : whether this node is the root
   */
  node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
         std::shared_ptr<tchecker::clockval_t> valuation_1, std::shared_ptr<tchecker::clockval_t> valuation_2,
         std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2,
         std::size_t id = 0, bool urgent_clk_exists = false, bool initial = false);

  /*!
    \brief Constructor
    \param s_1 : a synchronized, symbolic state of vcg1 that contains only a single state
    \param s_2 : a synchronized, symbolic state of vcg2 that is virtual equivalent to first
    \param id : the id of this node within the witness graph
    \param no_of_orig_clks_1 : the number of original clocks of the first vcg
    \param no_of_orig_clks_2 : the number of original clocks of the second vcg
    \param urgent : whether an urgent clk exists
    \param initial : whether this node is the root
  */
  node_t(tchecker::zg::state_sptr_t s_1, tchecker::zg::state_sptr_t s_2,
         tchecker::clock_id_t no_of_orig_clks_1, tchecker::clock_id_t no_of_orig_clks_2,
         std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2,
         std::size_t id = 0, bool urgent_clk_exists = false, bool initial = false);
  
  /*!
   \brief Copy Constructor
   */
  node_t(const node_t & other);

  /*!
   \brief Accessor
   \return _valuation
   */
  inline std::pair<std::shared_ptr<tchecker::clockval_t>, std::shared_ptr<tchecker::clockval_t>> valuation() const { return _valuation; }

    /*!
   \brief Accessor
   \return _invariant
   */
  inline std::pair<std::shared_ptr<tchecker::clock_constraint_container_t>, std::shared_ptr<tchecker::clock_constraint_container_t>> invariant() const { return _invariant; }

  /*!
   \brief Accessor
   \return _invariant
   */
  inline bool final() const { return _final; }

  /*!
   \brief Equality Operator
   \return whether this is semantically equivalent to other
   */
  bool operator==(node_t & other);


  /*!
  \brief Accessor to node attributes
  \param m : the map to add the attributes
  \param vcg1: the first vcg
  \param vcg2 : the second vcg
  \post The attributes of this node are added to m
  */
  void attributes(std::map<std::string, std::string> & m, 
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const override;

  /*!
   \brief checks whether both valuations are synchronized
   \return true, if this is the case.
   */
  bool is_synchronized() const;

  /*!
   \brief synchronizes this node
   \post is_synchronized() returns true
   \note only resets to zero are possible!
  */
  void synchronize();

  /*!
   \brief checks whether this node is element of the given symbolic state
   \param symb_state_1 : the symbolic state belonging to the first VCG
   \param symb_state_2 : the symbolic state belonging to the second VCG
   \return true, if the location pair matches the locations of the symb states and valuation can be element of them.
   */
  bool is_element_of(tchecker::zg::state_sptr_t symb_state_1, tchecker::zg::state_sptr_t symb_state_2) const;

  /*!
   \brief checks whether this node can be the leaf of a Contradiction DAG
   \param vcg1 : the first vcg
   \param vcg2 : the second vcg
  */
  bool is_leaf(tchecker::zg::state_sptr_t & init_1, tchecker::zg::state_sptr_t & init_2, 
               std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2,
               clock_rational_value_t max_possible_delay);

  /*!
   \brief generate two zones that contain the valuations of this node
   \param vcg1 : the first vcg
   \param vcg2 : the second vcg
   \return a pair. The first zone is the narrowst zone of a symbolic state of the first vcg that only contains _valuation_1 of this node and analogously for the second
   */
  std::shared_ptr<std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>>
  generate_zones(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const;

  /*!
   \brief calculates the maximum delay while staying within the given virtual constraints
   \param vcs : the virtual constraints to stay within
   \param max_delay : the maximum delay to use
   \param vcg1 : the first vcg
   \param vcg2 : the second vcg
   \return a pair: first element is the delay, second element is the resulting node
   \note In case no maximum delay exists as there is a non-included upper bound, .5 is used. In case there exists no upper bound, max_delay is used as delay.
   */
  std::pair<clock_rational_value_t, std::shared_ptr<node_t>>
  max_delay(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vcs, clock_rational_value_t max_delay_value, 
            std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2);

  /*!
   \brief calculates the maximum delay while staying within the given virtual constraint
   \param vc : the virtual constraint to stay within
   \param max_delay : the maximum delay to use
   \param vcg1 : the first vcg
   \param vcg2 : the second vcg
   \return a pair: first element is the delay, second element is the resulting node
   \note In case no maximum delay exists as there is a non-included upper bound, .5 is used. In case there exists no upper bound, max_delay is used as delay.
   */
  std::pair<clock_rational_value_t, std::shared_ptr<node_t>>
  max_delay(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc, clock_rational_value_t max_delay_value, 
            std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2);

  /*!
   \brief Setter for valuation
   \param new_val : the new valuation
   \post the values of _valuation are equal to the values of new_val
   */
  void set_valuation(std::pair<std::shared_ptr<tchecker::clockval_t>, std::shared_ptr<tchecker::clockval_t>> new_val);

  /*!
   \brief sets a value of a clock to zero
   \param first_not_second : whether the clock belongs to the first or the second vcg
   \param i : the id of the clock
   \post the ith clock of the corresponding valuation is set to zero
   */
  void reset_value(bool first_not_second, tchecker::clock_id_t i);

private:

  /*!
   \brief Helper for max_delay
   */
  clock_rational_value_t
  max_delay(tchecker::zg::zone_t & zone, clock_rational_value_t max_delay_value, clock_rational_value_t min_delay, bool first_not_second = true);

  std::pair<std::shared_ptr<tchecker::clockval_t>, std::shared_ptr<tchecker::clockval_t>> _valuation;
  // I do not understand how I can access the invariant of a state of a TA. Therefore: Ugly hack.
  const std::pair<std::shared_ptr<tchecker::clock_constraint_container_t>, std::shared_ptr<tchecker::clock_constraint_container_t>> _invariant;

  const bool _urgent_clk_exists;

  bool _final;
  bool _final_is_delay;
  std::string _final_delay;
  std::shared_ptr<tchecker::graph::edge_vedge_t> _final_trans;
  bool _final_first_has_transition;

};

} // end of namespace contra

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif