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

#include "NFmiFileSystem.h"
#include "NFmiSettings.h"
#include "NFmiFontHersheyCache.h"
#include "NFmiFontGlobals.h"
#include <stdexcept>

using namespace std;

namespace Imagine
{

  // ----------------------------------------------------------------------
  // Given a Hershey font name - return respective NFmiHersheyData
  // ----------------------------------------------------------------------
  
  const NFmiHersheyData & NFmiFontHersheyCache::Data(const string & theName)
  {
	const string hershey_path(NFmiSettings::Optional("imagine::hershey_path", string(".")));
	
	// First try to find the font from the cache
	
	HersheyCache::const_iterator find_iter;
	
	find_iter = itsData.find(theName);
	
	if(find_iter != itsData.end())
	  return find_iter->second;
	
	// Then, try loading the file. First current directory, then default path
	
	string filename = FileComplete(theName + kFmiSuffixHershey, hershey_path);
	
	// If we found a correct filename - see if it is readable
	
	if(!FileReadable(filename))
	  throw std::runtime_error("Hershey font " + filename + " is not readable");
	
	// Now, if we found an invalid font name, we would return
	// a dummy font name. However, constructing a dummy is
	// exactly equal to constructing the actual font, hence
	// there is no distinction here. Simply the fact that
	// opening the file fails is enough to make the font
	// created an empty one.
	
	// insert returns pair<iterator,flag>, from which we return iterator->second
	
	return itsData.insert(std::make_pair(theName,NFmiHersheyData(filename))).first->second;
	
  }

} // namespace Imagine
  
// ======================================================================
  
