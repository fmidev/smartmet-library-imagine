// ======================================================================
//
// Hershey Font Container Class
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#include "NFmiFontHershey.h"
#include "NFmiFontHersheyCache.h"
#include "NFmiHersheyData.h"
#include "NFmiEsriBox.h"
#include "NFmiText.h"

// This class will serve the actual font data

static NFmiFontHersheyCache itsCache;
using namespace std;
// ----------------------------------------------------------------------
// Fill text with given color and rule
// ----------------------------------------------------------------------

void NFmiFontHershey::Fill(const NFmiText & theText,
			    NFmiImage & theImage,
			    NFmiColorTools::Color theColor,
			    NFmiColorTools::NFmiBlendRule theRule) const
{

  // Filling Hershey fonts is meaningless, the font is a stroke font,
  // not an outline font.

  return;

}

// ----------------------------------------------------------------------
// Fill text with given pattern, rule and alpha factor
// ----------------------------------------------------------------------

void NFmiFontHershey::Fill(const NFmiText & theText,
			    NFmiImage & theImage,
			    const NFmiImage & thePattern,
			    NFmiColorTools::NFmiBlendRule theRule,
			    float theAlphaFactor) const
{
  // Filling Hershey fonts is meaningless, the font is a stroke font,
  // not an outline font.

  return;
}


// ----------------------------------------------------------------------
// Stroke text with given color and rule
// ----------------------------------------------------------------------

void NFmiFontHershey::Stroke(const NFmiText & theText,
			      NFmiImage & theImage,
			      NFmiColorTools::Color theColor,
			      NFmiColorTools::NFmiBlendRule theRule) const
{
  if(theColor == NFmiColorTools::NoColor)
    return;

  Path(theText).Stroke(theImage,theColor,theRule);
}

// ----------------------------------------------------------------------
// Return text as a path
// ----------------------------------------------------------------------

NFmiPath NFmiFontHershey::Path(const NFmiText & theText) const
{
  NFmiPath outpath;

  // Quick exit for empty text

  if(theText.Text().empty())
    return outpath;

  // Obtain the data

  const NFmiHersheyData & font = itsCache.Data(itsName);

  // Return an empty path if the font is not valid (is empty)

  if(!font.IsValid())
    return outpath;
  
  // Render the data, first without any transformations

  const string & text = theText.Text();

  float x = 0.0;
  float y = 0.0;

  int miny = font.MinY();
  int maxy = font.MaxY();

  for(unsigned int pos=0; pos<text.size(); pos++)
    {

      // The character to be drawn

      unsigned char ch = text[pos];

      // Character dimensions

      int minx = font.MinX(ch);
      int maxx = font.MaxX(ch);

      // A character always begins with a moveto
      
      bool moveto = true;

      // Do all the movements for the character
      // Note that the first coordinate is not a genuine movement,
      // it is only used to get a leading for the character. It
      // has an effect on the bounding box of the character, but
      // could have been deleted from the actual data used for
      // rendering. Instead, we just start the loop from 1.

      for(int i=1; i<font.Size(ch); i++)
	{
	  if(font.IsSkip(ch,i))
	    moveto = true;
	  else
	    {
	      int xx = font.X(ch,i);
	      int yy = font.Y(ch,i);
	      if(moveto)
		outpath.MoveTo(x + xx - minx, y + yy - miny);
	      else
		outpath.LineTo(x + xx - minx, y + yy - miny);
	      
	      moveto = false;
	    }
	}
      
      // Advance by the width of the character

      x += maxx-minx;

      // An extra advance to get better readability

      x += 1;
      
    }

  // And calculate the affine transformation for the path
  //
  // 1. We wish the height to be theText.Size();
  // 2. We wish the corner theText.Alignment() to be at theText.X(),theText.Y()
  // 3. We wish the text to be at angle theText.Angle() (decimal)

  // This aligns the text to the desired corner

  outpath.Align(theText.Alignment());

  // Now we establish the bounding box

  NFmiEsriBox box = outpath.BoundingBox();

  // And derive the remaining transformation

  NFmiAffine trans;
  float scale = theText.Size()/(maxy-miny);
  trans.Translate(theText.X(),theText.Y());
  trans.Scale(scale);
  trans.Rotate(theText.Angle());

  // And apply it

  outpath.Transform(trans);

  // The possible optimization would be to use bbox to calculate
  // the required translation directly, instead of using Align.

  return outpath;

}

// ----------------------------------------------------------------------

