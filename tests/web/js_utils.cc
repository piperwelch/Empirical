//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2016-2020.
//  Released under the MIT Software license; see doc/LICENSE

#include <string>
#include <array>

#include "web/_MochaTestRunner.h"
#include "../../tests2/unit_tests.h"
#include "config/command_line.h"
#include "base/assert.h"
#include "base/vector.h"
#include "web/init.h"
#include "web/JSWrap.h"
#include "web/js_utils.h"

#include <cassert>

// This file tests source/web/js_utils.h (using Mocha + Karma framework)
// - pass_array_to_javascript
// - pass_array_to_cpp

struct Test_pass_array_to_javascript : public emp::web::BaseTest {

  struct JSDataObject {
    EMP_BUILD_INTROSPECTIVE_TUPLE(
          int, val,
          std::string, word,
          double, val2
      )
  };

  JSDataObject test_obj_1{};
  JSDataObject test_obj_2{};

  emp::vector<uint32_t> wrapped_fun_ids;

  Test_pass_array_to_javascript() { Setup(); }

  ~Test_pass_array_to_javascript() {
    // cleanup wrapped functions
    std::for_each(
      wrapped_fun_ids.begin(),
      wrapped_fun_ids.end(),
      [](uint32_t fun_id) {
        emp::JSDelete(fun_id);
      }
    );
  }

  void Setup() {
    test_obj_1.val() = 10;
    test_obj_1.word() = "hi";
    test_obj_1.val2() = 4.4;

    test_obj_2.val() = 40;
    test_obj_2.word() = "hi2";
    test_obj_2.val2() = 11.2;

    // Configure functions that can be called from JS that pass various C++ vectors/arrays to JS
    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        [](){
          emp::vector<emp::vector<emp::vector<double> > > nested_vec{{{1,2,3},{4,5,6}}};
          emp::pass_array_to_javascript(nested_vec);
        },
        "PassNestedVectorToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        []() {
          emp::vector<int> int_vec{5,1,2,3,6};
          emp::pass_array_to_javascript(int_vec);
        },
        "PassIntVectorToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        []() {
          emp::vector<std::string> string_vec{"a", "vector", "of", "strings"};
          emp::pass_array_to_javascript(string_vec);
        },
        "PassStringVectorToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        []() {
          emp::array<int32_t, 3> test_data{{10,30,60}};
          emp::pass_array_to_javascript(test_data);
        },
        "PassArrayIntToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        [this]() {
          emp::array<JSDataObject, 2> test_data_2{{test_obj_1, test_obj_2}};
          emp::pass_array_to_javascript(test_data_2);
        },
        "PassArrayJSDataObjectToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        []() {
          emp::array<emp::array<std::string, 5>, 1> string_arr{{{{"do", "strings", "work", "in", "arrays?"}}}};
          emp::pass_array_to_javascript(string_arr);
        },
        "PassNestedArrayStringToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        []() {
          emp::array<emp::array<emp::array<int, 2>, 1>, 5> horrible_array{{{{{{0,0}}}}, {{{{0,10}}}}, {{{{10,10}}}}, {{{{20,20}}}}, {{{{30, 30}}}}}};
          emp::pass_array_to_javascript(horrible_array);
        },
        "PassNestedArrayIntToJS",
        false
      )
    );

    wrapped_fun_ids.emplace_back(
      emp::JSWrap(
        [this]() {
          emp::array<emp::array<JSDataObject, 2>, 2> test_data_4{{{{test_obj_1, test_obj_2}}, {{test_obj_2, test_obj_2}}}};
          emp::pass_array_to_javascript(test_data_4);
        },
        "PassNestedArrayJSDataObjectToJS",
        false
      )
    );
  }

  void Describe() override {

    // Test passing arrays to javascript
    EM_ASM({
      describe("pass_array_to_javascript", function() {
        it("should pass nested C++ vectors to javascript", function() {
          emp.PassNestedVectorToJS(); // {{1,2,3},{4,5,6}}}
          const incoming = emp_i.__incoming_array;
          chai.assert.equal(incoming[0][0][0], 1);
          chai.assert.equal(incoming[0][0][1], 2);
          chai.assert.equal(incoming[0][0][2], 3);
          chai.assert.equal(incoming[0][1][0], 4);
          chai.assert.equal(incoming[0][1][1], 5);
          chai.assert.equal(incoming[0][1][2], 6);
        });
        it("should pass emp::vector<int> to javascript", function() {
          emp.PassIntVectorToJS(); // {5,1,2,3,6}
          const incoming = emp_i.__incoming_array;
          chai.assert.deepEqual(incoming, [5, 1, 2, 3, 6]);
        });
        it("should pass emp::vector<std::string> to javascript", function() {
          emp.PassStringVectorToJS(); // {"a", "vector", "of", "strings"}
          const incoming = emp_i.__incoming_array;
          chai.assert.deepEqual(incoming, ['a', 'vector', 'of', 'strings']);
        });
        it("should pass emp::array<int, N> to javascript", function() {
          emp.PassArrayIntToJS(); // {{10,30,60}}
          const incoming = emp_i.__incoming_array;
          chai.assert.deepEqual(incoming, [10, 30, 60]);
        });
        it("should pass emp::array<JSDataObject, N> to javascript", function() {
          emp.PassArrayJSDataObjectToJS();
          const incoming = emp_i.__incoming_array;
          chai.assert.deepEqual(incoming, [{val: 10, word: 'hi', val2: 4.4}, {val: 40, word: 'hi2', val2: 11.2}]);
        });
        it("should pass emp::array<emp::array<std::string, N0>, N1> to javascript", function() {
          emp.PassNestedArrayStringToJS();
          const incoming = emp_i.__incoming_array;
          // chai.assert.deepEqual(incoming, ['do', 'strings', 'work', 'in', 'arrays?']);
          chai.assert.equal(incoming[0][3], "in")
        });
        it("should pass emp::array<emp::array<emp::array<int, N0>, N1>, N2> to javascript", function() {
          emp.PassNestedArrayIntToJS(); // {{{{{{0,0}}}}, {{{{0,10}}}}, {{{{10,10}}}}, {{{{20,20}}}}, {{{{30, 30}}}}}}
          const incoming = emp_i.__incoming_array;
          chai.assert.equal(incoming[4][0][0], 30);
        });
        it("should pass emp::array<emp::array<JSDataObject, N0>, N1> to javascript", function() {
          emp.PassNestedArrayJSDataObjectToJS(); // {{{{test_obj_1, test_obj_2}}, {{test_obj_2, test_obj_2}}}};
          const incoming = emp_i.__incoming_array;
          chai.assert.equal(incoming[1][0].val, 40);
          chai.assert.equal(incoming[1][0].val2, 11.2);
        });
      });
    });
  }

};


