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
#include "tchecker/utils/tmp_file.hh"

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

  std::string replace_1 = tchecker::create_temp_filename() + ".tck";
  std::string replace_2 = tchecker::create_temp_filename() + ".tck";

  tchecker::compare_tools::syncer_t(tmps.get_first(), tmps.get_second(), replace_1, replace_2, tmps.get_result());

  std::string expected_result = "system:synchronized\n\
\n\
event:P_a_1\n\
event:P_b_1\n\
event:P_c_1\n\
int:5:-128:127:-1:j_1\n\
clock:1:x_1\n\
process:product_1\n\
location:product_1:A{initial:}\n\
location:product_1:B{do:j_1=j_1+1 : invariant:x_1<2}\n\
location:product_1:C\n\
edge:product_1:A:B:P_a_1{provided:x_1<=0 && j_1 < 125}\n\
edge:product_1:B:C:P_b_1\n\
edge:product_1:C:A:P_c_1{do:x_1=0 : provided:x_1>3}\n\
\n\
event:P_a_2\n\
event:P_b_2\n\
event:P_c_2\n\
int:5:-128:127:-1:j_2\n\
clock:1:x_2\n\
process:product_2\n\
location:product_2:A{initial:}\n\
location:product_2:B{do:j_2=j_2+1 : invariant:x_2<2}\n\
location:product_2:C\n\
edge:product_2:A:B:P_a_2{provided:x_2<=0 && j_2 < 125}\n\
edge:product_2:B:C:P_b_2\n\
edge:product_2:C:A:P_c_2{do:x_2=0 : provided:x_2>3}\n\
\n\
sync:product_1@P_a_1:product_2@P_a_2\n\
sync:product_1@P_b_1:product_2@P_b_2\n\
sync:product_1@P_c_1:product_2@P_c_2\n\
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

  std::string replace_1 = tchecker::create_temp_filename() + ".tck";
  std::string replace_2 = tchecker::create_temp_filename() + ".tck";
  tchecker::compare_tools::syncer_t(tmps.get_first(), tmps.get_second(), replace_1, replace_2, tmps.get_result());

  std::string expected_result = "system:synchronized\n\
\n\
event:P_a_P42_c_1\n\
event:P_b_1\n\
event:P_c_1\n\
int:5:-128:127:-1:j_1\n\
clock:1:x_1\n\
clock:1:y_1\n\
process:product_1\n\
location:product_1:A_jd{initial:}\n\
location:product_1:B_jd{do:j_1=j_1+1 : invariant:x_1<2}\n\
location:product_1:C_jd\n\
edge:product_1:A_jd:B_jd:P_a_P42_c_1{do:y_1=0 : provided:x_1<=0 && j_1 < 125 : provided:y_1 > 3}\n\
edge:product_1:B_jd:C_jd:P_b_1\n\
edge:product_1:C_jd:A_jd:P_c_1{do:x_1=0 : provided:x_1>3}\n\
\n\
event:P_a_P42_c_2\n\
event:P_b_P21_b_2\n\
event:P_c_2\n\
int:5:-128:127:-1:j_2\n\
clock:1:x_2\n\
clock:1:y_2\n\
process:product_2\n\
location:product_2:A_jd_blub{initial:}\n\
location:product_2:B_jd_blub{do:j_2=j_2+1 : invariant:x_2<2}\n\
location:product_2:C_jd_blublub\n\
location:product_2:A_jd_blublub\n\
location:product_2:B_jd_blublub{do:j_2=j_2+1 : invariant:x_2<2}\n\
edge:product_2:A_jd_blub:B_jd_blub:P_a_P42_c_2{do:y_2=0 : provided:x_2<=0 && j_2 < 125 : provided:y_2 > 3}\n\
edge:product_2:B_jd_blub:C_jd_blublub:P_b_P21_b_2\n\
edge:product_2:C_jd_blublub:A_jd_blublub:P_c_2{do:x_2=0 : provided:x_2>3}\n\
edge:product_2:A_jd_blublub:B_jd_blublub:P_a_P42_c_2{do:y_2=0 : provided:x_2<=0 && j_2 < 125 : provided:y_2 > 3}\n\
\n\
sync:product_1@P_a_P42_c_1:product_2@P_a_P42_c_2\n\
sync:product_1@P_c_1:product_2@P_c_2\n\
\n\
";

  if(tmps.get_result_content() != expected_result) {
    std::size_t i = 0;
    while (i < tmps.get_result_content().size() && i < expected_result.size() &&  tmps.get_result_content()[i] == expected_result[i]) {
      ++i;
    }

    std::cerr << "First difference at position " << i << '\n';

    auto print_context = [](const std::string& s, std::size_t pos) {
        std::size_t begin = pos > 20 ? pos - 20 : 0;
        std::size_t end = std::min(pos + 20, s.size());

        for (std::size_t j = begin; j < end; ++j) {
          unsigned char c = s[j];

          if (c == '\n')
            std::cerr << "\\n";
          else if (c == '\r')
            std::cerr << "\\r";
          else if (c == '\t')
            std::cerr << "\\t";
          else
            std::cerr << c;
          }
          std::cerr << '\n';
      };

      std::cerr << "expected: ";
      print_context(expected_result, i);

      std::cerr << "actual:   ";
      print_context(tmps.get_result_content(), i);
    }
  REQUIRE(tmps.get_result_content() == expected_result);
  tmps.destroy_tmp_files();
}