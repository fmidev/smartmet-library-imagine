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

  // const string font = "/data/share/fonts/ttf/ArialNarrow.ttf";
  const string font = "/usr/lib/X11/fonts/misc/6x13.pcf.gz";
  NFmiFace face(NFmiFreeType::Instance().Face(font,6,13));

  face.Draw(image,0,0,"NorthWest",kFmiAlignNorthWest);
  face.Draw(image,width/2,0,"North",kFmiAlignNorth);
  face.Draw(image,width,0,"NorthEast",kFmiAlignNorthEast);

  face.Draw(image,0,height/2,"West",kFmiAlignWest);
  face.Draw(image,width/2,height/2,"Center",kFmiAlignCenter);
  face.Draw(image,width,height/2,"East",kFmiAlignEast);

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
