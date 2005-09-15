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

	// AND rectangle with itself
	{
	  NFmiPath p;
	  p.Add("M0,0 L0,1 L1,1 L1,0 L0,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p,p);
	  if(os.str() != "M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("p AND p failed: "+os.str());
	}

	// AND two disjoint rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M2,0 L2,1 L3,1 L3,0 L2,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "")
		TEST_FAILED("AND failed for disjoint rectangles: "+os.str());
	}

	// AND two touching rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M1,0 L1,1 L2,1 L2,0 L1,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "")
		TEST_FAILED("AND failed for touching rectangles: "+os.str());
	}

	// AND rectangle inside another
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,3 L3,3 L3,0 L0,0");
	  p2.Add("M1,1 L1,2 L2,2 L2,1 L1,1");

	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "M2,1 L1,1 L1,2 L2,2 L2,1")
		TEST_FAILED("AND failed for rectangle inside another: "+os.str());
	}

	// AND intersecting rectangles

	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,5 L5,5 L5,0 L0,0");
	  p2.Add("M2,2 L2,3 L10,3 L 10,2 L 2,2");
	  
	  std::ostringstream os;
	  os << NFmiGpcTools::And(p1,p2);
	  if(os.str() != "M5,2 L2,2 L2,3 L5,3 L5,2")
		TEST_FAILED("AND failed for intersecting rectangles: "+os.str());
	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiGpcToolsTest::Or
   */
  // ----------------------------------------------------------------------

  void Or()
  {
	using namespace Imagine;

	// OR rectangle with itself
	{
	  NFmiPath p;
	  p.Add("M0,0 L0,1 L1,1 L1,0 L0,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Or(p,p);
	  if(os.str() != "M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("p OR p failed: "+os.str());
	}

	// OR two disjoint rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M2,0 L2,1 L3,1 L3,0 L2,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Or(p1,p2);
	  if(os.str() != "M3,0 L2,0 L2,1 L3,1 L3,0 M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("OR failed for disjoint rectangles: "+os.str());
	}

	// OR two touching rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M1,0 L1,1 L2,1 L2,0 L1,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Or(p1,p2);
	  if(os.str() != "M2,0 L0,0 L0,1 L2,1 L2,0")
		TEST_FAILED("OR failed for touching rectangles: "+os.str());
	}

	// OR rectangle inside another
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,3 L3,3 L3,0 L0,0");
	  p2.Add("M1,1 L1,2 L2,2 L2,1 L1,1");

	  std::ostringstream os;
	  os << NFmiGpcTools::Or(p1,p2);
	  if(os.str() != "M3,0 L0,0 L0,3 L3,3 L3,0")
		TEST_FAILED("OR failed for rectangle inside another: "+os.str());
	}

	// OR intersecting rectangles

	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,5 L5,5 L5,0 L0,0");
	  p2.Add("M2,2 L2,3 L10,3 L 10,2 L 2,2");
	  
	  std::ostringstream os;
	  os << NFmiGpcTools::Or(p1,p2);
	  if(os.str() != "M5,3 L10,3 L10,2 L5,2 L5,0 L0,0 L0,5 L5,5 L5,3")
		TEST_FAILED("OR failed for intersecting rectangles: "+os.str());
	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiGpcToolsTest::Minus
   */
  // ----------------------------------------------------------------------

  void Minus()
  {
	using namespace Imagine;

	// MINUS rectangle with itself
	{
	  NFmiPath p;
	  p.Add("M0,0 L0,1 L1,1 L1,0 L0,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Minus(p,p);
	  if(os.str() != "")
		TEST_FAILED("p MINUS p failed: "+os.str());
	}

	// MINUS two disjoint rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M2,0 L2,1 L3,1 L3,0 L2,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Minus(p1,p2);
	  if(os.str() != "M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("MINUS failed for disjoint rectangles: "+os.str());
	}

	// MINUS two touching rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M1,0 L1,1 L2,1 L2,0 L1,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Minus(p1,p2);
	  if(os.str() != "M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("MINUS failed for touching rectangles: "+os.str());
	}

	// MINUS rectangle inside another
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,3 L3,3 L3,0 L0,0");
	  p2.Add("M1,1 L1,2 L2,2 L2,1 L1,1");

	  std::ostringstream os;
	  os << NFmiGpcTools::Minus(p1,p2);
	  if(os.str() != "M2,2 L1,2 L1,1 L2,1 L2,2 M3,0 L0,0 L0,3 L3,3 L3,0")
		TEST_FAILED("MINUS failed for rectangle inside another: "+os.str());
	}

	// MINUS intersecting rectangles

	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,5 L5,5 L5,0 L0,0");
	  p2.Add("M2,2 L2,3 L10,3 L 10,2 L 2,2");
	  
	  std::ostringstream os;
	  os << NFmiGpcTools::Minus(p1,p2);
	  if(os.str() != "M5,3 L2,3 L2,2 L5,2 L5,0 L0,0 L0,5 L5,5 L5,3")
		TEST_FAILED("MINUS failed for intersecting rectangles: "+os.str());
	}

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiGpcToolsTest::Xor
   */
  // ----------------------------------------------------------------------

  void Xor()
  {
	using namespace Imagine;

	// XOR rectangle with itself
	{
	  NFmiPath p;
	  p.Add("M0,0 L0,1 L1,1 L1,0 L0,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Xor(p,p);
	  if(os.str() != "")
		TEST_FAILED("p XOR p failed: "+os.str());
	}

	// XOR two disjoint rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M2,0 L2,1 L3,1 L3,0 L2,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Xor(p1,p2);
	  if(os.str() != "M3,0 L2,0 L2,1 L3,1 L3,0 M1,0 L0,0 L0,1 L1,1 L1,0")
		TEST_FAILED("XOR failed for disjoint rectangles: "+os.str());
	}

	// XOR two touching rectangles
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,1 L1,1 L1,0 L0,0");
	  p2.Add("M1,0 L1,1 L2,1 L2,0 L1,0");

	  std::ostringstream os;
	  os << NFmiGpcTools::Xor(p1,p2);
	  if(os.str() != "M2,0 L0,0 L0,1 L2,1 L2,0")
		TEST_FAILED("XOR failed for touching rectangles: "+os.str());
	}

	// XOR rectangle inside another
	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,3 L3,3 L3,0 L0,0");
	  p2.Add("M1,1 L1,2 L2,2 L2,1 L1,1");

	  std::ostringstream os;
	  os << NFmiGpcTools::Xor(p1,p2);
	  if(os.str() != "M2,2 L1,2 L1,1 L2,1 L2,2 M3,0 L0,0 L0,3 L3,3 L3,0")
		TEST_FAILED("XOR failed for rectangle inside another: "+os.str());
	}

	// XOR intersecting rectangles

	{
	  NFmiPath p1, p2;
	  p1.Add("M0,0 L0,5 L5,5 L5,0 L0,0");
	  p2.Add("M2,2 L2,3 L10,3 L 10,2 L 2,2");
	  
	  std::ostringstream os;
	  os << NFmiGpcTools::Xor(p1,p2);
	  if(os.str() != "M5,3 L10,3 L10,2 L5,2 L5,3 L2,3 L2,2 L5,2 L5,0 L0,0 L0,5 L5,5 L5,3")
		TEST_FAILED("XOR failed for intersecting rectangles: "+os.str());
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
	  TEST(Or);
	  TEST(Minus);
	  TEST(Xor);
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
