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
