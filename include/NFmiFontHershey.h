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

#ifndef IMAGINE_NFMIFONTHERSHEY_H
#define IMAGINE_NFMIFONTHERSHEY_H

#include "NFmiFont.h"

namespace Imagine
{

  class _FMI_DLL NFmiFontHershey : public NFmiFont
  {
  public:
	
	// Constructors, destructors
	
	NFmiFontHershey(const std::string & theName)
	  : NFmiFont(theName)
	{
	  itsType = kFmiFontHershey;
	}
	
	virtual ~NFmiFontHershey(void) {}
	
	// Copy, assignment
	
	NFmiFontHershey(const NFmiFontHershey & theFont)
	  : NFmiFont(theFont)
	{ }
	
	NFmiFontHershey & operator=(const NFmiFontHershey & theFont)
	{
	  NFmiFont::operator=(theFont);
	  return *this;
	}
	
	// Cloning
	
	NFmiFont * Clone(void) const
	{
	  return new NFmiFontHershey(*this);
	}
	
	// Rendering
	
	void Fill(const NFmiText & theText,
			  NFmiImage & theImage,
			  NFmiColorTools::Color theColor,
			  NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;
	
	void Fill(const NFmiText & theText,
			  NFmiImage & theImage,
			  const NFmiImage & thePattern,
			  NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy,
			  float theAlphaFactor=1.0) const;
	
	void Stroke(const NFmiText & theText,
				NFmiImage & theImage,
				NFmiColorTools::Color theColor,
				NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;
	
	// Return text as a path
	
	NFmiPath Path(const NFmiText & theText) const;
	
	// Data-part
	
  private:
	
  };

} // namespace Imagine
  
#endif // IMAGINE_NFMIFONRHERSHEY_H
  
// ======================================================================
