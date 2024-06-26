/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */
 #include "tchecker/zg/zone.hh"
 #include "tchecker/vcg/virtual_constraint.hh"
 

 TEST_CASE ("Extract virtual constraint", "[evc]") {
    std::shared_ptr<tchecker::zg::zone_t>zone = tchecker::zg::factory(5);
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc = tchecker::virtual_constraint::factory(zone, 2);
    REQUIRE(vc->get_no_of_virtual_clocks() == 2);
 }
