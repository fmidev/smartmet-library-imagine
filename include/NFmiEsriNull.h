// ======================================================================
//
// Esri Shapefile Techinical Description, page 5
//
//
// Position	Field	Value	Type	Number	Endian
//
// 0		Type	0	int	1	little
//
// ======================================================================

#ifndef IMAGINE_NFMIESRINULL_H
#define IMAGINE_NFMIESRINULL_H

#include "NFmiEsriElement.h"
#include "NFmiEsriBox.h"

namespace Imagine
{
  using namespace NFmiEsriBuffer;	// Conversion tools

  class _FMI_DLL NFmiEsriNull : public NFmiEsriElement
  {
  public:
	
	// Constructors, destructors
	
	~NFmiEsriNull(void) {}
	
	NFmiEsriNull(int theNumber=0)
	  : NFmiEsriElement(kFmiEsriNull,theNumber)
	{}
	
	// Updating bounding boxes
	
	void Update(NFmiEsriBoxZ & theBox) const
	{
	}

	void Project(const NFmiEsriProjector & theProjector)
	{
	}
	
	// Writing string buffer
	
	std::ostream & Write(std::ostream & os) const
	{
	  os << LittleEndianInt(kFmiEsriNull);
	  return os;
	}
	
	// Conversion to string buffer
	
	std::string ToString(void) const
	{
	  return LittleEndianInt(kFmiEsriNull);
	}
	
	// Calculating string buffer size
	
	int StringSize(void) const	{ return 4; }	// the type takes 4
	
  };

} // namespace Imagine
  
#endif // IMAGINE_NFMIESRINULL_H
  
// ======================================================================
  
