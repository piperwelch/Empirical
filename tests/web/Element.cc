//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2020.
//  Released under the MIT Software license; see doc/LICENSE

#include <functional>
#include <unordered_map>

#include "base/assert.h"
#include "web/_MochaTestRunner.h"
#include "web/Document.h"
#include "web/Element.h"
#include "web/web.h"


// Test that the Element class properly gets attached and laid out via emp::web::Document.
struct Test_Element_HTMLLayout : public emp::web::BaseTest {

  Test_Element_HTMLLayout()
  : BaseTest({"emp_test_container"})
  { ; }

  void Setup() override {
    // Construct the following HTML structure:
    // <div id="emp_test_container">
    //   <div id="test_div">
    //     <h1 id="element_h1"><span>Header!</span></h1>
    //     <p id="element_p">
    //       <h4 id="element_h4"></h4>
    //     </p>
    //   </div>
    // </div>

    // **Empirical weirdness warning**
    // because this test will be created *after* the document's ready signal is triggered,
    // we need to manually activate the document + trigger the ready signal

    emp::web::Element header("h1", "element_h1");

    Doc("emp_test_container")
      << emp::web::Div("test_div")
      << header << "Header1!";

    Doc("emp_test_container").Div("test_div")
      << emp::web::Element("p", "element_p")
      << emp::web::Element("h4", "element_h4")
      << "Header4!";

    Redraw();
  }

  void Describe() override {

    EM_ASM({
      describe("emp::web::Element HTML Layout Scenario", function() {

        // test that everything got layed out correctly in the HTML document
        describe("div#test_div", function() {

          it('should exist', function() {
            chai.assert.equal($( "div#test_div" ).length, 1);
          });

          it('should have parent #emp_test_container', function() {
            const parent_id = $("#test_div").parent().attr("id");
            chai.assert.equal(parent_id, "emp_test_container");
          });

          it('should have child p#element_p', function() {
            chai.assert.equal($("div#test_div").children("p#element_p").length, 1);
          });

          it('should have child h1#element_h1', function() {
            chai.assert.equal($("div#test_div").children("h1#element_h1").length, 1);
          });

        });

        describe("h1#element_h1", function() {

          it('should exist', function() {
            chai.assert.equal($( "h1#element_h1" ).length, 1);
          });

          it('should have parent #test_div', function() {
            const parent_id = $("#element_h1").parent().attr("id");
            chai.assert.equal(parent_id, "test_div");
          });

        });

        describe("p#element_p", function() {

          it('should exist', function() {
            chai.assert.equal($( "p#element_p" ).length, 1);
          });

          it('should have parent #test_div', function() {
            const parent_id = $("#element_p").parent().attr("id");
            chai.assert.equal(parent_id, "test_div");
          });

          it('should have child h4#element_h4', function() {
            chai.assert.equal($("p#element_p").children("h4#element_h4").length, 1);
          });

        });

        describe("h4#element_h4", function() {

          it('should exist', function() {
            chai.assert.equal($( "h4#element_h4" ).length, 1);
          });

          it('should have parent #element_p', function() {
            const parent_id = $("#element_h4").parent().attr("id");
            chai.assert.equal(parent_id, "element_p");
          });
        });

      });
    });
  }

};

// Create a MochaTestRunner object in the global namespace so that it hangs around after main finishes.
emp::web::MochaTestRunner test_runner;

int main() {

  test_runner.Initialize({"emp_test_container"});

  // We add tests to the test runner like this:
  //  where "Test Element" is the name of the test (and does not need to be unique)
  test_runner.AddTest<Test_Element_HTMLLayout>(
    "Test Element HTML Layout"
    /*, any constructor args for test struct would go here*/
  );

  // Once we add all of the tests we want to run in this file, run them!
  test_runner.Run();
}
