// ======================================================================
/*!
 * \file NFmiContourDataHelper.cpp
 * \brief Defintion of a class to contain helpful speedup information
 *        on data to be contoured for NFmiContourTree.
 *
 * \b History:
 *
 * \li 18.01.2002, Mika Heiskanen\par
 * Implemented
 *
 */
// ======================================================================

#include "NFmiContourDataHelper.h"

// ======================================================================
/*!
 * The constructor takes as input the NFmiDataMatrix containing the
 * values to be contoured. It then calculates minmax values for each
 * row and stores them internally for later use.
 */
// ======================================================================

NFmiContourDataHelper::NFmiContourDataHelper(const NFmiDataMatrix<float> & theMatrix)
  : itsLoLimit(kFloatMissing)
  , itsHiLimit(kFloatMissing)
  , itsMissingFlag(false)
{
  int n = theMatrix.NY();
  
  // Temporary information holders for matrix rows
  
  vector<float> lolimits(n,kFloatMissing);
  vector<float> hilimits(n,kFloatMissing);
  vector<bool>  missingflags(n,false);
  
  // Then check the status for each row of data
  
  unsigned int j;
  for(j=0; j<theMatrix.NY(); j++)
    {
      float lo=kFloatMissing;
      float hi=kFloatMissing;
      bool missing=false;
      for(unsigned int i=0; i<theMatrix.NX(); i++)
		{
		  float value = theMatrix[i][j];
		  if(value == kFloatMissing)
			missing = true;
		  else if(lo == kFloatMissing)
			lo = hi = value;
		  else
			{
			  lo = (lo<value ? lo : value);
			  hi = (hi>value ? hi : value);
			}
		}
	  
      lolimits[j] = lo;
      hilimits[j] = hi;
      missingflags[j] = missing;
    }
  
  // Allocate the data part to its correct size
  
  itsLoLimits.resize(n,kFloatMissing);
  itsHiLimits.resize(n,kFloatMissing);
  itsMissingFlags.resize(n,false);
  
  // Then establish the speedup information for each contouring
  // row, which consts of two adjacent rows in the data matrix.
  
  for(j=0; j<theMatrix.NY()-1; j++)
    {
      // row is missing oif both rows are missing
	  
      itsMissingFlags[j] = missingflags[j] && missingflags[j+1];
	  
      // lolimit is the minimum of the 2 rows
	  
      if(lolimits[j]==kFloatMissing)
		itsLoLimits[j]=lolimits[j+1];
      else if(lolimits[j+1]==kFloatMissing)
		itsLoLimits[j]=lolimits[j];
      else
		itsLoLimits[j] = min(lolimits[j],lolimits[j+1]);
	  
      // hilimit is the maximum of the 2 rows
	  
      if(hilimits[j]==kFloatMissing)
		itsHiLimits[j]=hilimits[j+1];
      else if(hilimits[j+1]==kFloatMissing)
		itsHiLimits[j]=hilimits[j];
      else
		itsHiLimits[j] = max(hilimits[j],hilimits[j+1]);
	  
      // Respective information for the entire matrix
	  
      itsMissingFlag |= itsMissingFlags[j];
	  
      if(itsLoLimits[j]!=kFloatMissing)
		if(itsLoLimit==kFloatMissing)
		  itsLoLimit = itsLoLimits[j];
		else
		  itsLoLimit = min(itsLoLimit,itsLoLimits[j]);
	  
      if(itsHiLimits[j]!=kFloatMissing)
		if(itsHiLimit==kFloatMissing)
		  itsHiLimit = itsHiLimits[j];
		else
		  itsHiLimit = max(itsHiLimit,itsHiLimits[j]);
    }
}

// ======================================================================


