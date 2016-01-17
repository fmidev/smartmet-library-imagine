// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace NFmiBezierTools
 */
// ======================================================================

#include "NFmiBezierTools.h"
#include "NFmiCounter.h"
#include "NFmiPath.h"
#include "NFmiStringTools.h"
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

  if (NFmiBezierTools::IsClosed(path)) TEST_FAILED("An empty path is not closed");

  path.MoveTo(1, 0);
  if (NFmiBezierTools::IsClosed(path)) TEST_FAILED("A single moveto cannot be closed");

  path.LineTo(1, 1);
  if (NFmiBezierTools::IsClosed(path)) TEST_FAILED("moveto 1,0 + lineto 1,1 is not closed");

  path.LineTo(0, 1);
  if (NFmiBezierTools::IsClosed(path))
    TEST_FAILED("moveto 1,0 + lineto 1,1 + lineto 0,1 is not closed");

  path.LineTo(1, 0);
  if (!NFmiBezierTools::IsClosed(path))
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
  using namespace Imagine;
  using NFmiStringTools::Convert;

  NFmiPath path1;
  path1.MoveTo(1, 0);  //
  path1.LineTo(1, 1);  // +--+
  path1.LineTo(0, 1);  //  \ |
  path1.LineTo(1, 0);  //   \+

  NFmiPath path2;
  path2.MoveTo(0, 0);
  path2.LineTo(0, 1);  // +--+

  NFmiBezierTools::NFmiPaths paths;
  paths.push_back(path1);
  paths.push_back(path2);

  NFmiCounter<NFmiPoint> counts = NFmiBezierTools::VertexCounts(paths);

  int count;
  if ((count = counts.Count(NFmiPoint(1, 0))) != 1)
    TEST_FAILED("Count of 1,0 is 1, not " + Convert(count));
  if ((count = counts.Count(NFmiPoint(1, 1))) != 1)
    TEST_FAILED("Count of 1,1 is 1, not " + Convert(count));
  if ((count = counts.Count(NFmiPoint(0, 1))) != 2)
    TEST_FAILED("Count of 0,1 is 2, not " + Convert(count));
  if ((count = counts.Count(NFmiPoint(0, 0))) != 1)
    TEST_FAILED("Count of 0,0 is 1, not " + Convert(count));

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test NFmiBezierTools::bezierlength
 */
// ----------------------------------------------------------------------

void bezierlength()
{
  using namespace Imagine;

  // simple straight line
  {
    NFmiPath path;
    path.MoveTo(0, 0);
    path.CubicTo(1, 0);
    path.CubicTo(2, 0);
    path.CubicTo(3, 0);

    double len = NFmiBezierTools::BezierLength(path, 0.1);
    if (len != 3) TEST_FAILED("Failed to calculate trivial length exactly");
  }

  // a simple parabola type curve

  {
    NFmiPath path;
    path.MoveTo(0, 0);
    path.CubicTo(1, 1);
    path.CubicTo(2, 1);
    path.CubicTo(3, 0);

    const double accurate = 3.44338072408896;
    double len;

    len = NFmiBezierTools::BezierLength(path, 0.1);
    if (abs(len - accurate) > 0.1) TEST_FAILED("Failed to calculate length to accuracy 0.1");

    len = NFmiBezierTools::BezierLength(path, 0.01);
    if (abs(len - accurate) > 0.01) TEST_FAILED("Failed to calculate length to accuracy 0.01");

    len = NFmiBezierTools::BezierLength(path, 0.001);
    if (abs(len - accurate) > 0.001) TEST_FAILED("Failed to calculate length to accuracy 0.001");

    len = NFmiBezierTools::BezierLength(path, 0.0001);
    if (abs(len - accurate) > 0.0001) TEST_FAILED("Failed to calculate length to accuracy 0.0001");
  }

  // up and down curve

  {
    NFmiPath path;
    path.MoveTo(0, 0);
    path.CubicTo(1, 1);
    path.CubicTo(2, -1);
    path.CubicTo(3, 0);

    const double accurate = 3.27480396;
    double len;

    len = NFmiBezierTools::BezierLength(path, 0.1);
    if (abs(len - accurate) > 0.1) TEST_FAILED("Failed to calculate length to accuracy 0.1");

    len = NFmiBezierTools::BezierLength(path, 0.01);
    if (abs(len - accurate) > 0.01) TEST_FAILED("Failed to calculate length to accuracy 0.01");

    len = NFmiBezierTools::BezierLength(path, 0.001);
    if (abs(len - accurate) > 0.001) TEST_FAILED("Failed to calculate length to accuracy 0.001");

    len = NFmiBezierTools::BezierLength(path, 0.0001);
    if (abs(len - accurate) > 0.0001) TEST_FAILED("Failed to calculate length to accuracy 0.0001");
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
  virtual const char* error_message_prefix() const { return "\n\t"; }
  void test(void)
  {
    TEST(isclosed);
    TEST(vertexcounts);
    TEST(bezierlength);
  }
};

}  // namespace NFmiBezierToolsTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "NFmiBezierTools tester" << endl << "======================" << endl;
  NFmiBezierToolsTest::tests t;
  return t.run();
}

// ======================================================================
