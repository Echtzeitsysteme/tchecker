/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 * 
 * Parts of this code were generated with Github Copilot.
 *
 */
#include "tchecker/zg/zone.hh"
#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/vcg/revert_transitions.hh"

TEST_CASE ("Extract virtual constraint", "[evc]") {

   SECTION ("Test for soundness of virtual constraints") {
      std::shared_ptr<tchecker::zg::zone_t>zone_basic_fivedim = tchecker::zg::factory(5);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc = tchecker::virtual_constraint::factory(zone_basic_fivedim, 2);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_empty = tchecker::virtual_constraint::factory(zone_basic_fivedim, 0);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_minusone;

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_big = tchecker::virtual_constraint::factory(zone_basic_fivedim, 50);

      REQUIRE(vc->get_no_of_virtual_clocks() == 2);
      REQUIRE(vc_empty->get_no_of_virtual_clocks() == 0);
      REQUIRE(vc_big->get_no_of_virtual_clocks() == 50 );
      REQUIRE_THROWS_AS(vc_minusone = tchecker::virtual_constraint::factory(zone_basic_fivedim, -1), std::invalid_argument);
   }

   std::shared_ptr<tchecker::zg::zone_t>zone_comp = tchecker::zg::factory(5);
   tchecker::clock_id_t const dim = 5;
   tchecker::clock_id_t const x = 1;
   tchecker::clock_id_t const y = 2;
   tchecker::clock_id_t const z = 3;
   tchecker::clock_id_t const w = 4;

   // create container with all clock constraints, 0 = x, 1 = y, 2 = z, 3 = w, for zero clock one has to use tchecker::REFCLOCK_ID
   tchecker::clock_constraint_container_t cc_container;
   for (unsigned int i = 0; i < dim-1; i++)
   {
      tchecker::clock_constraint_t cc_tmp = tchecker::clock_constraint_t(i, tchecker::REFCLOCK_ID, tchecker::LE, i+1);
      cc_container.push_back(cc_tmp);
      cc_tmp = tchecker::clock_constraint_t(tchecker::REFCLOCK_ID, i, tchecker::LE, -i-1);
      cc_container.push_back(cc_tmp);

      for (tchecker::clock_id_t j = 0; j < dim-1; j++)
      {
         tchecker::clock_constraint_t cc_tmp = tchecker::clock_constraint_t(i, j, tchecker::LE, i-j);
         cc_container.push_back(cc_tmp);
      }  
   }

   tchecker::dbm::constrain(zone_comp->dbm(), dim, cc_container); //Change zone-dbm according to constraints

   // Extracting virtual constraints out of given zone
   std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_comp = tchecker::virtual_constraint::factory(zone_comp, 4);
   std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_empty = tchecker::virtual_constraint::factory(zone_comp, 0);
   std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_half = tchecker::virtual_constraint::factory(zone_comp, dim/2);
   std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_double = tchecker::virtual_constraint::factory(zone_comp, 9); 

   // creating different clockvals for different boundaries
   tchecker::clockval_t * clockval_fitting = tchecker::clockval_allocate_and_construct(dim, 0);
   
   tchecker::clockval_t * clockval_x_toobig = tchecker::clockval_allocate_and_construct(dim, 0);
   tchecker::clockval_t * clockval_x_toosmall = tchecker::clockval_allocate_and_construct(dim, 0);
   
   tchecker::clockval_t * clockval_y_toobig = tchecker::clockval_allocate_and_construct(dim, 0);
   tchecker::clockval_t * clockval_y_toosmall = tchecker::clockval_allocate_and_construct(dim, 0);
   
   tchecker::clockval_t * clockval_z_toobig = tchecker::clockval_allocate_and_construct(dim, 0);
   tchecker::clockval_t * clockval_z_toosmall = tchecker::clockval_allocate_and_construct(dim, 0);

   tchecker::clockval_t * clockval_w_toobig = tchecker::clockval_allocate_and_construct(dim, 0);
   tchecker::clockval_t * clockval_w_toosmall = tchecker::clockval_allocate_and_construct(dim, 0);

   tchecker::clockval_t * clockval_empty = tchecker::clockval_allocate_and_construct(1, 0);

   tchecker::clockval_t * clockval_double = tchecker::clockval_allocate_and_construct(dim*2, 0);

   tchecker::clockval_t * clockval_half_fit = tchecker::clockval_allocate_and_construct(dim/2+1, 0);
   (*clockval_half_fit)[1] = 3;
   (*clockval_half_fit)[2] = 4;

   tchecker::clockval_t * clockval_half_z_toobig = tchecker::clockval_allocate_and_construct(dim/2 + 1, 0);
   (*clockval_half_z_toobig)[1] = 4; 
   (*clockval_half_z_toobig)[2] = 4; 

   tchecker::clockval_t * clockval_half_z_toosmall = tchecker::clockval_allocate_and_construct(dim/2 + 1, 0);
   (*clockval_half_z_toosmall)[1] = 0; 
   (*clockval_half_z_toosmall)[2] = 4; 

   tchecker::clockval_t * clockval_half_w_toobig = tchecker::clockval_allocate_and_construct(dim/2 + 1, 0);
   (*clockval_half_w_toobig)[1] = 3; 
   (*clockval_half_w_toobig)[2] = 5; 

   tchecker::clockval_t * clockval_half_w_toosmall = tchecker::clockval_allocate_and_construct(dim/2 + 1, 0);
   (*clockval_half_w_toosmall)[1] = 3; 
   (*clockval_half_w_toosmall)[2] = 0; 


   (*clockval_fitting)[x] = 1;
   (*clockval_fitting)[y] = 2;
   (*clockval_fitting)[z] = 3;
   (*clockval_fitting)[w] = 4;

   (*clockval_x_toobig)[x] = 2;
   (*clockval_x_toobig)[y] = 2;
   (*clockval_x_toobig)[z] = 3;
   (*clockval_x_toobig)[w] = 4;

   (*clockval_x_toosmall)[x] = 0;
   (*clockval_x_toosmall)[y] = 2;
   (*clockval_x_toosmall)[z] = 3;
   (*clockval_x_toosmall)[w] = 4;

   (*clockval_y_toobig)[x] = 1;
   (*clockval_y_toobig)[y] = 3;
   (*clockval_y_toobig)[z] = 3;
   (*clockval_y_toobig)[w] = 4;

   (*clockval_y_toosmall)[x] = 1;
   (*clockval_y_toosmall)[y] = 0;
   (*clockval_y_toosmall)[z] = 3;
   (*clockval_y_toosmall)[w] = 4;

   (*clockval_z_toobig)[x] = 1;
   (*clockval_z_toobig)[y] = 2;
   (*clockval_z_toobig)[z] = 4;
   (*clockval_z_toobig)[w] = 4;

   (*clockval_z_toosmall)[x] = 1;
   (*clockval_z_toosmall)[y] = 2;
   (*clockval_z_toosmall)[z] = 0;
   (*clockval_z_toosmall)[w] = 4;

   (*clockval_w_toobig)[x] = 1;
   (*clockval_w_toobig)[y] = 2;
   (*clockval_w_toobig)[z] = 3;
   (*clockval_w_toobig)[w] = 5;

   (*clockval_w_toosmall)[x] = 1;
   (*clockval_w_toosmall)[y] = 2;
   (*clockval_w_toosmall)[z] = 3;
   (*clockval_w_toosmall)[w] = 0;
   
   
   SECTION ("Test for completeness of vc_comp and zone_comp") {
      std::string base_string = "zone dbm is ";
      std::stringstream info(base_string);
      info << std::endl;
      tchecker::dbm::output_matrix(info, zone_comp->dbm(), zone_comp->dim());
      info << "vc dbm is " << std::endl;
      tchecker::dbm::output_matrix(info, vc_comp->dbm(), vc_comp->dim());
      info << "clock_val_w_toosmall is " << std::endl;
      std::function<std::string(tchecker::clock_id_t)> clk_name {[](tchecker::clock_id_t clk) {return std::to_string(clk);}};
      tchecker::output(info, *clockval_w_toosmall, clk_name  );
      INFO(info.str());
      REQUIRE(zone_comp->belongs(*clockval_fitting));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_x_toobig));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_x_toosmall));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_y_toobig));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_y_toosmall));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_z_toobig));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_z_toosmall));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_w_toobig));
      REQUIRE_FALSE(zone_comp->belongs(*clockval_w_toosmall));

      REQUIRE(vc_comp->belongs(*clockval_fitting));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_x_toobig));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_x_toosmall));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_y_toobig));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_y_toosmall));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_z_toobig));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_z_toosmall));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_w_toobig));
      REQUIRE_FALSE(vc_comp->belongs(*clockval_w_toosmall));
   }

   SECTION ("Test for completeness of vc_empty") {
      REQUIRE(vc_empty->belongs(*clockval_empty));
   }

   SECTION ("Test for vc_half") {
      REQUIRE(vc_half->belongs(*clockval_half_fit));
      REQUIRE_FALSE(vc_half->belongs(*clockval_half_z_toobig));
      REQUIRE_FALSE(vc_half->belongs(*clockval_half_z_toosmall));
      REQUIRE_FALSE(vc_half->belongs(*clockval_half_w_toobig));
      REQUIRE_FALSE(vc_half->belongs(*clockval_half_w_toosmall));
   }

   SECTION ("Test for completeness of vc_double") {
      REQUIRE(vc_double->belongs(*clockval_double));
   }

   SECTION ("Test if virtual constraint is canonic") {
      // Testing, if vc remains canonic after tightening constraints
      std::shared_ptr<tchecker::zg::zone_t>zone_canonic = tchecker::zg::factory(3);
      tchecker::clock_id_t const dim = 3;
      tchecker::clock_id_t const x = 1;
      tchecker::clock_id_t const y = 2;

      // create zone:   
      // (0,<=)     (-2,<=)    (-1,<=)
      // (inf,<=)   (0,<=)     (1,<=)       
      // (inf,<=)   (-1,<=)    (0,<=)
      zone_canonic->dbm()[0] = tchecker::dbm::LE_ZERO;
      zone_canonic->dbm()[1] = tchecker::dbm::db(tchecker::LE, -2); 
      zone_canonic->dbm()[2] = tchecker::dbm::db(tchecker::LE, -1);
      zone_canonic->dbm()[3] = tchecker::dbm::LT_INFINITY;
      zone_canonic->dbm()[4] = tchecker::dbm::LE_ZERO;
      zone_canonic->dbm()[5] = tchecker::dbm::db(tchecker::LE, 1);
      zone_canonic->dbm()[6] = tchecker::dbm::LT_INFINITY;
      zone_canonic->dbm()[7] = tchecker::dbm::db(tchecker::LE, -1);
      zone_canonic->dbm()[8] = tchecker::dbm::LE_ZERO;

      tchecker::clockval_t * clockval_fitting = tchecker::clockval_allocate_and_construct(dim, 0);
      (*clockval_fitting)[x] = 2;
      (*clockval_fitting)[y] = 1;

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc_canonic = tchecker::virtual_constraint::factory(zone_canonic, 2);
      
      // two changes to vc necessary in order to keep dbm tight
      vc_canonic->dbm()[1] = tchecker::dbm::db(tchecker::LE, -3);
      vc_canonic->dbm()[5] = tchecker::dbm::db(tchecker::LE, 2);
      REQUIRE(zone_canonic->belongs(*clockval_fitting));
      REQUIRE_FALSE(vc_canonic->belongs(*clockval_fitting));

      tchecker::clockval_destruct_and_deallocate(clockval_fitting);
   }

   tchecker::clockval_destruct_and_deallocate(clockval_fitting);

   tchecker::clockval_destruct_and_deallocate(clockval_x_toobig);
   tchecker::clockval_destruct_and_deallocate(clockval_x_toosmall);
   
   tchecker::clockval_destruct_and_deallocate(clockval_y_toobig);
   tchecker::clockval_destruct_and_deallocate(clockval_y_toosmall);
   
   tchecker::clockval_destruct_and_deallocate(clockval_z_toobig);
   tchecker::clockval_destruct_and_deallocate(clockval_z_toosmall);
   
   tchecker::clockval_destruct_and_deallocate(clockval_w_toobig);
   tchecker::clockval_destruct_and_deallocate(clockval_w_toosmall);
   
   tchecker::clockval_destruct_and_deallocate(clockval_empty);
   
   tchecker::clockval_destruct_and_deallocate(clockval_half_fit);
   tchecker::clockval_destruct_and_deallocate(clockval_half_z_toobig);
   tchecker::clockval_destruct_and_deallocate(clockval_half_z_toosmall);
   tchecker::clockval_destruct_and_deallocate(clockval_half_w_toobig);
   tchecker::clockval_destruct_and_deallocate(clockval_half_w_toosmall);
   
   tchecker::clockval_destruct_and_deallocate(clockval_double);
}  

