/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <filesystem>
#include <random>
#include <fstream>

#include "tchecker/compare-tools/synchronize.hh"


class Needed_Tmp_Files {
 public:
  Needed_Tmp_Files(std::string first_TA_content, std::string second_TA_content) {

    auto tempDir = std::filesystem::temp_directory_path();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 999999);

    do {
      _first = tempDir / ("Tchecker_Synchronize_Basic_Test_First" + std::to_string(dist(gen)) + ".tck");
    } while (std::filesystem::exists(_first));

    do {
      _second = tempDir / ("Tchecker_Synchronize_Basic_Test_Second" + std::to_string(dist(gen)) + ".tck");
    } while (std::filesystem::exists(_second));

    do {
      _result = tempDir / ("Tchecker_Synchronize_Basic_Test_Result" + std::to_string(dist(gen)) + ".tck");
    } while (std::filesystem::exists(_result));

    write_string_into_file(_first, first_TA_content);
    write_string_into_file(_second, second_TA_content);
  }

  void destroy_tmp_files(){
    std::filesystem::remove(_first);
    std::filesystem::remove(_second);
    std::filesystem::remove(_result);
  }

  std::string get_result_content() {
    std::ifstream file(_result);
    if (!file.is_open()) {
      throw std::runtime_error("Cannot open file: " + _result);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
  }

  std::string get_first() {
    return _first;
  }

  std::string get_second() {
    return _second;
  }

  std::string get_result() {
    return _result;
  }
  
 private:

  void write_string_into_file(std::string file_to_write, std::string content) {
    std::ofstream file(file_to_write);
    if (!file.is_open())
      throw std::runtime_error("Cannot open file: " + file_to_write); // failed to open for writing

    file << content << std::endl;
    file.close();
  }

  std::string _first;
  std::string _second;
  std::string _result;
};

TEST_CASE("Synchronize Two Basic Systems (No Network)", "[Synchronize Basic]")
{

  std::string first_TA = "system:simple \n\
clock:1:x \n\
int:5:-128:127:-1:j \n\
event:a \n\
event:b \n\
event:c \n\
\n\
process:P \n\
location:P:A{initial:} \n\
location:P:B{invariant: x<2 : do: j=j+1} \n\
location:P:C{} \n\
edge:P:A:B:a{provided: x<=0 && j < 125} \n\
edge:P:B:C:b{} \n\
edge:P:C:A:c{provided: x>3 : do: x=0} \n\
";

  std::string second_TA = "system:simple \n\
clock:1:x \n\
int:5:-128:127:-1:j \n\
event:a \n\
event:b \n\
event:c \n\
\n\
process:P \n\
location:P:A{initial:} \n\
location:P:B{invariant: x<2 : do: j=j+1} \n\
location:P:C{} \n\
edge:P:A:B:a{provided: x<=0 && j < 125} \n\
edge:P:B:C:b{} \n\
edge:P:C:A:c{provided: x>3 : do: x=0} \n\
";

  Needed_Tmp_Files tmps{first_TA, second_TA};

  tchecker::compare_tools::syncer_t(tmps.get_first(), tmps.get_second(), tmps.get_result());

  std::string expected_result = "system:synchronized\n\
\n\
clock:1:x_1 \n\
int:5:-128:127:-1:j_1 \n\
event:a_1 \n\
event:b_1 \n\
event:c_1 \n\
\n\
process:P_1 \n\
location:P_1:A{initial:} \n\
location:P_1:B{invariant: x_1<2 : do: j_1=j_1+1} \n\
location:P_1:C{} \n\
edge:P_1:A:B:a_1{provided: x_1<=0 && j_1 < 125} \n\
edge:P_1:B:C:b_1{} \n\
edge:P_1:C:A:c_1{provided: x_1>3 : do: x_1=0} \n\
\n\
\n\
clock:1:x_2 \n\
int:5:-128:127:-1:j_2 \n\
event:a_2 \n\
event:b_2 \n\
event:c_2 \n\
\n\
process:P_2 \n\
location:P_2:A{initial:} \n\
location:P_2:B{invariant: x_2<2 : do: j_2=j_2+1} \n\
location:P_2:C{} \n\
edge:P_2:A:B:a_2{provided: x_2<=0 && j_2 < 125} \n\
edge:P_2:B:C:b_2{} \n\
edge:P_2:C:A:c_2{provided: x_2>3 : do: x_2=0} \n\
\n\
\n\
sync:P_1@a_1:P_2@a_2\n\
sync:P_1@b_1:P_2@b_2\n\
sync:P_1@c_1:P_2@c_2\n\
\n\
";

  REQUIRE(tmps.get_result_content() == expected_result);
  tmps.destroy_tmp_files();
}

