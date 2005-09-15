// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiGpcTools
 */
// ======================================================================

#include "NFmiGpcTools.h"
#include "NFmiPath.h"
#include "tframe.h"
#include <sstream>

//! Protection against conflicts with global functions
namespace NFmiGpcToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiGpcToolsTest::And
   */
  // ----------------------------------------------------------------------

  void And()
  {
	using namespace Imagine;

	// And rectangle with itself
	{
	  NFmiPath p;
	  p.Add("M0,0 L0,1 L1,1 L1,0 L0,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p,p);
	  if(os.str() != "M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("p AND p failed: "+os.str());
	}

	// And two disjoint rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M2,0 L2,1 L3,1 L3,0 L2,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "")
		TEST_FAILED("AND failed for disjoint rectangles: "+os.str());
	}

	// And two touching rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M1,0 L1,1 L2,1 L2,0 L1,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "")
		TEST_FAILED("AND failed for touching rectangles: "+os.str());
	}

	// And rectangle inside another
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,3 L3,3 L3,0 L0,0");
	  p2.Add("M1,1 L1,2 L2,2 L2,1 L1,1");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "M1,1 L1,2 L2,2 L2,1 L1,1")
		TEST_FAILED("AND failed for rectangle inside another: "+os.str());
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
	  TEST(And);
	}
  };
 
} // namespace NFmiGpcToolsTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl
	   << "NFmiGpcTools tester" << endl
	   << "===================" << endl;
  NFmiGpcToolsTest::tests t;
  return t.run();
}

// ======================================================================
