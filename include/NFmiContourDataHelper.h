// ======================================================================
/*!
 * \file NFmiContourDataHelper.h
 * \brief Definition of a class to contain helpful speedup information
 *        on data to be contoured for NFmiContourTree.
 *
 * \b History:
 *
 * \li 18.01.2002, Mika Heiskanen\par
 * Implemented
 *
 */
// ======================================================================
/*!
 * \class NFmiContourDataHelper
 *
 * When contouring a very large NFmiDataMatrix, for example radar data,
 * a lot of function calls can be avoided if the NFmiDataMatrix is
 * analyzed before actual contouring for possible speedups.
 *
 * The current idea is to store the minimum and maximum values for
 * each row of data, and then while contouring use that information
 * to possibly skip the row completely, or alternatively to colour
 * it completely in a single step.
 *
 * Also, for potentially great speedups, global minima and maxima
 * are also determined.
 *
 */
// ======================================================================

#ifndef _NFMICONTOURDATAHELPER_H
#define _NFMICONTOURDATAHELPER_H

// Essential includes:

#include <vector>
#include "NFmiDataMatrix.h"

//! Class for analyzing a NFmiDataMatrix for contouring speedups.

class NFmiContourDataHelper
{
public:
  
  //! Constructor
  
  NFmiContourDataHelper(const NFmiDataMatrix<float> & theMatrix);
  
  //! Destructor
  
  ~NFmiContourDataHelper(void) {}
  
  //! Return minimum value in the matrix
  
  float Min(void) const
  { return itsLoLimit; }
  
  //! Return minimum value on the given contouring row
  
  float Min(int row) const
  { return itsLoLimits[row]; }
  
  //! Return maximum value in the matrix
  
  float Max(void) const
  { return itsHiLimit; }
  
  //! Return maximum value on the given contouring row
  
  float Max(int row) const
  { return itsHiLimits[row]; }
  
  //! Return whether there were missing values in the matrix
  
  bool HasMissing(void) const
  { return itsMissingFlag; }
  
  //! Return whether there were missing values on the given contouring row
  
  bool HasMissing(int row) const
  { return itsMissingFlags[row]; }
  
  //! Return whether the matrix consists only of missing values
  
  bool IsFullyMissing(void) const
  { return itsLoLimit==kFloatMissing; }
  
  //! Return whether the contouring row consists only of missing values
  
  bool IsFullyMissing(int row) const
  { return itsLoLimits[row]==kFloatMissing; }
  
private:
  
  //! Default constructor is disabled
  
  NFmiContourDataHelper(void);
  
  // The following data elements have a value for each contouring row
  // of the input NFmiDataMatrix, which is equal to the number of rows
  // in the matrix minus one.
  
  //! Low limit for all the data. kFloatMissing implies matrix has no values.
  
  float itsLoLimit;
  
  //! Hi limit for all the data. kFloatMissing implies matrix has no values.
  
  float itsHiLimit;
  
  //! Boolean to indicate whether the matrix contains any missing values
  
  bool itsMissingFlag;
  
  //! Low limits of data. kFloatMissing implies row has no values.
  
  std::vector<float> itsLoLimits;
  
  //! High limits of data. kFloatMissing implies row has no values.
  
  std::vector<float> itsHiLimits;
  
  //! Booleans to indicate whether a row has missing values
  
  std::vector<bool> itsMissingFlags;
  
};

#endif // _NFMICONTOURDATAHELPER_H

// ======================================================================
