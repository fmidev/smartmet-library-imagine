// ======================================================================
//
// Utilities for reading/ writing values from/to a character buffer
//
// ======================================================================

#ifndef IMAGINE_NFMIESRIBUFFER_H
#define IMAGINE_NFMIESRIBUFFER_H

#include <string>

namespace Imagine
{
  namespace NFmiEsriBuffer
  {
	bool IsCpuLittleEndian(void);
	
	int BigEndianInt(const std::string & theBuffer, int thePos);
	int LittleEndianInt(const std::string & theBuffer, int thePos);
	int BigEndianShort(const std::string & theBuffer, int thePos);
	int LittleEndianShort(const std::string & theBuffer, int thePos);
	double LittleEndianDouble(const std::string & theBuffer, int thePos);
	
	std::string BigEndianInt(int theValue);
	std::string LittleEndianInt(int theValue);
	std::string LittleEndianDouble(double theValue);
	std::string LittleEndianShort(int theValue);
	
	bool EsriRead(std::istream & is, std::string & theString, unsigned int theLength);
	
  }
} // namespace Imagine

#endif // IMAGINE_NFMIESRIBUFFER_H

// ======================================================================
