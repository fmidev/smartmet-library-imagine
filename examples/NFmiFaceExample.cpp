// ======================================================================
/*!
 * \file
 * \brief Demonstration of NFmiFace usage
 *
 * The result is written to /tmp/NFmiFace.png and is displayed with display
 */
// ======================================================================

#include "NFmiFace.h"
#include "NFmiFileSystem.h"
#include "NFmiFreeType.h"
#include "NFmiImage.h"
#include "NFmiPath.h"

#include <iostream>

using namespace std;
using namespace Imagine;

void demo()
{
  const string tmpfile = "NFmiFaceExample.png";
  const int width = 500;
  const int height = 500;

  NFmiColorTools::Color white = NFmiColorTools::MakeColor(255,255,255);
  NFmiImage image(width,height,white);

  NFmiPath path;
  path.MoveTo(0,0);
  path.LineTo(width,height);
  path.MoveTo(width,0);
  path.LineTo(0,height);
  path.Stroke(image,NFmiColorTools::MakeColor(0,255,0));

#if 0
  const string font = "ttf/ArialNarrow.ttf";
  NFmiFace face(NFmiFreeType::Instance().Face(font,0,20));
#else  
  const string font = "misc/6x13.pcf.gz";
  NFmiFace face(NFmiFreeType::Instance().Face(font,6,13));
#endif

  face.Draw(image,0,0,"NorthWest",kFmiAlignNorthWest);
  face.Draw(image,width/2,0,"North",kFmiAlignNorth);
  face.Draw(image,width,0,"NorthEast",kFmiAlignNorthEast);

  face.Background(true);
  // face.BackgroundRule(NFmiColorTools::kFmiColorOver);
  face.Draw(image,0,height/2,"West",kFmiAlignWest);
  face.Draw(image,width/2,height/2,"Center",kFmiAlignCenter);
  face.Draw(image,width,height/2,"East",kFmiAlignEast);

  face.BackgroundColor(NFmiColorTools::MakeColor(255,0,0));
  // face.BackgroundRule(NFmiColorTools::kFmiColorAtop);
  face.Draw(image,0,height,"SouthWest",kFmiAlignSouthWest);
  face.Draw(image,width/2,height,"South",kFmiAlignSouth);
  face.Draw(image,width,height,"SouthEast",kFmiAlignSouthEast);

  image.WritePng(tmpfile);
  string cmd = "display "+tmpfile;
  system(cmd.c_str());
  NFmiFileSystem::RemoveFile(tmpfile);
}

int main()
{
  try
	{
	  demo();
	  return 0;
	}
  catch(exception & e)
	{
	  cerr << "Caught exception:" << endl << e.what() << endl;
	  return 1;
	}
}
