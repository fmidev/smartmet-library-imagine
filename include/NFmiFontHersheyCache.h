// ======================================================================
//
// Hershey Font Cache Class
//
// The purpose of the class is to return a single Hershey font data
// for the user. The font is to be loaded if necessary. The class
// may choose to delete some fonts if the total size of the cached
// fonts exceeds some limit. The user has no control of the cache.
//
// The class is intended to be used by NFmiFontHershey only,
// but is not protected in case someone wishes to get direct
// access to the data.
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ----------------------------------------------------------------------

#ifndef _NFMIFONTHERSHEYCACHE_H
#define _NFMIFONTHERSHEYCACHE_H

#include <string>
#include <map>

#include "NFmiHersheyData.h"

class NFmiFontHersheyCache
{
public:
  NFmiFontHersheyCache(void) {}
  ~NFmiFontHersheyCache(void) {}
  
  const NFmiHersheyData & Data(const std::string & theName);
  
private:
  
  typedef std::map<std::string,NFmiHersheyData> HersheyCache;
  
  HersheyCache itsData;
  
};

#endif // _NFMIFONTHERSHEYCACHE_H

// ======================================================================