TEST_CASE("Synchronize Two Networks", "[Synchronize Network]")
{

  std::string first_TA = "system:complex \n\
clock:1:x \n\
clock:1:y \n\
int:5:-128:127:-1:j \n\
event:a \n\
event:b \n\
event:c \n\
\n\
process:P \n\
location:P:A{initial:} \n\
location:P:B{invariant: x<2 : do: j=j+1} \n\
location:P:C{} \n\
edge:P:A:B:a{provided: x<=0 && j < 125} \n\
edge:P:B:C:b{} \n\
edge:P:C:A:c{provided: x>3 : do: x=0} \n\
\n\
process:P42 \n\
location:P42:jd{initial:} \n\
edge:P42:jd:jd:c{provided: y > 3 : do: y=0} \n\
\n\
sync:P@a:P42@c \n\
";

  std::string second_TA = "system:complex \n\
clock:1:x \n\
clock:1:y \n\
int:5:-128:127:-1:j \n\
event:a \n\
event:b \n\
event:c \n\
\n\
process:P \n\
location:P:A{initial:} \n\
location:P:B{invariant: x<2 : do: j=j+1} \n\
location:P:C{} \n\
edge:P:A:B:a{provided: x<=0 && j < 125} \n\
edge:P:B:C:b{} \n\
edge:P:C:A:c{provided: x>3 : do: x=0} \n\
\n\
process:P42 \n\
location:P42:jd{initial:} \n\
edge:P42:jd:jd:c{provided: y > 3 : do: y=0} \n\
\n\
process:P21 \n\
location:P21:blub{initial:} \n\
location:P21:blublub{} \n\
edge:P21:blub:blublub:b{} \n\
\n\
sync:P@a:P42@c \n\
sync:P@b:P21@b \n\
";

  Needed_Tmp_Files tmps{first_TA, second_TA};

  tchecker::compare_tools::syncer_t(tmps.get_first(), tmps.get_second(), tmps.get_result());

  std::string expected_result = "system:synchronized\n\
\n\
clock:1:x_1 \n\
clock:1:y_1 \n\
int:5:-128:127:-1:j_1 \n\
event:a_1 \n\
event:b_1 \n\
event:c_1 \n\
\n\
process:P_1 \n\
location:P_1:A{initial:} \n\
location:P_1:B{invariant: x_1<2 : do: j_1=j_1+1} \n\
location:P_1:C{} \n\
edge:P_1:A:B:a_1{provided: x_1<=0 && j_1 < 125} \n\
edge:P_1:B:C:b_1{} \n\
edge:P_1:C:A:c_1{provided: x_1>3 : do: x_1=0} \n\
\n\
process:P42_1 \n\
location:P42_1:jd{initial:} \n\
edge:P42_1:jd:jd:c_1{provided: y_1 > 3 : do: y_1=0} \n\
\n\
sync:P_1@a_1:P42_1@c_1 \n\
\n\
\n\
clock:1:x_2 \n\
clock:1:y_2 \n\
int:5:-128:127:-1:j_2 \n\
event:a_2 \n\
event:b_2 \n\
event:c_2 \n\
\n\
process:P_2 \n\
location:P_2:A{initial:} \n\
location:P_2:B{invariant: x_2<2 : do: j_2=j_2+1} \n\
location:P_2:C{} \n\
edge:P_2:A:B:a_2{provided: x_2<=0 && j_2 < 125} \n\
edge:P_2:B:C:b_2{} \n\
edge:P_2:C:A:c_2{provided: x_2>3 : do: x_2=0} \n\
\n\
process:P42_2 \n\
location:P42_2:jd{initial:} \n\
edge:P42_2:jd:jd:c_2{provided: y_2 > 3 : do: y_2=0} \n\
\n\
process:P21_2 \n\
location:P21_2:blub{initial:} \n\
location:P21_2:blublub{} \n\
edge:P21_2:blub:blublub:b_2{} \n\
\n\
sync:P_2@a_2:P42_2@c_2 \n\
sync:P_2@b_2:P21_2@b_2 \n\
\n\
\n\
sync:P_1@a_1:P_2@a_2\n\
sync:P_1@a_1:P42_2@a_2\n\
sync:P_1@a_1:P21_2@a_2\n\
sync:P42_1@a_1:P_2@a_2\n\
sync:P42_1@a_1:P42_2@a_2\n\
sync:P42_1@a_1:P21_2@a_2\n\
sync:P_1@b_1:P_2@b_2\n\
sync:P_1@b_1:P42_2@b_2\n\
sync:P_1@b_1:P21_2@b_2\n\
sync:P42_1@b_1:P_2@b_2\n\
sync:P42_1@b_1:P42_2@b_2\n\
sync:P42_1@b_1:P21_2@b_2\n\
sync:P_1@c_1:P_2@c_2\n\
sync:P_1@c_1:P42_2@c_2\n\
sync:P_1@c_1:P21_2@c_2\n\
sync:P42_1@c_1:P_2@c_2\n\
sync:P42_1@c_1:P42_2@c_2\n\
sync:P42_1@c_1:P21_2@c_2\n\
\n\
";

  REQUIRE(tmps.get_result_content() == expected_result);
  tmps.destroy_tmp_files();
}