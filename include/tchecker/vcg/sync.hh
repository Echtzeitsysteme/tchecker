/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VCG_SYNC_HH
#define TCHECKER_VCG_SYNC_HH

#include "tchecker/dbm/dbm.hh"
#include "virtual_constraint.hh"

namespace tchecker {

namespace vcg {

class sync_vc_t {

public:

  /*!
   \brief Constructor
   \param A : the first vcg
   \param B : the second vcg
  */
  sync_vc_t(std::shared_ptr<tchecker::vcg::vcg_t> A, std::shared_ptr<tchecker::vcg::vcg_t> B);

  /*!
  \brief A wrapper for the sync function that considers the urgent clock if needed
  \param A_state : a state
  \param B_state : a state
  \param orig_reset1 : the resets of the transition of the first TA
  \param orig_reset2 : the resets of the transition of the second TA
  \post zone_1 and zone_2 are synced, consistent and tight
  \note the change happens inplace
  \note this function works if and only if only resets to zero are allowed!
  */
  void sync_with_urgent(tchecker::zg::state_sptr_t & A_state, tchecker::zg::state_sptr_t & B_state, 
                        tchecker::clock_reset_container_t const & orig_reset1, 
                        tchecker::clock_reset_container_t const & orig_reset2);

  /*!
   \brief A wrapper for the revert-sync function that considers the urgent clock if needed
   \param A_state : a state
   \param B_state : a state
   \param contradiction : the vc for which the sync shall be reverted
   \return a pair of virtual constraints
   \note this function works if and only if only resets to zero are allowed!
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  revert_sync_with_urgent(tchecker::zg::const_state_sptr_t & A_state, tchecker::zg::const_state_sptr_t & B_state,
                          const std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> & contradiction);

//private:
  /*!
     \brief sync function (see the TR of Lieb et al.)
     \param dbm_1 : a dbm
     \param db,_2 : a dbm
     \param  no_of_orig_clocks_1 : the number of orig clocks in the first TA
     \param  no_of_orig_clocks_2 : the number of orig clocks in the second TA
     \param orig_reset1 : the resets of the transition of the first TA
     \param orig_reset2 : the resets of the transition of the second TA
     \post zone_1 and zone_2 are synced, consistent and tight
     \note the change happens inplace
     \note this function works if and only if only resets to zero are allowed!
     */
  void sync(tchecker::dbm::db_t * dbm1, tchecker::dbm::db_t * dbm2, tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
            tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
            tchecker::clock_reset_container_t const & orig_reset1, tchecker::clock_reset_container_t const & orig_reset2);

  /*!
   \brief sync function (see the TR of Lieb et al.)
   \param zone_1 : a zone
   \param zone_2 : a zone
   \param  no_of_orig_clocks_1 : the number of orig clocks in the first TA
   \param  no_of_orig_clocks_2 : the number of orig clocks in the second TA
   \param orig_reset1 : the resets of the transition of the first TA
   \param orig_reset2 : the resets of the transition of the second TA
   \post zone_1 and zone_2 are synced, consistent and tight
   \note the change happens inplace
   \note this function works if and only if only resets to zero are allowed!
   */
  void sync(tchecker::zg::zone_t & zone_1, tchecker::zg::zone_t & zone_2, tchecker::clock_id_t no_of_orig_clocks_1,
            tchecker::clock_id_t no_of_orig_clocks_2, tchecker::clock_reset_container_t const & orig_reset1,
            tchecker::clock_reset_container_t const & orig_reset2);

  /*!
    \brief revert-sync function (see the TR of Lieb et al.)
    \param zone_1 : a zone
    \param zone_2 : a zone
    \param  no_of_orig_clocks_1 : the number of orig clocks in the first TA
    \param  no_of_orig_clocks_1 : the number of orig clocks in the first TA
    \param phi_e : the vc to revert
    \return a pair of virtual constraints
    \note this function works if and only if only resets to zero are allowed!
    */
  std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>,
            std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>>
  revert_sync(tchecker::zg::zone_t const & zone_1, tchecker::zg::zone_t const & zone_2,
              tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
              const tchecker::virtual_constraint::virtual_constraint_t & phi_e);

  const std::shared_ptr<tchecker::vcg::vcg_t> _A;
  const std::shared_ptr<tchecker::vcg::vcg_t> _B;

  tchecker::zg::state_sptr_t _A_copy_for_revert_sync;
  tchecker::zg::state_sptr_t _B_copy_for_revert_sync;
};

/*!
 \brief checks whether the given dbm are synced
 \param dbm1 : the DBM of A
 \param dbm2 : the DBM of B
 \param dim1 : the dim of dbm1
 \param dim2 : the dim of dbm2
 \param no_of_original_clocks_1 : the number of non virt (and non ref) clocks of A
 \param no_of_original_clocks_2 : the number of non virt (and non ref) clocks of B
 */

bool are_dbm_synced(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim1,
                    tchecker::clock_id_t dim2, tchecker::clock_id_t no_of_orig_clocks_1,
                    tchecker::clock_id_t no_of_orig_clocks_2);

/*!
 \brief checks whether the given zones are synced
 \param zone_1 : the first zone
 \param zone_2 : the second zone
 \param no_of_original_clocks_1 : the number of non virt (and non ref) clocks of A
 \param no_of_original_clocks_2 : the number of non virt (and non ref) clocks of B
 */
bool are_zones_synced(tchecker::zg::zone_t const & zone_1, tchecker::zg::zone_t const & zone_2,
                      tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2);

/*!
 \brief checks whether the given zones are virtually equivalent
 \param zone_1 : the first zone
 \param zone_2 : the second zone
 \param no_of_original_clocks_1 : the number of non virt (and non ref) clocks of A
 \param no_of_original_clocks_2 : the number of non virt (and non ref) clocks of B
 */
bool is_virtually_equivalent(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim1,
                             tchecker::clock_id_t dim2, tchecker::clock_id_t no_of_orig_clocks_1,
                             tchecker::clock_id_t no_of_orig_clocks_2);

} // end of namespace vcg

} // end of namespace tchecker

#endif
