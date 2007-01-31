// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiDataHints
 */
// ======================================================================

#include "NFmiDataHints.h"
#include "NFmiGlobals.h"
#include "tframe.h"
#include <iostream>
#include <cmath>

using namespace std;

//! Protection against conflicts with global functions
namespace NFmiDataHintsTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiDataHintsTest::rectangles
   */
  // ----------------------------------------------------------------------

  void rectangles()
  {
	using namespace Imagine;
	typedef NFmiDataHints::return_type rectangles;

	NFmiDataMatrix<float> data(100,100);
	for(unsigned int j=0; j<data.NY(); j++)
	  for(unsigned int i=0; i<data.NX(); i++)
		data[i][j] = i+j;

	{
	  NFmiDataHints hints(data,10);

	  rectangles r = hints.rectangles(-99,-98);
	  if(r.size() != 0)
		TEST_FAILED("i+j interval -99..-98 should be empty");

	  r = hints.rectangles(998,999);
	  if(r.size() != 0)
		TEST_FAILED("i+j interval 998..999 should be empty");

	  r = hints.rectangles(0,5);
	  if(r.size() != 1)
		TEST_FAILED("Failed to extract i+j interval 0...5");
	  if(r.front().x1 != 0)
		TEST_FAILED("i+j interval 0...5 1st x1<>0");
	  if(r.front().y1 != 0)
		TEST_FAILED("i+j interval 0...5 1st y1<>0");
	  if(r.front().x2 != 6)
		TEST_FAILED("i+j interval 0...5 1st x2<>6");
	  if(r.front().y2 != 6)
		TEST_FAILED("i+j interval 0...5 1st y2<>6");
	  if(r.front().minimum != 0)
		TEST_FAILED("i+j interval 0...5 1st minimum<>0");
	  if(r.front().maximum != 12)
		TEST_FAILED("i+j interval 0...5 1st maximum<>12");

	  r = hints.rectangles(0,10);
	  if(r.size() != 2)
		TEST_FAILED("Failed to extract i+j interval 0...10");
	  if(r.front().x1 != 0)
		TEST_FAILED("i+j interval 0...10 1st x1<>0");
	  if(r.front().y1 != 6)
		TEST_FAILED("i+j interval 0...10 1st y1<>6");
	  if(r.front().x2 != 6)
		TEST_FAILED("i+j interval 0...10 1st x2<>6");
	  if(r.front().y2 != 12)
		TEST_FAILED("i+j interval 0...10 1st y2<>12");
	  if(r.front().minimum != 6)
		TEST_FAILED("i+j interval 0...10 1st minimum<>6");
	  if(r.front().maximum != 18)
		TEST_FAILED("i+j interval 0...10 1st maximum<>18");

	  if(r.back().x1 != 0)
		TEST_FAILED("i+j interval 0...10 1st x1<>0");
	  if(r.back().y1 != 0)
		TEST_FAILED("i+j interval 0...10 1st y1<>0");
	  if(r.back().x2 != 12)
		TEST_FAILED("i+j interval 0...10 1st x2<>12");
	  if(r.back().y2 != 6)
		TEST_FAILED("i+j interval 0...10 1st y2<>6");
	  if(r.back().minimum != 0)
		TEST_FAILED("i+j interval 0...10 1st minimum<>0");
	  if(r.back().maximum != 18)
		TEST_FAILED("i+j interval 0...10 1st maximum<>18");

	  r = hints.rectangles(100,kFloatMissing);
	  if(r.size() != 23)
		TEST_FAILED("i+j interval 100...inf failed");
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
	  TEST(rectangles);
	}
  };
 
} // namespace NFmiDataHintsTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl
	   << "NFmiDataHints tester" << endl
	   << "====================" << endl;
  NFmiDataHintsTest::tests t;
  return t.run();
}

// ======================================================================
