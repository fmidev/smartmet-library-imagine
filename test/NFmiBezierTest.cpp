// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiBezier
 */
// ======================================================================

#include "NFmiBezier.h"
#include "NFmiPath.h"
#include "tframe.h"
#include <cmath>

using namespace std;

//! Protection against conflicts with global functions
namespace NFmiBezierTest
{

#define EXPECT(a,b) if(a.SVG() != b) TEST_FAILED("Expected '"+string(b)+"', got '"+a.SVG()+"'");

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiBezier::CardinalApproximation
   */
  // ----------------------------------------------------------------------

  void CardinalApproximation()
  {
	using namespace Imagine;

	NFmiPath p, s;

	p.MoveTo(0,0);
	p.LineTo(0,1);
	p.LineTo(1,1);
	s = NFmiBezier::CardinalApproximation(p,0.5);
	EXPECT(s,"M0,0 C0,0.25 -0.125,0.875 0,1 0.125,1.125 0.75,1 1,1");

	p.LineTo(1,0);
	s = NFmiBezier::CardinalApproximation(p,0.5);
	EXPECT(s,"M0,0 C0,0.25 -0.125,0.875 0,1 0.125,1.125 0.875,1.125 1,1 1.125,0.875 1,0.25 1,0");

	p.LineTo(0,0);
	s = NFmiBezier::CardinalApproximation(p,0.5);
	EXPECT(s,"M0,0 C-0.125,0.125 -0.125,0.875 0,1 0.125,1.125 0.875,1.125 1,1 1.125,0.875 1.125,0.125 1,0 0.875,-0.125 0.125,-0.125 0,0");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiBezier::ToleranceApproximation
   */
  // ----------------------------------------------------------------------

  void ToleranceApproximation()
  {
	using namespace Imagine;

	TEST_NOT_IMPLEMENTED();
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
	  TEST(CardinalApproximation);
	  TEST(ToleranceApproximation);
	}
  };
 
} // namespace NFmiBezierTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl
	   << "NFmiBezier tester" << endl
	   << "=================" << endl;
  NFmiBezierTest::tests t;
  return t.run();
}

// ======================================================================