// Because we want to check values on the C++ end, this test relies on raw C++ asserts (from cassert)
// to check values.
struct Test_pass_array_to_cpp : public emp::web::BaseTest {

  Test_pass_array_to_cpp() { Setup(); }

  void Setup() {

    // Test ints
    EM_ASM({emp_i.__outgoing_array = ([5, 1, 3])});
    emp::array<int, 3> test_arr_1;
    emp::pass_array_to_cpp(test_arr_1);
    assert(test_arr_1[0] == 5);
    assert(test_arr_1[1] == 1);
    assert(test_arr_1[2] == 3);

    // Test floats
    EM_ASM({emp_i.__outgoing_array = ([5.2, 1.5, 3.1])});
    emp::array<float, 3> test_arr_2;
    emp::pass_array_to_cpp(test_arr_2);
    assert(emp::to_string(test_arr_2[0]) == emp::to_string(5.2));
    assert(test_arr_2[1] == 1.5);
    assert(emp::to_string(test_arr_2[2]) == emp::to_string(3.1));

    // Test doubles
    EM_ASM({emp_i.__outgoing_array = ([5.2, 1.5, 3.1])});
    emp::array<double, 3> test_arr_3;
    emp::pass_array_to_cpp(test_arr_3);
    assert(test_arr_3[0] == 5.2);
    assert(test_arr_3[1] == 1.5);
    assert(test_arr_3[2] == 3.1);

    // Test doubles in vector
    EM_ASM({emp_i.__outgoing_array = ([5.3, 1.6, 3.2])});
    emp::vector<double> test_vec;
    emp::pass_vector_to_cpp(test_vec);
    assert(test_vec[0] == 5.3);
    assert(test_vec[1] == 1.6);
    assert(test_vec[2] == 3.2);

    // Test chars
    EM_ASM({emp_i.__outgoing_array = (["h", "i", "!"])});
    emp::array<char, 3> test_arr_4;
    emp::pass_array_to_cpp(test_arr_4);
    assert(test_arr_4[0] == 'h');
    assert(test_arr_4[1] == 'i');
    assert(test_arr_4[2] == '!');
    emp::vector<char> test_vec_4;
    emp::pass_vector_to_cpp(test_vec_4);
    assert(test_vec_4[0] == 'h');
    assert(test_vec_4[1] == 'i');
    assert(test_vec_4[2] == '!');

    // Test std::strings
    EM_ASM({emp_i.__outgoing_array = (["jello", "world", "!!"])});
    emp::array<std::string, 3> test_arr_5;
    emp::pass_array_to_cpp(test_arr_5);
    assert(test_arr_5[0] == "jello");
    assert(test_arr_5[1] == "world");
    assert(test_arr_5[2] == "!!");
    emp::vector<std::string> test_vec_5;
    emp::pass_vector_to_cpp(test_vec_5);
    assert(test_vec_5[0] == "jello");
    assert(test_vec_5[1] == "world");
    assert(test_vec_5[2] == "!!");

    // Test nested arrays
    EM_ASM({emp_i.__outgoing_array = ([[4,5], [3,1], [7,8]])});
    emp::array<emp::array<int, 2>, 3> test_arr_6;
    emp::pass_array_to_cpp(test_arr_6);
    assert(test_arr_6[0][0] == 4);
    assert(test_arr_6[0][1] == 5);
    assert(test_arr_6[1][0] == 3);
    assert(test_arr_6[1][1] == 1);
    assert(test_arr_6[2][0] == 7);
    assert(test_arr_6[2][1] == 8);

    // Test nested vectors
    EM_ASM({emp_i.__outgoing_array = ([[4,5], [3,1], [7,8]])});
    emp::vector<emp::vector<int> > test_vec_6;
    emp::pass_vector_to_cpp(test_vec_6);
    assert(test_vec_6[0][0] == 4);
    assert(test_vec_6[0][1] == 5);
    assert(test_vec_6[1][0] == 3);
    assert(test_vec_6[1][1] == 1);
    assert(test_vec_6[2][0] == 7);
    assert(test_vec_6[2][1] == 8);

    // Test more deeply nested arrays
    EM_ASM({emp_i.__outgoing_array = ([[["Sooo", "many"], ["strings", "here"]],
      [["and", "they're"], ["all", "nested"]],
      [["in", "this"], ["nested", "array!"]]]);});
    emp::array<emp::array<emp::array<std::string, 2>, 2>, 3> test_arr_7;
    emp::pass_array_to_cpp(test_arr_7);
    assert(test_arr_7[0][0][0] == "Sooo");
    assert(test_arr_7[0][0][1] == "many");
    assert(test_arr_7[0][1][0] == "strings");
    assert(test_arr_7[0][1][1] == "here");
    assert(test_arr_7[1][0][0] == "and");
    assert(test_arr_7[1][0][1] == "they're");
    assert(test_arr_7[1][1][0] == "all");
    assert(test_arr_7[1][1][1] == "nested");
    assert(test_arr_7[2][0][0] == "in");
    assert(test_arr_7[2][0][1] == "this");
    assert(test_arr_7[2][1][0] == "nested");
    assert(test_arr_7[2][1][1] == "array!");
  }

};

emp::web::MochaTestRunner test_runner;

int main() {
  emp::Initialize();

  test_runner.AddTest<Test_pass_array_to_javascript>("Test pass_array_to_javascript");
  test_runner.AddTest<Test_pass_array_to_cpp>("Test pass_array_to_cpp");

  test_runner.Run();

}