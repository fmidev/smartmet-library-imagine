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
#define EXPECT(a, b) \
  if (a.SVG() != b)  \
    TEST_FAILED("Expected '" + string(b) + "', got '" + a.SVG() + "'");

// ----------------------------------------------------------------------
/*!
 * \brief Test NFmiApproximateBezierFit::Fit
 */
// ----------------------------------------------------------------------

void Fit()
{
  using namespace Imagine;

  // A difficult case before the Bezier length test was added
  // to the Graphics Gems algorithm:
  {
    NFmiPath p, s;
    p.MoveTo(60.2, 95.8);
    p.LineTo(3.9, 63.7);
    p.LineTo(12.2, 59.5);  // 1
    p.LineTo(15.9, 56.9);  // ->
    p.LineTo(27.8, 50.2);  // 2
    p.LineTo(39.8, 43.5);
    p.LineTo(49.3, 53.4);
    p.LineTo(60.2, 95.8);
    s = NFmiApproximateBezierFit::Fit(p, 1.0);
    EXPECT(s,
           "M60.2,95.8 C39.133,100.58 21.13,76.73 3.9,63.7 1.427,61.83 9.502,61.029 12.2,59.5 "
           "13.511,58.757 14.605,57.672 15.9,56.9 19.81,54.569 23.829,52.426 27.8,50.2 "
           "31.796,47.96 35.269,42.826 39.8,43.5 44.324,44.173 47.638,49.139 49.3,53.4 "
           "54.603,66.995 74.431,92.571 60.2,95.8");
  }

  {
    NFmiPath p, s;
    p.MoveTo(230.878189, 135.041138);
    p.LineTo(227.785355, 137.013580);
    p.LineTo(226.464218, 140.816055);
    p.LineTo(226.098022, 141.471664);
    p.LineTo(225.875824, 141.540802);
    p.LineTo(225.806458, 141.569244);
    p.LineTo(225.650833, 141.626785);
    p.LineTo(220.831345, 142.943329);
    p.LineTo(219.354843, 143.687881);
    p.LineTo(216.888992, 145.444397);
    p.LineTo(215.845871, 147.902191);
    p.LineTo(214.917480, 149.909866);
    p.LineTo(214.353790, 152.886902);
    p.LineTo(213.895218, 154.332718);
    p.LineTo(213.738541, 155.165314);
    p.LineTo(212.949051, 160.717590);
    p.LineTo(212.941086, 160.763260);
    p.LineTo(212.937546, 160.797470);
    p.LineTo(212.903229, 161.092224);
    p.LineTo(212.032822, 161.630829);
    p.LineTo(208.817719, 163.121628);
    p.LineTo(206.451614, 166.623535);
    p.LineTo(205.617096, 161.595047);
    p.LineTo(205.584091, 160.763260);
    p.LineTo(205.606445, 159.920853);
    p.LineTo(205.410156, 155.370773);
    p.LineTo(205.434219, 154.332718);
    p.LineTo(205.432297, 153.316742);
    p.LineTo(205.752792, 148.598740);
    p.LineTo(205.751282, 147.902191);
    p.LineTo(205.484451, 146.938187);
    p.LineTo(204.591675, 143.325516);
    p.LineTo(204.034851, 141.471664);
    p.LineTo(204.586670, 139.612823);
    p.LineTo(206.451614, 136.416595);
    p.LineTo(207.009415, 135.597122);
    p.LineTo(207.301056, 135.041138);
    p.LineTo(208.888962, 132.611755);
    p.LineTo(209.474136, 131.623245);
    p.LineTo(212.286087, 128.610596);
    p.LineTo(212.604126, 128.312469);
    p.LineTo(212.903229, 127.959412);
    p.LineTo(214.719482, 126.800285);
    p.LineTo(216.497543, 125.762650);
    p.LineTo(219.354843, 123.283577);
    p.LineTo(221.595032, 124.412941);
    p.LineTo(222.881836, 125.095154);
    p.LineTo(225.806458, 126.421570);
    p.LineTo(226.857635, 127.562851);
    p.LineTo(229.247894, 128.610596);
    p.LineTo(229.897461, 130.963486);
    p.LineTo(230.405579, 133.194702);

    s = NFmiApproximateBezierFit::Fit(p, 5);
    EXPECT(s,
           "M230.878,135.041 C226.505,137.83 206.601,166.695 206.452,166.624 191.489,159.487 "
           "222.593,98.889 230.406,133.195");
  }

  {
    NFmiPath p, s;

    p.MoveTo(0, 0);
    p.LineTo(0, 3);
    p.LineTo(3, 3);
    s = NFmiApproximateBezierFit::Fit(p, 1.5);
    EXPECT(s, "M0,0 C0,4 -1,3 3,3");

    p.LineTo(3, 0);
    s = NFmiApproximateBezierFit::Fit(p, 1.5);
    EXPECT(s, "M0,0 C0,4.5 3,4.5 3,0");

    p.LineTo(0, 0);
    s = NFmiApproximateBezierFit::Fit(p, 1.5);
    EXPECT(s, "M0,0 C-1,1 2,4 3,3 4,2 1,-1 0,0");
  }

  {
    NFmiPath p, s;
    p.MoveTo(85.381981, 327.957031);
    p.LineTo(84.955460, 326.876068);
    p.LineTo(83.870964, 325.901672);
    p.LineTo(81.028053, 325.123413);
    p.LineTo(80.431259, 324.954956);
    p.LineTo(77.419357, 324.149628);
    p.LineTo(74.917046, 325.462891);
    p.LineTo(71.074730, 327.957031);
    p.LineTo(71.086594, 328.075500);
    p.LineTo(70.967743, 328.559448);
    p.LineTo(70.379112, 333.800873);
    p.LineTo(70.150650, 334.387573);
    p.LineTo(70.190468, 335.162323);
    p.LineTo(70.967743, 335.926910);
    p.LineTo(74.003410, 337.792358);
    p.LineTo(74.830643, 338.237854);
    p.LineTo(77.419357, 339.685333);
    p.LineTo(78.697594, 339.544037);
    p.LineTo(83.114151, 340.063751);
    p.LineTo(83.870964, 339.922058);
    p.LineTo(84.904381, 335.417603);
    p.LineTo(84.909729, 334.387573);
    p.LineTo(85.007240, 333.255005);
    p.LineTo(85.257103, 329.338654);
    p.LineTo(85.381981, 327.957031);
    s = NFmiApproximateBezierFit::Fit(p, 5);
    EXPECT(s,
           "M85.382,327.957 C84.495,320.718 65.038,325.456 70.19,335.162 76.155,346.399 "
           "86.64,338.231 85.382,327.957");
  }

  {
    NFmiPath p, s;
    p.MoveTo(86.442192, 192.915909);
    p.LineTo(84.604469, 193.647018);
    p.LineTo(83.870964, 193.921692);
    p.LineTo(82.977676, 193.806274);
    p.LineTo(80.969513, 192.915909);
    p.LineTo(82.224884, 191.275208);
    p.LineTo(83.870964, 189.746033);
    p.LineTo(85.917007, 190.876556);
    p.LineTo(86.442192, 192.915909);
    s = NFmiApproximateBezierFit::Fit(p, 5);
    EXPECT(s, "M86.442,192.916 C83.251,199.652 89.095,187.316 86.442,192.916");
  }

  // A self touching curve:
  {
    NFmiPath p, s;
    p.MoveTo(-100, 0);
    p.LineTo(-50, -50);
    p.LineTo(0, 0);
    p.LineTo(50, -50);
    p.LineTo(100, 0);
    p.LineTo(50, 50);
    p.LineTo(0, 0);
    p.LineTo(-50, 50);
    p.LineTo(-100, 0);

    s = NFmiApproximateBezierFit::Fit(p, 1);
    EXPECT(s,
           "M-100,0 C-116.667,16.667 -73.57,-50 -50,-50 -26.43,-50 -23.57,0 0,0 23.57,0 26.43,-50 "
           "50,-50 73.57,-50 100,-23.57 100,0 100,23.57 73.57,50 50,50 26.43,50 23.57,0 0,0 "
           "-23.57,0 -26.43,50 -50,50 -73.57,50 -83.333,-16.667 -100,0");
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
  void test(void) { TEST(Fit); }
};

}  // namespace NFmiApproximateBezierFitTest

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
