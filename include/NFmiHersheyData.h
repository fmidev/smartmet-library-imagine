// ======================================================================
//
// Hershey Font Data Container Class
//
// Example usage:
//
// if(font.isvalid)
// {
//   string tmp("Hello world!");
//   for(int i=0; i<tmp.size(); i++)
//   {
//     unsigned char ch = tmp[i];
//     for(int j = 0; j<font.Size(ch); j++)
//        cout << "X=" << font.X(ch,j) << " Y=" << font.Y(ch,j) << endl;
//   }
// }
//
// History:
//
// 15.11.2001 Mika Heiskanen
//
// ======================================================================

#ifndef _NFMIHERSHEYDATA_H
#define _NFMIHERSHEYDATA_H

#include "NFmiDef.h"
#include <string>
#include <vector>

const int kFmiFirstHersheyChar = 32;
const int kFmiHersheyZero = 'R';

class _FMI_DLL NFmiHersheyData
{
public:
  
  NFmiHersheyData(const std::string & theFileName);
  ~NFmiHersheyData(void) {}
  
  bool IsValid(void) const { return itIsValid; }
  
  int Size(unsigned char theChar) const;
  bool IsSkip(unsigned char theChar, unsigned int i) const;
  int X(unsigned char theChar, unsigned int i) const;
  int Y(unsigned char theChar, unsigned int i) const;
  
  // Character specific bounding boxes:
  
  int MinX(unsigned char) const;
  int MaxX(unsigned char) const;
  int MinY(unsigned char) const;
  int MaxY(unsigned char) const;
  
  // Font specific bounding boxes:
  
  int MinX(void) const { return itsFontMinX - kFmiHersheyZero; }
  int MaxX(void) const { return itsFontMaxX - kFmiHersheyZero; }
  int MinY(void) const { return itsFontMinY - kFmiHersheyZero; }
  int MaxY(void) const { return itsFontMaxY - kFmiHersheyZero; }
  
private:
  
  NFmiHersheyData() {}
  
  bool itIsValid;
  
  std::vector<std::string> itsData;
  
  std::string itsCharactersMinX;
  std::string itsCharactersMaxX;
  
  std::string itsCharactersMinY;
  std::string itsCharactersMaxY;
  
  unsigned char itsFontMinY;
  unsigned char itsFontMaxY;
  
  unsigned char itsFontMinX;
  unsigned char itsFontMaxX;
  
};

#endif // _NFMIHERSHEYDATA_H

// ======================================================================
