// ======================================================================
//
// TrueType Font Container Class
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#include "NFmiFontTrueType.h"

namespace Imagine
{

  // ----------------------------------------------------------------------
  // Fill text with given color and rule
  // ----------------------------------------------------------------------
  
  void NFmiFontTrueType::Fill(const NFmiText & theText,
							  NFmiImage & theImage,
							  NFmiColorTools::Color theColor,
							  NFmiColorTools::NFmiBlendRule theRule) const
  {
	
  }
  
  // ----------------------------------------------------------------------
  // Fill text with given pattern, rule and alpha factor
  // ----------------------------------------------------------------------
  
  void NFmiFontTrueType::Fill(const NFmiText & theText,
							  NFmiImage & theImage,
							  const NFmiImage & thePattern,
							  NFmiColorTools::NFmiBlendRule theRule,
							  float theAlphaFactor) const
  {
  }
  
  
  // ----------------------------------------------------------------------
  // Stroke text with given color and rule
  // ----------------------------------------------------------------------
  
  void NFmiFontTrueType::Stroke(const NFmiText & theText,
								NFmiImage & theImage,
								NFmiColorTools::Color theColor,
								NFmiColorTools::NFmiBlendRule theRule) const
  {
  }
  
  // ----------------------------------------------------------------------
  // Return text as a path
  // ----------------------------------------------------------------------
  
  const NFmiPath NFmiFontTrueType::Path(const NFmiText & theText) const
  {
	NFmiPath outpath;
	return outpath;
  }

} // namespace Imagine
  
// ----------------------------------------------------------------------

  
