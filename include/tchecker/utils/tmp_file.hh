/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TMP_FILE_HH
#define TCHECKER_TMP_FILE_HH

#include <string>

namespace tchecker {

/*!
 \brief creates a filename in the temporary directory that does not exist, yet.
 */
std::string create_temp_filename();

} // end of namespace tchecker

#endif