TEST_CASE ("Revert Action Trans Operator", "[rato]") {
   tchecker::clock_id_t const dim = 5;
   tchecker::clock_id_t const x = 0;
   tchecker::clock_id_t const y = 1;
   tchecker::clock_id_t const z = 2;
   tchecker::clock_id_t const w = 3;

   SECTION("Trivial reset and invariants to zero") {
      // Create previous zone D_A
      std::shared_ptr<tchecker::zg::zone_t> zone_D_A = tchecker::zg::factory(dim);
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> evc_D_A = tchecker::virtual_constraint::factory(zone_D_A, dim-1);

      // Create guard for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> guard = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create reset container for the transition
      std::shared_ptr<tchecker::clock_reset_container_t> reset = std::make_shared<tchecker::clock_reset_container_t>();

      // Create invariants for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> tgt_invariant = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create Zone D_sigma_A which is the target zone of the transition
      std::shared_ptr<tchecker::zg::zone_t> zone_D_sigma_A_0 = tchecker::zg::factory(dim);

      // Set guard constraints
      for (unsigned int i = 0; i < dim-1; i++)
         {
            // guard->emplace_back(i, tchecker::REFCLOCK_ID, tchecker::LE, i+1);
            guard->emplace_back(tchecker::REFCLOCK_ID, i, tchecker::LE, -i-1);
         }
      
      tchecker::dbm::constrain(zone_D_A->dbm(), dim, *guard); //Change zone-dbm according to constraints

      // Set reset values
      reset->emplace_back(x, tchecker::REFCLOCK_ID, 0);
      reset->emplace_back(y, tchecker::REFCLOCK_ID, 0);
      reset->emplace_back(z, tchecker::REFCLOCK_ID, 0);
      reset->emplace_back(w, tchecker::REFCLOCK_ID, 0);

      // Set invariants to zero
      for (tchecker::clock_id_t i = 1; i < dim-1; i++)
      {
         tgt_invariant->emplace_back(i, tchecker::REFCLOCK_ID, tchecker::LE, 0);
      }

      tchecker::dbm::reset(zone_D_sigma_A_0->dbm(), zone_D_sigma_A_0->dim(), *reset);
      tchecker::dbm::constrain(zone_D_sigma_A_0->dbm(), dim, *tgt_invariant);

      // Create the VC for the transition with phi_split
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_split = tchecker::virtual_constraint::factory(zone_D_sigma_A_0, dim-1);

      // Call rever_action_trans function and write result to phi_res_0
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_res_0 = tchecker::vcg::revert_action_trans(*zone_D_A, *guard, *reset, *tgt_invariant, *phi_split);

      REQUIRE(*phi_res_0 <= *zone_D_A);
      REQUIRE(*phi_res_0 <= *evc_D_A);
   }

   SECTION("Non-trivial test for usefulness - x") {
      // Create previous zone D_A
      std::shared_ptr<tchecker::zg::zone_t> zone_D_A_1 = tchecker::zg::factory(dim);
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> evc_D_A_1 = tchecker::virtual_constraint::factory(zone_D_A_1, dim-1);

      // Create guard for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> guard_1 = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create reset container for the transition
      std::shared_ptr<tchecker::clock_reset_container_t> reset_1 = std::make_shared<tchecker::clock_reset_container_t>();

      // Create invariants for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> tgt_invariant_1 = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create Zone D_sigma_A which is the target zone of the transition
      std::shared_ptr<tchecker::zg::zone_t> zone_D_sigma_A_1 = tchecker::zg::factory(dim);

      // Set guard constraints
      for (unsigned int i = 0; i < dim-1; i++)
      {
         guard_1->emplace_back(tchecker::REFCLOCK_ID, i, tchecker::LE, -i-1);
      }
      
      tchecker::dbm::constrain(zone_D_A_1->dbm(), dim, *guard_1); //Change zone-dbm according to constraints

      reset_1->emplace_back(y, tchecker::REFCLOCK_ID, 0);
      reset_1->emplace_back(z, tchecker::REFCLOCK_ID, 0);
      reset_1->emplace_back(w, tchecker::REFCLOCK_ID, 0);
      
      tgt_invariant_1->emplace_back(x, tchecker::REFCLOCK_ID, tchecker::LE, 1);

      tchecker::dbm::constrain(zone_D_sigma_A_1->dbm(), dim, *guard_1);
      tchecker::dbm::reset(zone_D_sigma_A_1->dbm(), zone_D_sigma_A_1->dim(), *reset_1);
      tchecker::dbm::constrain(zone_D_sigma_A_1->dbm(), dim, *tgt_invariant_1);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_split_1 = tchecker::virtual_constraint::factory(zone_D_sigma_A_1, dim-1);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_res_1 = tchecker::vcg::revert_action_trans(*zone_D_A_1, *guard_1, *reset_1, *tgt_invariant_1, *phi_split_1);

      REQUIRE(*phi_res_1 <= *zone_D_A_1);
      REQUIRE(*phi_res_1 <= *evc_D_A_1);
   }

   SECTION("Second non-trivial test for usefulness - y") {
      // Create previous zone D_A
      std::shared_ptr<tchecker::zg::zone_t> zone_D_A_2 = tchecker::zg::factory(dim);
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> evc_D_A_2 = tchecker::virtual_constraint::factory(zone_D_A_2, dim-1);

      // Create guard for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> guard_2 = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create reset container for the transition
      std::shared_ptr<tchecker::clock_reset_container_t> reset_2 = std::make_shared<tchecker::clock_reset_container_t>();

      // Create invariants for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> tgt_invariant_2 = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create Zone D_sigma_A which is the target zone of the transition
      std::shared_ptr<tchecker::zg::zone_t> zone_D_sigma_A_2 = tchecker::zg::factory(dim);

      // Set guard constraints
      for (unsigned int i = 0; i < dim-1; i++)
      {
         guard_2->emplace_back(tchecker::REFCLOCK_ID, i, tchecker::LE, -i-1);
      }
   
      tchecker::dbm::constrain(zone_D_A_2->dbm(), dim, *guard_2); //Change zone-dbm according to constraints

      reset_2->emplace_back(z, tchecker::REFCLOCK_ID, 0);
      reset_2->emplace_back(w, tchecker::REFCLOCK_ID, 0);
      
      tgt_invariant_2->emplace_back(x, tchecker::REFCLOCK_ID, tchecker::LE, 1);
      tgt_invariant_2->emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LE, 2);

      tchecker::dbm::constrain(zone_D_sigma_A_2->dbm(), dim, *guard_2);
      tchecker::dbm::reset(zone_D_sigma_A_2->dbm(), zone_D_sigma_A_2->dim(), *reset_2);
      tchecker::dbm::constrain(zone_D_sigma_A_2->dbm(), dim, *tgt_invariant_2);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_split_2 = tchecker::virtual_constraint::factory(zone_D_sigma_A_2, dim-1);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_res_2 = tchecker::vcg::revert_action_trans(*zone_D_A_2, *guard_2, *reset_2, *tgt_invariant_2, *phi_split_2);

      REQUIRE(*phi_res_2 <= *zone_D_A_2);
      REQUIRE(*phi_res_2 <= *evc_D_A_2);
   }

   SECTION("Third non-trivial test for usefulness - z") {
      // Create previous zone D_A
      std::shared_ptr<tchecker::zg::zone_t> zone_D_A_3 = tchecker::zg::factory(dim);
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> evc_D_A_3 = tchecker::virtual_constraint::factory(zone_D_A_3, dim-1);

      // Create guard for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> guard_3 = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create reset container for the transition
      std::shared_ptr<tchecker::clock_reset_container_t> reset_3 = std::make_shared<tchecker::clock_reset_container_t>();

      // Create invariants for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> tgt_invariant_3 = std::make_shared<tchecker::clock_constraint_container_t>();

      // Create Zone D_sigma_A which is the target zone of the transition
      std::shared_ptr<tchecker::zg::zone_t> zone_D_sigma_A_3 = tchecker::zg::factory(dim);

      // Set guard constraints
      for (unsigned int i = 0; i < dim-1; i++)
      {
         guard_3->emplace_back(tchecker::REFCLOCK_ID, i, tchecker::LE, -i-1);
      }
   
      tchecker::dbm::constrain(zone_D_A_3->dbm(), dim, *guard_3); //Change zone-dbm according to constraints

      reset_3->emplace_back(w, tchecker::REFCLOCK_ID, 0);
      
      tgt_invariant_3->emplace_back(x, tchecker::REFCLOCK_ID, tchecker::LE, 1);
      tgt_invariant_3->emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LE, 2);
      tgt_invariant_3->emplace_back(z, tchecker::REFCLOCK_ID, tchecker::LE, 3);

      tchecker::dbm::constrain(zone_D_sigma_A_3->dbm(), dim, *guard_3);
      tchecker::dbm::reset(zone_D_sigma_A_3->dbm(), zone_D_sigma_A_3->dim(), *reset_3);
      tchecker::dbm::constrain(zone_D_sigma_A_3->dbm(), dim, *tgt_invariant_3);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_split_3 = tchecker::virtual_constraint::factory(zone_D_sigma_A_3, dim-1);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_res_3 = tchecker::vcg::revert_action_trans(*zone_D_A_3, *guard_3, *reset_3, *tgt_invariant_3, *phi_split_3);

      REQUIRE(*phi_res_3 <= *zone_D_A_3);
      REQUIRE(*phi_res_3 <= *evc_D_A_3);
   }

   SECTION("Non-trivial test for usefulness - w") {
      // Create previous zone D_A
      std::shared_ptr<tchecker::zg::zone_t> zone_D_A_4 = tchecker::zg::factory(dim);
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> evc_D_A_4 = tchecker::virtual_constraint::factory(zone_D_A_4, dim-1);
      // Create guard for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> guard_4 = std::make_shared<tchecker::clock_constraint_container_t>();
      // Create reset container for the transition
      std::shared_ptr<tchecker::clock_reset_container_t> reset_4 = std::make_shared<tchecker::clock_reset_container_t>();
      // Create invariants for the transition
      std::shared_ptr<tchecker::clock_constraint_container_t> tgt_invariant_4 = std::make_shared<tchecker::clock_constraint_container_t>();
      // Create Zone D_sigma_A which is the target zone of the transition
      std::shared_ptr<tchecker::zg::zone_t> zone_D_sigma_A_4 = tchecker::zg::factory(dim);
      
      // Set guard constraints
      for (unsigned int i = 0; i < dim-1; i++) {
         guard_4->emplace_back(tchecker::REFCLOCK_ID, i, tchecker::LE, -i-1);
      }

      tchecker::dbm::constrain(zone_D_A_4->dbm(), dim, *guard_4); //Change zone-dbm according to constraints

      tgt_invariant_4->emplace_back(x, tchecker::REFCLOCK_ID, tchecker::LE, 1);
      tgt_invariant_4->emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LE, 2);
      tgt_invariant_4->emplace_back(z, tchecker::REFCLOCK_ID, tchecker::LE, 3);
      tgt_invariant_4->emplace_back(w, tchecker::REFCLOCK_ID, tchecker::LE, 4);

      tchecker::dbm::constrain(zone_D_sigma_A_4->dbm(), dim, *guard_4);
      tchecker::dbm::reset(zone_D_sigma_A_4->dbm(), zone_D_sigma_A_4->dim(), *reset_4);
      tchecker::dbm::constrain(zone_D_sigma_A_4->dbm(), dim, *tgt_invariant_4);

      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_split_4 = tchecker::virtual_constraint::factory(zone_D_sigma_A_4, dim-1);
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_res_4 = tchecker::vcg::revert_action_trans(*zone_D_A_4, *guard_4, *reset_4, *tgt_invariant_4, *phi_split_4);

      REQUIRE(*phi_res_4 <= *zone_D_A_4);
      REQUIRE(*phi_res_4 <= *evc_D_A_4);
   }
}
