// ======================================================================
/*!
 * \file NFmiDrawable.h
 * \brief Description of an abstract base class describing the interface
 *        of objects which can be drawn on a NFmiImage.
 *
 * \b History:
 *
 * \li 27.08.2001 Mika Heiskanen\par
 * Implemented.
 */
// ======================================================================
/*!
 * \class NFmiDrawable
 *
 * A drawable object has the ability to fill, pattern-fill or stroke
 * itself onto the given image. The task of filling may be delegated
 * to a NFmiFillMap by providing an Add() method.
 */
// ======================================================================

#ifndef _NFMIDRAWABLE_H
#define _NFMIDRAWABLE_H

// Note: Any drawable must invariably know how to handle colour,
//       hence this include is here instead of derived classes.

#include "NFmiGlobals.h"
#include "NFmiColorTools.h"

class NFmiImage;	// Derived class, hence cannot simply include header
class NFmiFillMap;	// Derived class, ...

//! Abstract base class for objects that can render themselves.

class NFmiDrawable
{
public:
  
  //! Constructor
  /*!
   * In the future this might take as input a CSS style sheet,
   * and the class would have a data member storing it.
   */
  
  NFmiDrawable(void) {}
  
  //! Add the drawable into a fill map.
  
  virtual void Add(NFmiFillMap & theMap) const {}
  
  //! Fill onto given image using various colour blending rules.
  
  virtual void Fill(NFmiImage & theImage,
					NFmiColorTools::Color theColor,
					NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;
  
  //! Fill onto given image with given pattern.
  
  virtual void Fill(NFmiImage & theImage,
					const NFmiImage & thePattern,
					NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy,
					float theAlphaFactor=1.0) const;
  
  //! Stroke onto given image using various colour blending rules.
  
  virtual void Stroke(NFmiImage & theImage,
					  NFmiColorTools::Color theColor,
					  NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;
  
private:
  
  // In the future here might be a style sheet data element.
  
};

#endif // _NFMIDRAWABLE_H

// ======================================================================

