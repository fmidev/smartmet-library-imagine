// ======================================================================
//
// Hershey Data Container Class
//
// History:
//
// 15.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#include "NFmiHersheyData.h"

#include <fstream>
#include <cassert>

using namespace std;

namespace Imagine
{

  // ----------------------------------------------------------------------
  // Return number of coordinates in given character
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::Size(unsigned char theChar) const
  {
	// There is nothing below the first character (space)
	
	if(theChar < kFmiFirstHersheyChar)
	  return 0;
	
	// There is nothing above the last character..
	
	if(theChar > itsData.size() + kFmiFirstHersheyChar)
	  return 0;
	
	// Return the size of the char
	
	return itsData[theChar-kFmiFirstHersheyChar].size()/2;
	
  }
  
  // ----------------------------------------------------------------------
  // Return true if given step of character is a skip (moveto)
  // ----------------------------------------------------------------------
  
  bool NFmiHersheyData::IsSkip(unsigned char theChar, unsigned int i) const
  {
	assert(2*i<itsData[theChar-kFmiFirstHersheyChar].size());
	return( itsData[theChar-kFmiFirstHersheyChar][2*i] == ' ' );
  }
  
  // ----------------------------------------------------------------------
  // Return the X-coordinate of the character at the given step
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::X(unsigned char theChar, unsigned int i) const
  {
	assert(2*i<itsData[theChar-kFmiFirstHersheyChar].size());
	return( itsData[theChar-kFmiFirstHersheyChar][2*i] - kFmiHersheyZero );
  }
  
  // ----------------------------------------------------------------------
  // Return the Y-coordinate of the character at the given step
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::Y(unsigned char theChar, unsigned int i) const
  {
	assert(2*i+1<itsData[theChar-kFmiFirstHersheyChar].size());
	return( itsData[theChar-kFmiFirstHersheyChar][2*i+1] - kFmiHersheyZero );
  }
  
  // ----------------------------------------------------------------------
  // Return character specific minimum X coordinate
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::MinX(unsigned char theChar) const
  {
	// There is nothing below the first character
	// There is nothing above the last character..
	
	if(theChar < kFmiFirstHersheyChar)				return 0;
	if(theChar > itsCharactersMinX.size() + kFmiFirstHersheyChar)	return 0;
	
	// Return the desired coordinate
	
	return itsCharactersMinX[theChar-kFmiFirstHersheyChar] - kFmiHersheyZero;
	
  }
  
  // ----------------------------------------------------------------------
  // Return character specific maximum X coordinate
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::MaxX(unsigned char theChar) const
  {
	// There is nothing below the first character
	// There is nothing above the last character..
	
	if(theChar < kFmiFirstHersheyChar)			return 0;
	if(theChar > itsCharactersMaxX.size() + kFmiFirstHersheyChar)	return 0;
	
	// Return the desired coordinate
	
	return itsCharactersMaxX[theChar-kFmiFirstHersheyChar] - kFmiHersheyZero;
  }
  
  // ----------------------------------------------------------------------
  // Return character specific minimum Y coordinate
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::MinY(unsigned char theChar) const
  {
	// There is nothing below the first character
	// There is nothing above the last character..
	
	if(theChar < kFmiFirstHersheyChar)			return 0;
	if(theChar > itsCharactersMinY.size() + kFmiFirstHersheyChar)	return 0;
	
	// Return the desired coordinate
	
	return itsCharactersMinY[theChar-kFmiFirstHersheyChar] - kFmiHersheyZero;
  }
  
  // ----------------------------------------------------------------------
  // Return character specific maximum Y coordinate
  // ----------------------------------------------------------------------
  
  int NFmiHersheyData::MaxY(unsigned char theChar) const
  {
	// There is nothing below the first character
	// There is nothing above the last character..
	
	if(theChar < kFmiFirstHersheyChar)			return 0;
	if(theChar > itsCharactersMaxY.size() + kFmiFirstHersheyChar)	return 0;
	
	// Return the desired coordinate
	
	return itsCharactersMaxY[theChar-kFmiFirstHersheyChar] - kFmiHersheyZero;
  }
  
  // ----------------------------------------------------------------------
  // Constructor based on given filename
  // ----------------------------------------------------------------------
  
