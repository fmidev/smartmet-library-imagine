// ======================================================================
//
// Hershey Font Cache Class
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ----------------------------------------------------------------------
#ifdef WIN32
#pragma warning(disable : 4786) // poistaa n kpl VC++ kääntäjän varoitusta
#endif

#include "NFmiFileSystem.h"
#include "NFmiSettings.h"
#include "NFmiFontHersheyCache.h"
#include "NFmiFontGlobals.h"

using namespace std;
// ----------------------------------------------------------------------
// Given a Hershey font name - return respective NFmiHersheyData
// ----------------------------------------------------------------------

const NFmiHersheyData & NFmiFontHersheyCache::Data(const string & theName)
{
  const string hershey_path = NFmiSettings::instance().value("imagine::hershey_path",".");

  // First try to find the font from the cache
  
  HersheyCache::const_iterator find_iter;
  
  find_iter = itsData.find(theName);
  
  if(find_iter != itsData.end())
    return find_iter->second;
  
  // Then, try loading the file. First current directory, then default path
  
  bool ok = true;	// for now everything seems ok
  
  string filename = theName + kFmiSuffixHershey;
  if(!FileExists(filename))
    {
      filename = hershey_path + filename;
      if(!FileExists(filename))
		ok = false;
    }
  
  // If we found a correct filename - see if it is readable
  
  if(ok) ok = FileReadable(filename);
  
  // Now, if we found an invalid font name, we would return
  // a dummy font name. However, constructing a dummy is
  // exactly equal to constructing the actual font, hence
  // there is no distinction here. Simply the fact that
  // opening the file fails is enough to make the font
  // created an empty one.
  
  // insert returns pair<iterator,flag>, from which we return iterator->second
  
  return itsData.insert(std::make_pair(theName,NFmiHersheyData(filename))).first->second;
  
}

// ======================================================================
