/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/tmp_file.hh"

#include <filesystem>
#include <random>

namespace tchecker {


std::string create_temp_filename()
{
  auto tempDir = std::filesystem::temp_directory_path();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 999999);

  std::filesystem::path tempFile;
  do {
    tempFile = tempDir / ("tck_tmp_file_" + std::to_string(dist(gen)));
  } while (std::filesystem::exists(tempFile));

  return tempFile.string();
}

} // end of namespace tchecker