// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiApproximateBezierFit
 */
// ======================================================================

#include "NFmiApproximateBezierFit.h"
#include "NFmiPath.h"
#include "tframe.h"
#include <cmath>

using namespace std;

//! Protection against conflicts with global functions
namespace NFmiApproximateBezierFitTest
{

#define EXPECT(a,b) if(a.SVG() != b) TEST_FAILED("Expected '"+string(b)+"', got '"+a.SVG()+"'");

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiApproximateBezierFit::Fit
   */
  // ----------------------------------------------------------------------

  void Fit()
  {
	using namespace Imagine;

	{
	  NFmiPath p, s;
	  
	  p.MoveTo(0,0);
	  p.LineTo(0,3);
	  p.LineTo(3,3);
	  s = NFmiApproximateBezierFit::Fit(p,1.5);
	  EXPECT(s,"M0,0 C0,4 -1,3 3,3");
	  
	  p.LineTo(3,0);
	  s = NFmiApproximateBezierFit::Fit(p,1.5);
	  EXPECT(s,"M0,0 C0,4.5 3,4.5 3,0");
	  
	  p.LineTo(0,0);
	  s = NFmiApproximateBezierFit::Fit(p,1.5);
	  EXPECT(s,"M0,0 C-1,1 2,4 3,3 4,2 1,-1 0,0");
	}


	{
	  NFmiPath p, s;
	  p.MoveTo(85.381981,327.957031);
	  p.LineTo(84.955460,326.876068);
	  p.LineTo(83.870964,325.901672);
	  p.LineTo(81.028053,325.123413);
	  p.LineTo(80.431259,324.954956);
	  p.LineTo(77.419357,324.149628);
	  p.LineTo(74.917046,325.462891);
	  p.LineTo(71.074730,327.957031);
	  p.LineTo(71.086594,328.075500);
	  p.LineTo(70.967743,328.559448);
	  p.LineTo(70.379112,333.800873);
	  p.LineTo(70.150650,334.387573);
	  p.LineTo(70.190468,335.162323);
	  p.LineTo(70.967743,335.926910);
	  p.LineTo(74.003410,337.792358);
	  p.LineTo(74.830643,338.237854);
	  p.LineTo(77.419357,339.685333);
	  p.LineTo(78.697594,339.544037);
	  p.LineTo(83.114151,340.063751);
	  p.LineTo(83.870964,339.922058);
	  p.LineTo(84.904381,335.417603);
	  p.LineTo(84.909729,334.387573);
	  p.LineTo(85.007240,333.255005);
	  p.LineTo(85.257103,329.338654);
	  p.LineTo(85.381981,327.957031);
	  s = NFmiApproximateBezierFit::Fit(p,5);
	  EXPECT(s,"M85.382,327.957 C84.495,320.718 65.038,325.456 70.19,335.162 76.155,346.399 86.64,338.231 85.382,327.957");
	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * The actual test suite
   */
  // ----------------------------------------------------------------------

  class tests : public tframe::tests
  {
	virtual const char * const error_message_prefix() const
	{
	  return "\n\t";
	}

	void test(void)
	{
	  TEST(Fit);
	}
  };
 
} // namespace NFmiApproximateBezierFitTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl
	   << "NFmiApproximateBezierFit tester" << endl
	   << "===============================" << endl;
  NFmiApproximateBezierFitTest::tests t;
  return t.run();
}

// ======================================================================