  NFmiHersheyData::NFmiHersheyData(const string & theFileName)
	: itIsValid(false)
  {
	
	// Try opening the file
	
	ifstream infile(theFileName.c_str());
	if(!infile) return;
	
	// Read all of it into a string and close the file
	
	string hershey;
	
	const int bufsize = 1024;	// reader buffer size
	char buffer[bufsize];
	
	while(infile.good())
	  {
		// Mika: Unix C++ kirjastosta puuttuu readsome
#if 0
		int n = infile.readsome(buffer,bufsize);
		hershey.append(buffer,n);
#else
		infile.getline(buffer,bufsize);
		hershey += buffer;
		hershey += "\n";
#endif
	  }
	
	infile.close();
	
	// Parse the contents of the string into the member objects
	// Each line gets added as is with push_back into itsData
	
	unsigned int pos1 = 0;
	
	do
	  {
		// Find position last character to be inserted
		
		unsigned int pos2 = hershey.find('\n',pos1);
		
		if(pos2==string::npos)
		  pos2 = hershey.size()-1;
		else
		  pos2--;
		
		// Add the new character
		
		itsData.push_back(hershey.substr(pos1,pos2-pos1+1));
		
		// Skip to next line
		
		pos1 = pos2 + 2;
		
		// Done when reached end of text
		
	  }
	while(pos1<hershey.size());
	
	// Next we calculate character and font bounding boxes
	
	if(itsData.empty())
	  {
		itIsValid = false;
		return;
	  }
	
	itsCharactersMinX.reserve(itsData.size());
	itsCharactersMaxX.reserve(itsData.size());
	itsCharactersMinY.reserve(itsData.size());
	itsCharactersMaxY.reserve(itsData.size());
	
	itsFontMinX = 255;
	itsFontMinY = 255;
	itsFontMaxX = 0;
	itsFontMaxY = 0;
	
	for(unsigned int i=0; i<itsData.size(); i++)
	  {
		string & data = itsData[i];
		
		unsigned char minx=255;
		unsigned char miny=255;
		unsigned char maxx=0;
		unsigned char maxy=0;
		
		// Find chacracter bounding box
		
		for(unsigned int j=0; j<data.size(); j+=2)
		  {
			if(data[j]!=' ')	// if not " R" aka skip
			  {
				minx = (minx < data[j] ? minx : data[j]);
				maxx = (maxx > data[j] ? maxx : data[j]);
				miny = (miny < data[j+1] ? miny : data[j+1]);
				maxy = (maxy > data[j+1] ? maxy : data[j+1]);
			  }
		  }
		
		// Update individual character bounding box
		
		itsCharactersMinX += minx;
		itsCharactersMaxX += maxx;
		itsCharactersMinY += miny;
		itsCharactersMaxY += maxy;
		
		// Update font bounding box (largest possible character)
		
		if(i==0)
		  {
			itsFontMinX = minx;
			itsFontMaxX = maxx;
			itsFontMinY = miny;
			itsFontMaxY = maxy;
		  }
		else
		  {
			itsFontMinX = (itsFontMinX < minx ? itsFontMinX : minx);
			itsFontMaxX = (itsFontMaxX > maxx ? itsFontMaxX : maxx);
			itsFontMinY = (itsFontMinY < miny ? itsFontMinY : miny);
			itsFontMaxY = (itsFontMaxY > maxy ? itsFontMaxY : maxy);
		  }
	  }
	
	// Fix the space character to have a fixed width
	// We use the width of 'a' if it is defined, the last charcter if not
	
	int usechar;
	if('a' < itsData.size()+kFmiFirstHersheyChar)
	  usechar = 'a' - kFmiFirstHersheyChar;
	else
	  usechar = itsData.size()-1;
	
	itsCharactersMinX[0] = itsCharactersMinX[usechar];
	itsCharactersMaxX[0] = itsCharactersMaxX[usechar];
	itsCharactersMinY[0] = itsCharactersMinY[usechar];
	itsCharactersMaxY[0] = itsCharactersMaxY[usechar];
	
	itIsValid = true;
	
  }
  
} // namespace Imagine
  
// ======================================================================
  
