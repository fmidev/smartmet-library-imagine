// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiCounter
 */
// ======================================================================

#include "NFmiCounter.h"
#include "NFmiGlobals.h"
#include "tframe.h"
#include <cmath>

//! Protection against conflicts with global functions
namespace NFmiCounterTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test NFmiCounterTest::Count
   */
  // ----------------------------------------------------------------------

  void count()
  {
	Imagine::NFmiCounter<int> counter;

	for(int i=1; i<10; i++)
	  for(int j=0; j<i; j++)
		counter.Add(i);

	if(counter.Count(0) != 0)
	  TEST_FAILED("Count of 0 should be 0");
	if(counter.Count(1) != 1)
	  TEST_FAILED("Count of 1 should be 1");
	if(counter.Count(2) != 2)
	  TEST_FAILED("Count of 2 should be 2");
	if(counter.Count(3) != 3)
	  TEST_FAILED("Count of 3 should be 3");
	if(counter.Count(4) != 4)
	  TEST_FAILED("Count of 4 should be 4");
	if(counter.Count(5) != 5)
	  TEST_FAILED("Count of 5 should be 5");
	if(counter.Count(6) != 6)
	  TEST_FAILED("Count of 6 should be 6");
	if(counter.Count(7) != 7)
	  TEST_FAILED("Count of 7 should be 7");
	if(counter.Count(8) != 8)
	  TEST_FAILED("Count of 8 should be 8");
	if(counter.Count(9) != 9)
	  TEST_FAILED("Count of 9 should be 9");

	TEST_PASSED();
  }

  // ----------------------------------------------------------------------
  /*!
   * The actual test suite
   */
  // ----------------------------------------------------------------------

  class tests : public tframe::tests
  {
	virtual const char * error_message_prefix() const
	{
	  return "\n\t";
	}

	void test(void)
	{
	  TEST(count);
	}
  };
 
} // namespace NFmiCounterTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl
	   << "NFmiCounter tester" << endl
	   << "==================" << endl;
  NFmiCounterTest::tests t;
  return t.run();
}

// ======================================================================
