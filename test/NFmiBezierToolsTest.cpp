// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiBezierTools
 */
// ======================================================================

#include "NFmiBezierTools.h"
#include "NFmiPath.h"
#include "tframe.h"
#include <cmath>

using namespace std;

//! Protection against conflicts with global functions
namespace NFmiBezierToolsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiBezierTools::IsClosed
   */
  // ----------------------------------------------------------------------

  void isclosed()
  {
	using namespace Imagine;

	NFmiPath path;
	
	if(NFmiBezierTools::IsClosed(path))
	  TEST_FAILED("An empty path is not closed");

	path.MoveTo(1,0);
	if(NFmiBezierTools::IsClosed(path))
	  TEST_FAILED("A single moveto cannot be closed");

	path.LineTo(1,1);
	if(NFmiBezierTools::IsClosed(path))
	  TEST_FAILED("moveto 1,0 + lineto 1,1 is not closed");
	  
	path.LineTo(0,1);
	if(NFmiBezierTools::IsClosed(path))
	  TEST_FAILED("moveto 1,0 + lineto 1,1 + lineto 0,1 is not closed");

	path.LineTo(1,0);
	if(!NFmiBezierTools::IsClosed(path))
	  TEST_FAILED("moveto 1,0 + lineto 1,1 + lineto 0,1 + lineto 1,0 is closed");
	TEST_PASSED();

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiBezierTools::vertexcounts
   */
  // ----------------------------------------------------------------------

  void vertexcounts()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiBezierTools::bezierlength
   */
  // ----------------------------------------------------------------------

  void bezierlength()
  {
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
	  TEST(isclosed);
	  TEST(vertexcounts);
	  TEST(bezierlength);
	}
  };
 
} // namespace NFmiBezierToolsTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl
	   << "NFmiBezierTools tester" << endl
	   << "======================" << endl;
  NFmiBezierToolsTest::tests t;
  return t.run();
}

// ======================================================================
