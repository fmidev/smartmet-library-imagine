// ======================================================================
/*!
 * \file NFmiContourTree.cpp
 * \brief Definition of a class to calculate 2D contour polygons.
 *
 * \b History:
 *
 * \li 13.08.2001, Mika Heiskanen\par
 * Implemented.
 *
 * \li 14.08.2001, Mika Heiskanen\par
 * Forced recursion level to be exactly the same in cells which
 * are 'painted' to make sure the edges match exactly. This
 * implied not being able to handle saddle points conveniently.
 *
 * \li 27.08.2001, Mika Heiskanen\par
 * Added special case \f$(-\infty,\infty)\f$ to mean the area
 * containing missing values. The constructor will initialize the tree
 * to contain the entire (finite) real space, so that the
 * actual contouring will be to crop the areas where finite
 * values do exist.
 * Also, when only one value in a rectangle is missing, the
 * opposite corner of the rectangle is contoured using
 * a triangle contourer. 
 *
 * \li 12.09.2001, Mika Heiskanen\par
 * Fixed painting of saddle points by using triangulation
 * of the rectangle.
 *
 * \li 27.11.2001, Mika Heiskanen\par
 * Added Doxygen comments.
 *
 * \li 18.01.2002, Mika Heiskanen\par
 * Added use of NFmiContourDataHelper.
 *
 */
// ======================================================================

#include "NFmiContourTree.h"
#include <stdexcept>


#ifndef square
/// Utility macro to calculate number squared.
#define square(a) ((a)*(a))
#endif

using namespace std;

namespace Imagine
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the path
   *
   * Note how we handle missing value contouring by explicitly adding
   * a surrounding box afterwards.
   */
  // ----------------------------------------------------------------------
  
  NFmiPath NFmiContourTree::Path(void) const
  {
	NFmiPath path = NFmiEdgeTree::Path();
	
	if(path.Size() != 0 && ContouringMissing())
	  path.InsideOut();
	
	return path;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   * \param theInterpolation The interpolation method to be used.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::Contour(const NFmiDataMatrix<NFmiPoint> & thePts,
								const NFmiDataMatrix<float> & theValues,
								const NFmiContourInterpolation & theInterpolation,
								int theMaxDepth)
  {
	assert(thePts.NX()==theValues.NX() && thePts.NY()==theValues.NY());
	
	switch(theInterpolation)
	  {
	  case kFmiContourNearest:
		NFmiContourTree::ContourNearest(thePts,theValues);
		break;
	  case kFmiContourLinear:
		NFmiContourTree::ContourLinear(thePts,theValues,theMaxDepth);
		break;
	  case kFmiContourDiscrete:
		NFmiContourTree::ContourDiscrete(thePts,theValues);
	  default:
		break;
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param theValues The values at the points.
   * \param theInterpolation The interpolation method to be used.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::Contour(const NFmiDataMatrix<float> & theValues,
								const NFmiContourInterpolation & theInterpolation,
								int theMaxDepth)
  {
	switch(theInterpolation)
	  {
	  case kFmiContourNearest:
		NFmiContourTree::ContourNearest(theValues);
		break;
	  case kFmiContourLinear:
		NFmiContourTree::ContourLinear(theValues,theMaxDepth);
		break;
	  case kFmiContourDiscrete:
		NFmiContourTree::ContourDiscrete(theValues);
		break;
	  default:
		break;
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up contouring.
   * \param theInterpolation The interpolation method to be used.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::Contour(const NFmiDataMatrix<NFmiPoint> & thePts,
								const NFmiDataMatrix<float> & theValues,
								const NFmiContourDataHelper & theHelper,
								const NFmiContourInterpolation & theInterpolation,
								int theMaxDepth)
  {
	assert(thePts.NX()==theValues.NX() && thePts.NY()==theValues.NY());
	
	switch(theInterpolation)
	  {
	  case kFmiContourNearest:
		NFmiContourTree::ContourNearest(thePts,theValues,theHelper);
		break;
	  case kFmiContourLinear:
		NFmiContourTree::ContourLinear(thePts,theValues,theHelper,theMaxDepth);
		break;
	  case kFmiContourDiscrete:
		NFmiContourTree::ContourDiscrete(thePts,theValues,theHelper);
		break;
	  default:
		break;
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up contouring.
   * \param theInterpolation The interpolation method to be used.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::Contour(const NFmiDataMatrix<float> & theValues,
								const NFmiContourDataHelper & theHelper,
								const NFmiContourInterpolation & theInterpolation,
								int theMaxDepth)
  {
	switch(theInterpolation)
	  {
	  case kFmiContourNearest:
		NFmiContourTree::ContourNearest(theValues,theHelper);
		break;
	  case kFmiContourLinear:
		NFmiContourTree::ContourLinear(theValues,theHelper,theMaxDepth);
		break;
	  case kFmiContourDiscrete:
		NFmiContourTree::ContourDiscrete(theValues,theHelper);
		break;
	  default:
		break;
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourNearest(const NFmiDataMatrix<NFmiPoint> & thePts,
									   const NFmiDataMatrix<float> & theValues)
  {
	for(unsigned int j=0; j<thePts.NY()-1; j++)
	  for(unsigned int i=0; i<thePts.NX()-1; i++)
		
		ContourNearest4(thePts[i][j].X(),thePts[i][j].Y(),theValues[i][j],
						thePts[i+1][j].X(),thePts[i+1][j].Y(),theValues[i+1][j],
						thePts[i+1][j+1].X(),thePts[i+1][j+1].Y(),theValues[i+1][j+1],
						thePts[i][j+1].X(),thePts[i][j+1].Y(),theValues[i][j+1]);
  }

  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for discrete
   * interpolation.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourDiscrete(const NFmiDataMatrix<NFmiPoint> & thePts,
										const NFmiDataMatrix<float> & theValues)
  {
	for(unsigned int j=0; j<thePts.NY()-1; j++)
	  for(unsigned int i=0; i<thePts.NX()-1; i++)
		
		ContourDiscrete4(thePts[i][j].X(),thePts[i][j].Y(),theValues[i][j],
						 thePts[i+1][j].X(),thePts[i+1][j].Y(),theValues[i+1][j],
						 thePts[i+1][j+1].X(),thePts[i+1][j+1].Y(),theValues[i+1][j+1],
						 thePts[i][j+1].X(),thePts[i][j+1].Y(),theValues[i][j+1]);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param theValues The values at the points.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourNearest(const NFmiDataMatrix<float> & theValues)
  {
	for(unsigned int j=0; j<theValues.NY()-1; j++)
	  for(unsigned int i=0; i<theValues.NX()-1; i++)
		
		ContourNearest4(i,j,theValues[i][j],
						i+1,j,theValues[i+1][j],
						i+1,j+1,theValues[i+1][j+1],
						i,j+1,theValues[i][j+1]);
  }

  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param theValues The values at the points.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourDiscrete(const NFmiDataMatrix<float> & theValues)
  {
	for(unsigned int j=0; j<theValues.NY()-1; j++)
	  for(unsigned int i=0; i<theValues.NX()-1; i++)
		
		ContourDiscrete4(i,j,theValues[i][j],
						 i+1,j,theValues[i+1][j],
						 i+1,j+1,theValues[i+1][j+1],
						 i,j+1,theValues[i][j+1]);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up the contouring
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourNearest(const NFmiDataMatrix<NFmiPoint> & thePts,
									   const NFmiDataMatrix<float> & theValues,
									   const NFmiContourDataHelper & theHelper)
  {
	if(theHelper.IsFullyMissing())
	  return;
	
	VertexInsidedness cmin = Insidedness(theHelper.Min());
	VertexInsidedness cmax = Insidedness(theHelper.Max());
	
	// If min,max both below or above, skip the contour
	
	if(cmin==cmax && cmin!=kInside)
	  return;
	
	// If min,max both inside, we could contour the entire matrix at once,
	// except that we'd need to check whether the matrix contained missing
	// values which are NOT to be included. For now we ignore this
	// optimization and just contour the matrix as usual.
	
	for(unsigned int j=0; j<thePts.NY()-1; j++)
	  {
		// Ignore fully missing rows
		
		if(theHelper.IsFullyMissing(j))
		  continue;
		
		cmin = Insidedness(theHelper.Min(j));
		cmax = Insidedness(theHelper.Max(j));
		
		// If min,max both below or above, skip to next row
		
		if(cmin==cmax && cmin!=kInside)
		  continue;
		
		// If min,max both inside, we could contour the entire row at once,
		// except that we'd need to check whether the row contained missing
		// values which are NOT to be included. For now we ignore this
		// optimization and just contour the row as usual.
		
		for(unsigned int i=0; i<thePts.NX()-1; i++)
		  ContourNearest4(thePts[i][j].X(),thePts[i][j].Y(),theValues[i][j],
						  thePts[i+1][j].X(),thePts[i+1][j].Y(),theValues[i+1][j],
						  thePts[i+1][j+1].X(),thePts[i+1][j+1].Y(),theValues[i+1][j+1],
						  thePts[i][j+1].X(),thePts[i][j+1].Y(),theValues[i][j+1]);
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up the contouring
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourDiscrete(const NFmiDataMatrix<NFmiPoint> & thePts,
										const NFmiDataMatrix<float> & theValues,
										const NFmiContourDataHelper & theHelper)
  {
	if(theHelper.IsFullyMissing())
	  return;
	
	VertexInsidedness cmin = Insidedness(theHelper.Min());
	VertexInsidedness cmax = Insidedness(theHelper.Max());
	
	// If min,max both below or above, skip the contour
	
	if(cmin==cmax && cmin!=kInside)
	  return;
	
	// If min,max both inside, we could contour the entire matrix at once,
	// except that we'd need to check whether the matrix contained missing
	// values which are NOT to be included. For now we ignore this
	// optimization and just contour the matrix as usual.
	
	for(unsigned int j=0; j<thePts.NY()-1; j++)
	  {
		// Ignore fully missing rows
		
		if(theHelper.IsFullyMissing(j))
		  continue;
		
		cmin = Insidedness(theHelper.Min(j));
		cmax = Insidedness(theHelper.Max(j));
		
		// If min,max both below or above, skip to next row
		
		if(cmin==cmax && cmin!=kInside)
		  continue;
		
		// If min,max both inside, we could contour the entire row at once,
		// except that we'd need to check whether the row contained missing
		// values which are NOT to be included. For now we ignore this
		// optimization and just contour the row as usual.
		
		for(unsigned int i=0; i<thePts.NX()-1; i++)
		  ContourDiscrete4(thePts[i][j].X(),thePts[i][j].Y(),theValues[i][j],
						   thePts[i+1][j].X(),thePts[i+1][j].Y(),theValues[i+1][j],
						   thePts[i+1][j+1].X(),thePts[i+1][j+1].Y(),theValues[i+1][j+1],
						   thePts[i][j+1].X(),thePts[i][j+1].Y(),theValues[i][j+1]);
	  }
  }
  
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up the contouring
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourNearest(const NFmiDataMatrix<float> & theValues,
									   const NFmiContourDataHelper & theHelper)
  {
	if(theHelper.IsFullyMissing())
	  return;
	
	VertexInsidedness cmin = Insidedness(theHelper.Min());
	VertexInsidedness cmax = Insidedness(theHelper.Max());
	
	// If min,max both below or above, skip the contour
	
	if(cmin==cmax && cmin!=kInside)
	  return;
	
	// If min,max both inside, we could contour the entire matrix at once,
	// except that we'd need to check whether the matrix contained missing
	// values which are NOT to be included. For now we ignore this
	// optimization and just contour the matrix as usual.
	
	for(unsigned int j=0; j<theValues.NY()-1; j++)
	  {
		// Ignore fully missing rows
		
		if(theHelper.IsFullyMissing(j))
		  continue;
		
		cmin = Insidedness(theHelper.Min(j));
		cmax = Insidedness(theHelper.Max(j));
		
		// If min,max both below or above, skip to next row
		
		if(cmin==cmax && cmin!=kInside)
		  continue;
		
		// If min,max both inside, we could contour the entire row at once,
		// except that we'd need to check whether the row contained missing
		// values which are NOT to be included. For now we ignore this
		// optimization and just contour the row as usual.
		
		for(unsigned int i=0; i<theValues.NX()-1; i++)
		  ContourNearest4(i,j,theValues[i][j],
						  i+1,j,theValues[i+1][j],
						  i+1,j+1,theValues[i+1][j+1],
						  i,j+1,theValues[i][j+1]);
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   *
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up the contouring
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourDiscrete(const NFmiDataMatrix<float> & theValues,
										const NFmiContourDataHelper & theHelper)
  {
	if(theHelper.IsFullyMissing())
	  return;
	
	VertexInsidedness cmin = Insidedness(theHelper.Min());
	VertexInsidedness cmax = Insidedness(theHelper.Max());
	
	// If min,max both below or above, skip the contour
	
	if(cmin==cmax && cmin!=kInside)
	  return;
	
	// If min,max both inside, we could contour the entire matrix at once,
	// except that we'd need to check whether the matrix contained missing
	// values which are NOT to be included. For now we ignore this
	// optimization and just contour the matrix as usual.
	
	for(unsigned int j=0; j<theValues.NY()-1; j++)
	  {
		// Ignore fully missing rows
		
		if(theHelper.IsFullyMissing(j))
		  continue;
		
		cmin = Insidedness(theHelper.Min(j));
		cmax = Insidedness(theHelper.Max(j));
		
		// If min,max both below or above, skip to next row
		
		if(cmin==cmax && cmin!=kInside)
		  continue;
		
		// If min,max both inside, we could contour the entire row at once,
		// except that we'd need to check whether the row contained missing
		// values which are NOT to be included. For now we ignore this
		// optimization and just contour the row as usual.
		
		for(unsigned int i=0; i<theValues.NX()-1; i++)
		  ContourDiscrete4(i,j,theValues[i][j],
						   i+1,j,theValues[i+1][j],
						   i+1,j+1,theValues[i+1][j+1],
						   i,j+1,theValues[i][j+1]);
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear(const NFmiDataMatrix<NFmiPoint> & thePts,
									  const NFmiDataMatrix<float> & theValues,
									  int theMaxDepth)
  {
	for(unsigned int j=0; j<thePts.NY()-1; j++)
	  for(unsigned int i=0; i<thePts.NX()-1; i++)
		
		ContourLinear4(thePts[i][j].X(),thePts[i][j].Y(),theValues[i][j],
					   thePts[i+1][j].X(),thePts[i+1][j].Y(),theValues[i+1][j],
					   thePts[i+1][j+1].X(),thePts[i+1][j+1].Y(),theValues[i+1][j+1],
					   thePts[i][j+1].X(),thePts[i][j+1].Y(),theValues[i][j+1],
					   theMaxDepth);
	
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param theValues The values at the points.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear(const NFmiDataMatrix<float> & theValues,
									  int theMaxDepth)
  {
	for(unsigned int j=0; j<theValues.NY()-1; j++)
	  for(unsigned int i=0; i<theValues.NX()-1; i++)
		
		ContourLinear4(i,j,theValues[i][j],
					   i+1,j,theValues[i+1][j],
					   i+1,j+1,theValues[i+1][j+1],
					   i,j+1,theValues[i][j+1],
					   theMaxDepth);
	
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param thePts The coordinates of the points.
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up the contouring.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear(const NFmiDataMatrix<NFmiPoint> & thePts,
									  const NFmiDataMatrix<float> & theValues,
									  const NFmiContourDataHelper & theHelper,
									  int theMaxDepth)
  {
	if(theHelper.IsFullyMissing())
	  return;
	
	VertexInsidedness cmin = Insidedness(theHelper.Min());
	VertexInsidedness cmax = Insidedness(theHelper.Max());
	
	// If min,max both below or above, skip the contour
	
	if(cmin==cmax && cmin!=kInside)
	  return;
	
	// If fully inside and there are no missing values, we can do it quickly
	// Is is especially beneficial for contouring the missing part of Tuliset,
	// since there are no missing values in the grid.
	
	if(cmin==cmax && cmin==kInside && !theHelper.HasMissing())
	  {
		// First and last columns
		unsigned int j=0;
		unsigned int i=0;
		for(j=0; j<thePts.NY()-1; j++)
		  for(i=0; i<thePts.NX(); i+=thePts.NX()-1)
			{
			  float x1 = thePts[i][j].X();
			  float y1 = thePts[i][j].Y();
			  
			  float x2 = thePts[i][j+1].X();
			  float y2 = thePts[i][j+1].Y();
			  
			  VertexExactness c1 = Exactness(theValues[i][j]);
			  VertexExactness c2 = Exactness(theValues[i][j+1]);
			  
			  Add(NFmiEdge(x1,y1,x2,y2,c1==c2 && c1!=kNeither));
			}
		
		// First and last rows
		
		for(j=0; j<thePts.NY(); j+=thePts.NY()-1)
		  for(i=0; i<thePts.NX()-1; i++)
			{
			  float x1 = thePts[i][j].X();
			  float y1 = thePts[i][j].Y();
			  
			  float x2 = thePts[i+1][j].X();
			  float y2 = thePts[i+1][j].Y();
			  
			  VertexExactness c1 = Exactness(theValues[i][j]);
			  VertexExactness c2 = Exactness(theValues[i+1][j]);
			  
			  Add(NFmiEdge(x1,y1,x2,y2,c1==c2 && c1!=kNeither));
			}
		
	  }
	
	
	else
	  
	  {
		// If min,max both inside, we could contour the entire matrix at once,
		// except that we'd need to check whether the matrix contained missing
		// values which are NOT to be included. For now we ignore this
		// optimization and just contour the matrix as usual.
		
		for(unsigned int j=0; j<thePts.NY()-1; j++)
		  {
			// Ignore fully missing rows
			
			if(theHelper.IsFullyMissing(j))
			  continue;
			
			cmin = Insidedness(theHelper.Min(j));
			cmax = Insidedness(theHelper.Max(j));
			
			// If min,max both below or above, skip to next row
			
			if(cmin==cmax && cmin!=kInside)
			  continue;
			
			// If min,max both inside, we could contour the entire row at once,
			// except that we'd need to check whether the row contained missing
			// values which are NOT to be included. For now we ignore this
			// optimization and just contour the row as usual.
			
			for(unsigned int i=0; i<thePts.NX()-1; i++)
			  ContourLinear4(thePts[i][j].X(),thePts[i][j].Y(),theValues[i][j],
							 thePts[i+1][j].X(),thePts[i+1][j].Y(),theValues[i+1][j],
							 thePts[i+1][j+1].X(),thePts[i+1][j+1].Y(),theValues[i+1][j+1],
							 thePts[i][j+1].X(),thePts[i][j+1].Y(),theValues[i][j+1],
							 theMaxDepth);
		  }
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour the given data values with given coordinates. The input data
   * is expected to be topologically equivalent to a 2D uniform rectangle.
   * Note that contour recursion depth is meaningless for nearest neighbour
   * interpolation, the contours would be exactly equivalent.
   *
   * \param theValues The values at the points.
   * \param theHelper A NFmiContourDataHelper for speeding up the contouring.
   * \param theMaxDepth The maximum recursion depth to be used for obtaining
   *	(slightly) smoother contour lines.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear(const NFmiDataMatrix<float> & theValues,
									  const NFmiContourDataHelper & theHelper,
									  int theMaxDepth)
  {
	if(theHelper.IsFullyMissing())
	  return;
	
	VertexInsidedness cmin = Insidedness(theHelper.Min());
	VertexInsidedness cmax = Insidedness(theHelper.Max());
	
	// If min,max both below or above, skip the contour
	
	if(cmin==cmax && cmin!=kInside)
	  return;
	
	// If fully inside and there are no missing values, we can do it quickly
	// Is is especially beneficial for contouring the missing part of Tuliset,
	// since there are no missing values in the grid.
	
	if(cmin==cmax && cmin==kInside && !theHelper.HasMissing())
	  {
		// First and last columns
		unsigned int j=0;
		unsigned int i=0;
		for(j=0; j<theValues.NY()-1; j++)
		  for(i=0; i<theValues.NX(); i+=theValues.NX()-1)
			{
			  VertexExactness c1 = Exactness(theValues[i][j]);
			  VertexExactness c2 = Exactness(theValues[i][j+1]);
			  
			  Add(NFmiEdge(i,j,i,j+1,c1==c2 && c1!=kNeither));
			}
		
		// First and last rows
		
		for(j=0; j<theValues.NY(); j+=theValues.NY()-1)
		  for(i=0; i<theValues.NX()-1; i++)
			{
			  VertexExactness c1 = Exactness(theValues[i][j]);
			  VertexExactness c2 = Exactness(theValues[i+1][j]);
			  
			  Add(NFmiEdge(i,j,i+1,j,c1==c2 && c1!=kNeither));
			}
		
	  }
	
	
	else
	  
	  {
		// If min,max both inside, we could contour the entire matrix at once,
		// except that we'd need to check whether the matrix contained missing
		// values which are NOT to be included. For now we ignore this
		// optimization and just contour the matrix as usual.
		
		for(unsigned int j=0; j<theValues.NY()-1; j++)
		  {
			// Ignore fully missing rows
			
			if(theHelper.IsFullyMissing(j))
			  continue;
			
			cmin = Insidedness(theHelper.Min(j));
			cmax = Insidedness(theHelper.Max(j));
			
			// If min,max both below or above, skip to next row
			
			if(cmin==cmax && cmin!=kInside)
			  continue;
			
			// If min,max both inside, we could contour the entire row at once,
			// except that we'd need to check whether the row contained missing
			// values which are NOT to be included. For now we ignore this
			// optimization and just contour the row as usual.
			
			for(unsigned int i=0; i<theValues.NX()-1; i++)
			  ContourLinear4(i,j,theValues[i][j],
							 i+1,j,theValues[i+1][j],
							 i+1,j+1,theValues[i+1][j+1],
							 i,j+1,theValues[i][j+1],
							 theMaxDepth);
		  }
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour a rectangular element. The input is the 4 coordinates defining
   * the rectangle, and the values at the rectangle vertices. The order
   * of the vertices may be clockwise or counter-clockwise, it does not
   * matter.
   *
   * Any of the input values may be missing, which implies the polygon
   * will not cause any contours to be output, except in the case
   * when only one value is missing, in which case ContourLinear3 is called.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear4(float x1, float y1, float z1,
									   float x2, float y2, float z2,
									   float x3, float y3, float z3,
									   float x4, float y4, float z4,
									   int maxdepth)
  {
	// Abort if any of the coordinates is missing
	
	if(x1==kFloatMissing ||
	   x2==kFloatMissing ||
	   x3==kFloatMissing ||
	   x4==kFloatMissing ||
	   y1==kFloatMissing ||
	   y2==kFloatMissing ||
	   y3==kFloatMissing ||
	   y4==kFloatMissing)
	  {
		return;
	  }
	
	// Handle missing values
	
	if(!IsValid(z1) || !IsValid(z2) || !IsValid(z3) || !IsValid(z4))
	  {
		if(IsValid(z1) && IsValid(z2) && IsValid(z3))
		  ContourLinear3(x1,y1,z1,x2,y2,z2,x3,y3,z3,maxdepth);
		else if(IsValid(z1)  && IsValid(z2) && IsValid(z4))
		  ContourLinear3(x1,y1,z1,x2,y2,z2,x4,y4,z4,maxdepth);
		else if(IsValid(z1) && IsValid(z3) && IsValid(z4))
		  ContourLinear3(x1,y1,z1,x3,y3,z3,x4,y4,z4,maxdepth);
		else if(IsValid(z2) && IsValid(z3) && IsValid(z4))
		  ContourLinear3(x2,y2,z2,x3,y3,z3,x4,y4,z4,maxdepth);
		return;
	  }
	
	// If both limits are missing, cover the entire rectangle, since
	// it contains only valid values now. The contours are never exact
	// when contouring missing values
	
	if(ContouringMissing())
	  {
		Add(NFmiEdge(x1,y1,x2,y2,false));
		Add(NFmiEdge(x2,y2,x3,y3,false));
		Add(NFmiEdge(x3,y3,x4,y4,false));
		Add(NFmiEdge(x4,y4,x1,y1,false));
		return;
	  }
	
	// Establish where the edges reside with respect to the desired range
	// -1 implies below, 0 inside, 1 above. Also note that in the errorneous
	// case of having both limits missing, which should by definition
	// mean range (-infinity,infinity), the default insidedness value is
	// correct.
	
	VertexInsidedness c1 = Insidedness(z1);
	VertexInsidedness c2 = Insidedness(z2);
	VertexInsidedness c3 = Insidedness(z3);
	VertexInsidedness c4 = Insidedness(z4);
	
	// If all points are in the same region, no recursion is needed
	
	if(c1==c2 && c2==c3 && c3==c4 && c1!=kInside)
	  {
		// If above or below, nothing to do
		
		if(c1 != kInside)		// here c1=c2=c3=c4
		  return;
		
		// Otherwise all points are inside, all we need to know is whether
		// the rectangle edges are exact or not. An edge is exact, if the
		// values on the edge are exactly equal to one of the range limits.
		// Also, it is important to note that we must effectively recurse
		// to cut the edges of the cell to the proper sizes as implied
		// by the maxdepth argument. Otherwise the line segments from
		// adjacent cells may not match. Luckily, we do not need to
		// recurse the entire cell, recursing the outline alone is sufficient.
		// The alternative would be to try matching overlapping edges, which
		// would be an unnecessarily complex operation.
		
		Add(NFmiEdge(x1,y1,x2,y2,z1==z2 && (z1==LoLimit() || z1==HiLimit())));
		Add(NFmiEdge(x2,y2,x3,y3,z2==z3 && (z2==LoLimit() || z2==HiLimit())));
		Add(NFmiEdge(x3,y3,x4,y4,z3==z4 && (z3==LoLimit() || z3==HiLimit())));
		Add(NFmiEdge(x4,y4,x1,y1,z4==z1 && (z4==LoLimit() || z4==HiLimit())));
		
		return;
	  }
	
	// Now, if we want extra smoothness via recursion, we do it here
	
	if(maxdepth>0)
	  {
		ContourLinear4Recurse(x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,maxdepth-1);
		return;
	  }
	
	// Now we know the cell is to be painted only partially.
	// In general we can simply go around the rectangle, enumerating
	// all points of intersection (and those corner points inside the
	// region) in order, and then simply connect them to get the painted
	// area. However, in case of saddle points we may have to paint
	// two disjointed areas.
	//
	// By enumerating all the possible perumutations of below,
	// inside, above, it is easy to see the troublesome cases include
	// precisely the set of permutations where the 'insidedness' of two
	// opposite vertices is exactly the same, and the insidedness of
	// the two remaining vertices is different from the other two:
	//
	//    below-below   vs (inside-inside or above-above or inside-above)
	//    inside-inside vs (below-below or above-above or below-above)
	//    above-above   vs (below-below or inside-inside or below-inside)
	//
	// *ALL* the cases can be easily solved by subdividing the rectangle
	// into 4 triangles as follows:
	//
	// 
	//      1------2
	//      |\    /|
	//      | \  / |
	//      |  \/  |
	//      |  /\  |
	//      | /  \ |
	//      |/    \|
	//      4------3
	//
	// The figure still shares the same edges with adjacent cells, hence
	// the intersection points along them will be exactly the same. Also,
	// inside the triangles there cannot be any saddle points. Combining
	// the solutions of the 4 triangles effectively then solves the
	// saddle point problem.
	
	// It is possible that two adjacent ranges consider the rectangle
	// differently - one sees it as a saddle point, one doesn't. This
	// will result in pixel-size gaps between the contours, since
	// the solution from the triangles will contain additional points.
	// The simple fix is to always triangulate all rectangles, which
	// are only partially covered (and hence contain atleast two
	// different contour ranges)
	
	if(itsSubTrianglesOn)
	  {
		float x0 = (x1+x2+x3+x4)/4;
		float y0 = (y1+y2+y3+y4)/4;
		float z0 = (z1+z2+z3+z4)/4;
		ContourLinear3(x1,y1,z1,x2,y2,z2,x0,y0,z0,maxdepth-1);
		ContourLinear3(x2,y2,z2,x3,y3,z3,x0,y0,z0,maxdepth-1);
		ContourLinear3(x3,y3,z3,x4,y4,z4,x0,y0,z0,maxdepth-1);
		ContourLinear3(x4,y4,z4,x1,y1,z1,x0,y0,z0,maxdepth-1);
		return;
	  }
	
	// Now, if there is a saddle point, we utilize triangles
	// to get the path correct, then collapse the small
	// line segments back into straight lines. Often one may
	// wish to avoid the subtriangles since using them may
	// make the contour a bit spiky.
	
	bool saddlepoint = ((c1==c3 || c2==c4) && (c1!=c2 && c3!=c4));
	
	if(saddlepoint)
	  {
		// Make new subcontourer
		NFmiContourTree subpath(itsLoLimit,
								itsHiLimit,
								itsLoLimitExact,
								itsHiLimitExact,
								itHasMissingValue,
								itsMissingValue);
		subpath.itHasDataLoLimit = itHasDataLoLimit;
		subpath.itHasDataHiLimit = itHasDataHiLimit;
		subpath.itsDataLoLimit = itsDataLoLimit;
		subpath.itsDataHiLimit = itsDataHiLimit;
		subpath.itsSubTrianglesOn = true;
		
		float x0 = (x1+x2+x3+x4)/4;
		float y0 = (y1+y2+y3+y4)/4;
		float z0 = (z1+z2+z3+z4)/4;
		subpath.ContourLinear3(x1,y1,z1,x2,y2,z2,x0,y0,z0,maxdepth-1);
		subpath.ContourLinear3(x2,y2,z2,x3,y3,z3,x0,y0,z0,maxdepth-1);
		subpath.ContourLinear3(x3,y3,z3,x4,y4,z4,x0,y0,z0,maxdepth-1);
		subpath.ContourLinear3(x4,y4,z4,x1,y1,z1,x0,y0,z0,maxdepth-1);
		
		// all ghostlines can be added as is
		{
		  for(EdgeTreeType::iterator it=subpath.itsEdges.begin();
			  it != subpath.itsEdges.end();
			  )
			{
			  if(it->Exact())
				++it;
			  else
				{
				  Add(*it);
				  subpath.itsEdges.erase(it);
				  it = subpath.itsEdges.begin();
				}
			}
		}
		
		// next we must collapse all polylines into single lines
		
		NFmiPath path = subpath.Path();
		const NFmiPathData & elements = path.Elements();
		float firstx = 0;
		float firsty = 0;
		float lastx = 0;
		float lasty = 0;
		for(NFmiPathData::const_iterator it=elements.begin();
			it!=elements.end();
			++it)
		  {
			switch((*it).Oper())
			  {
			  case kFmiMoveTo:
				if(it!=elements.begin())
				  Add(NFmiEdge(firstx,firsty,lastx,lasty,true));
				firstx = (*it).X();
				firsty = (*it).Y();
				lastx = firstx;
				lasty = firsty;
				break;
			  case kFmiLineTo:
				lastx = (*it).X();
				lasty = (*it).Y();
				if(it == --elements.end())
				  Add(NFmiEdge(firstx,firsty,lastx,lasty,true));
				break;
			  default:
				throw runtime_error("NFmiContourTree encountered bad path element");
			  }
		  }
		//	   cout << endl;
		
	  }
	
	// Here we know for certain that there are no ambiguous areas when
	// deciding what areas to 'fill'. Due to the nature of the algorithm
	// it is sufficient to simply enumerate all the edges surrounding
	// the inside area, the order is immaterial just as long as the set
	// of edges forms a closed area. This is clear by enumerating
	// all possible combinations, and studying what areas will be coloured.
	//
	// Hence we can simply go around the rectangle, enumerating all possible
	// intersection points in order. Connecting these intersection points
	// gives the resulting area.
	
	// Output the desired data into new vectors. Note that each edge
	// may be intersected max 2 times, hence 8 is the maximum size
	// of the vectors.
	
	else
	  
	  {
		vector<float> X,Y;
		vector<VertexExactness> B;
		
		IntersectEdge(X,Y,B, x1,y1,z1,c1, x2,y2,z2,c2);
		IntersectEdge(X,Y,B, x2,y2,z2,c2, x3,y3,z3,c3);
		IntersectEdge(X,Y,B, x3,y3,z3,c3, x4,y4,z4,c4);
		IntersectEdge(X,Y,B, x4,y4,z4,c4, x1,y1,z1,c1);
		
		// And add the data in the vectors into the tree
		
		AddEdges(X,Y,B);
	  }
	
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Handle contour recursion for rectangular polygons. The rectangle
   * is subdivided into 4 parts by connecting opposite midpoints of
   * the edges. The input is always valid, \c kFloatMissing never occurs
   * since this is only called from ContourLinear4, where \c kFloatMissing
   * has already been checked.
   *
   * \note
   * The calculated values are independent of the order of
   * the input vertices, since (a+b) is independent of the
   * order of a and b. Hence the code is safe as far as
   * coordinate equality comparisons from adjacent cells
   * are concerned.
   *
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear4Recurse(float x1, float y1, float z1,
											  float x2, float y2, float z2,
											  float x3, float y3, float z3,
											  float x4, float y4, float z4,
											  int depth)
  {
	// Edge center coordinates and function values
	
	float x12 = (x1+x2)/2;
	float x23 = (x2+x3)/2;
	float x34 = (x3+x4)/2;
	float x41 = (x4+x1)/2;
	
	float y12 = (y1+y2)/2;
	float y23 = (y2+y3)/2;
	float y34 = (y3+y4)/2;
	float y41 = (y4+y1)/2;
	
	float z12 = (z1+z2)/2;
	float z23 = (z2+z3)/2;
	float z34 = (z3+z4)/2;
	float z41 = (z4+z1)/2;
	
	// Rectangle center coordinates and function values
	
	float x0 = (x12+x34)/2;
	float y0 = (y12+y34)/2;
	float z0 = (z12+z34)/2;
	
	// Recursion
	//
	//  1--+--2
	//  |  |  |
	//  +--+--+
	//  |  |  |
	//  4--+--3
	
	ContourLinear4(x1,y1,z1,x12,y12,z12,x0,y0,z0,x41,y41,z41,depth);
	ContourLinear4(x12,y12,z12,x2,y2,z2,x23,y23,z23,x0,y0,z0,depth);
	ContourLinear4(x41,y41,z41,x0,y0,z0,x34,y34,z34,x4,y4,z4,depth);
	ContourLinear4(x0,y0,z0,x23,y23,z23,x3,y3,z3,x34,y34,z34,depth);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour a rectangular element using nearest neighbour interpolation.
   * The input is the 4 coordinates defining the rectangle, and the values
   * at the rectangle vertices. The order of the vertices may be clockwise
   * or counter-clockwise, it does not matter.
   *
   * Any of the input values may be missing, which implies the polygon
   * will not cause any contours to be output, except in the case
   * when only one value is missing, when ContourNearest3 is called.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourNearest4(float x1, float y1, float z1,
										float x2, float y2, float z2,
										float x3, float y3, float z3,
										float x4, float y4, float z4)
  {
	// Abort if any of the coordinates is missing
	
	if(x1==kFloatMissing ||
	   x2==kFloatMissing ||
	   x3==kFloatMissing ||
	   x4==kFloatMissing ||
	   y1==kFloatMissing ||
	   y2==kFloatMissing ||
	   y3==kFloatMissing ||
	   y4==kFloatMissing)
	  return;
	
	// Handle missing values
	
	if(!IsValid(z1) || !IsValid(z2) || !IsValid(z3) || !IsValid(z4))
	  {
		if(IsValid(z1) && IsValid(z2) && IsValid(z3))
		  ContourNearest3(x1,y1,z1,x2,y2,z2,x3,y3,z3);
		else if(IsValid(z1) && IsValid(z2) && IsValid(z4))
		  ContourNearest3(x1,y1,z1,x2,y2,z2,x4,y4,z4);
		else if(IsValid(z1) && z3!=kFloatMissing && IsValid(z4))
		  ContourNearest3(x1,y1,z1,x3,y3,z3,x4,y4,z4);
		else if(IsValid(z2) && z3!=kFloatMissing && IsValid(z4))
		  ContourNearest3(x2,y2,z2,x3,y3,z3,x4,y4,z4);
		return;
	  }
	
	// If both limits are missing, cover the entire rectangle, since
	// it contains only valid values now. The contours are never exact
	// when contouring missing values
	
	if(ContouringMissing())
	  {
		Add(NFmiEdge(x1,y1,x2,y2,false));
		Add(NFmiEdge(x2,y2,x3,y3,false));
		Add(NFmiEdge(x3,y3,x4,y4,false));
		Add(NFmiEdge(x4,y4,x1,y1,false));
		return;
	  }
	
	// Establish where the edges reside with respect to the desired range
	// -1 implies below, 0 inside, 1 above. Also note that in the errorneous
	// case of having both limits missing, which should by definition
	// mean range (-infinity,infinity), the default insidedness value is
	// correct.
	
	VertexInsidedness c1 = Insidedness(z1);
	VertexInsidedness c2 = Insidedness(z2);
	VertexInsidedness c3 = Insidedness(z3);
	VertexInsidedness c4 = Insidedness(z4);
	
	// We handle all 4 corners of the rectangle separately.
	
	// Edge center coordinates
	
	float x12 = (x1+x2)/2;
	float x23 = (x2+x3)/2;
	float x34 = (x3+x4)/2;
	float x41 = (x4+x1)/2;
	
	float y12 = (y1+y2)/2;
	float y23 = (y2+y3)/2;
	float y34 = (y3+y4)/2;
	float y41 = (y4+y1)/2;
	
	// Rectangle center coordinates
	
	float x0 = (x12+x34)/2;
	float y0 = (y12+y34)/2;
	
	if(c1==kInside)
	  {
		Add(NFmiEdge(x41,y41,x1,y1,true));
		Add(NFmiEdge(x1,y1,x12,y12,true));
	  }
	if(c2==kInside)
	  {
		Add(NFmiEdge(x12,y12,x2,y2,true));
		Add(NFmiEdge(x2,y2,x23,y23,true));
	  }
	if(c3==kInside)
	  {
		Add(NFmiEdge(x23,y23,x3,y3,true));
		Add(NFmiEdge(x3,y3,x34,y34,true));
	  }
	if(c4==kInside)
	  {
		Add(NFmiEdge(x34,y34,x4,y4,true));
		Add(NFmiEdge(x4,y4,x41,y41,true));
	  }
	if( (c1==kInside) ^ (c2==kInside) )
	  Add(NFmiEdge(x12,y12,x0,y0,true));
	if( (c2==kInside) ^ (c3==kInside) )
	  Add(NFmiEdge(x23,y23,x0,y0,true));
	if( (c3==kInside) ^ (c4==kInside) )
	  Add(NFmiEdge(x34,y34,x0,y0,true));
	if( (c4==kInside) ^ (c1==kInside) )
	  Add(NFmiEdge(x41,y41,x0,y0,true));
	
  }

  // ----------------------------------------------------------------------
  /*!
   * Contour a rectangular element using discrete interpolation.
   * The input is the 4 coordinates defining the rectangle, and the values
   * at the rectangle vertices. The order of the vertices may be clockwise
   * or counter-clockwise, it does not matter.
   *
   * Algorithm:
   *
   *  -# Fix inside points to value 1, outside points to value 2
   *     and then use regular linear interpolation.
   *
   * Any of the input values may be missing, which implies the polygon
   * will not cause any contours to be output, except in the case
   * when only one value is missing, when ContourDiscrete3 is called.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourDiscrete4(float x1, float y1, float z1,
										 float x2, float y2, float z2,
										 float x3, float y3, float z3,
										 float x4, float y4, float z4)
  {
	// Abort if any of the coordinates is missing
	
	if(x1==kFloatMissing ||
	   x2==kFloatMissing ||
	   x3==kFloatMissing ||
	   x4==kFloatMissing ||
	   y1==kFloatMissing ||
	   y2==kFloatMissing ||
	   y3==kFloatMissing ||
	   y4==kFloatMissing)
	  return;
	
	// Handle missing values
	
	if(!IsValid(z1) || !IsValid(z2) || !IsValid(z3) || !IsValid(z4))
	  {
		if(IsValid(z1) && IsValid(z2) && IsValid(z3))
		  ContourDiscrete3(x1,y1,z1,x2,y2,z2,x3,y3,z3);
		else if(IsValid(z1) && IsValid(z2) && IsValid(z4))
		  ContourDiscrete3(x1,y1,z1,x2,y2,z2,x4,y4,z4);
		else if(IsValid(z1) && z3!=kFloatMissing && IsValid(z4))
		  ContourDiscrete3(x1,y1,z1,x3,y3,z3,x4,y4,z4);
		else if(IsValid(z2) && z3!=kFloatMissing && IsValid(z4))
		  ContourDiscrete3(x2,y2,z2,x3,y3,z3,x4,y4,z4);
		return;
	  }
	
	// If both limits are missing, cover the entire rectangle, since
	// it contains only valid values now. The contours are never exact
	// when contouring missing values
	
	if(ContouringMissing())
	  {
		Add(NFmiEdge(x1,y1,x2,y2,false));
		Add(NFmiEdge(x2,y2,x3,y3,false));
		Add(NFmiEdge(x3,y3,x4,y4,false));
		Add(NFmiEdge(x4,y4,x1,y1,false));
		return;
	  }

	// If the square contains only two different values, we may use
	// the linear interpolation trick. If there are more values,
	// we risk generating gaps between the contours.

	set<float> values;
	values.insert(z1);
	values.insert(z2);
	values.insert(z3);
	values.insert(z4);

	if(values.size() > 2)
	  ContourNearest4(x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4);
	else
	  {
		VertexInsidedness c1 = Insidedness(z1);
		VertexInsidedness c2 = Insidedness(z2);
		VertexInsidedness c3 = Insidedness(z3);
		VertexInsidedness c4 = Insidedness(z4);
		
		// Make new subcontourer
		NFmiContourTree subpath(0.5,1.5,true,true,false);
		subpath.ContourLinear4(x1,y1,c1==kInside ? 1 : 2,
							   x2,y2,c2==kInside ? 1 : 2,
							   x3,y3,c3==kInside ? 1 : 2,
							   x4,y4,c4==kInside ? 1 : 2);

		for(EdgeTreeType::iterator it=subpath.itsEdges.begin();
			it != subpath.itsEdges.end();
			++it)
		  {
			Add(*it);
		  }
	  }

  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour a triangular element. The input is the 3 coordinates defining
   * the triangle, and the values at the triangle vertices. The order
   * of the vertices does not matter, the resulting triangle is always
   * uniquely definined by the vertex coordinates.
   * (This is not the case for a polygon with more vertices)
   *
   * Any of the input values may be missing, which implies the polygon
   * will not cause any contours to be output.
   *
   * This is basically a copy of ContourLinear4, but just for 3 vertices, and
   * with simplified code since there can be no saddle points. Since we'll
   * never need to generalize this for anything but 3 or 4 vertices, we'll
   * keep the subroutines separate for optimal speed. (The alternative
   * would be to assume input vectors define convex polygons, which we would
   * handle with generic loops and so on.)
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear3(float x1, float y1, float z1,
									   float x2, float y2, float z2,
									   float x3, float y3, float z3,
									   int maxdepth)
  {
	// Abort if any of the values is missing. We test z first, since
	// it is most likely that z is missing, not x or y
	
	if(!IsValid(z1) ||
	   !IsValid(z2) ||
	   !IsValid(z3) ||
	   x1==kFloatMissing ||
	   x2==kFloatMissing ||
	   x3==kFloatMissing ||
	   y1==kFloatMissing ||
	   y2==kFloatMissing ||
	   y3==kFloatMissing   )
	  return;
	
	// If both limits are missing, cover the entire triangle, since
	// it contains only valid values now. The contours are never exact
	// when contouring missing values
	
	if(ContouringMissing())
	  {
		Add(NFmiEdge(x1,y1,x2,y2,false));
		Add(NFmiEdge(x2,y2,x3,y3,false));
		Add(NFmiEdge(x3,y3,x1,y1,false));
		return;
	  }
	
	// Establish where the edges reside with respect to the desired range
	// -1 implies below, 0 inside, 1 above. Also note that in the errorneous
	// case of having both limits missing, which should by definition
	// mean range (-infinity,infinity), the default insidedness value is
	// correct.
	
	VertexInsidedness c1 = Insidedness(z1);
	VertexInsidedness c2 = Insidedness(z2);
	VertexInsidedness c3 = Insidedness(z3);
	
	// If all points are in the same region, no recursion is needed
	
	if(c1==c2 && c2==c3)
	  {
		// If above or below, nothing to do
		
		if(c1 != kInside)		// here c1=c2=c3
		  return;
		
		// Otherwise all points are inside, all we need to know is whether
		// the rectangle edges are exact or not. An edge is exact, if the
		// values on the edge are exactly equal to one of the range limits.
		// The edges must be recursed to full depth though to guarantee
		// matching line segments!
		
		Add(NFmiEdge(x1,y1,x2,y2,z1==z2 && (z1==LoLimit() || z1==HiLimit())));
		Add(NFmiEdge(x2,y2,x3,y3,z2==z3 && (z2==LoLimit() || z2==HiLimit())));
		Add(NFmiEdge(x3,y3,x1,y1,z3==z1 && (z3==LoLimit() || z3==HiLimit())));
		return;
	  }
	
	// Now, if we want extra smoothness via recursion, we do it here
	
	if(maxdepth>0)
	  {
		ContourLinear3Recurse(x1,y1,z1,x2,y2,z2,x3,y3,z3,maxdepth-1);
		return;
	  }
	
	// Here we know for certain that there are no ambiguous areas when
	// deciding what areas to 'fill'. Due to the nature of the algorithm
	// it is sufficient to simply enumerate all the edges surrounding
	// the inside area, the order is immaterial just as long as the set
	// of edges forms a closed area. This is clear by enumerating
	// all possible combinations, and studying what areas will be coloured.
	//
	// Hence we can simply go around the triangle, enumerating all possible
	// intersection points in order. Connecting these intersection points
	// gives the resulting area.
	
	// Output the desired data into new vectors. Note that each edge
	// may be intersected max 2 times, hence 6 is the maximum size
	// of the vectors.
	
	vector<float> X,Y;
	vector<VertexExactness>  B;
	
	IntersectEdge(X,Y,B, x1,y1,z1,c1, x2,y2,z2,c2);
	IntersectEdge(X,Y,B, x2,y2,z2,c2, x3,y3,z3,c3);
	IntersectEdge(X,Y,B, x3,y3,z3,c3, x1,y1,z1,c1);
	
	// And add the data in the vectors into the tree
	
	AddEdges(X,Y,B);
	
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Handle contour recursion for triangular polygons. The triangle
   * is subdivided into 4 parts by connecting adjacent midpoints of
   * the edges. The input is always valid, \c kFloatMissing never occurs
   * since this is only called from ContourLinear3, where \c kFloatMissing
   * has already been checked.
   *
   * \note
   *	The calculated values are independent of the order of
   *	the input vertices, since (a+b) is independent of the
   *	order of a and b. Hence the code is safe as far as
   *	coordinate equality comparisons from adjacent cells
   *	are concerned.
   *
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourLinear3Recurse(float x1, float y1, float z1,
											  float x2, float y2, float z2,
											  float x3, float y3, float z3,
											  int depth)
  {
	// Edge center coordinates and function values
	
	float x12 = (x1+x2)/2;
	float x23 = (x2+x3)/2;
	float x31 = (x3+x1)/2;
	
	float y12 = (y1+y2)/2;
	float y23 = (y2+y3)/2;
	float y31 = (y3+y1)/2;
	
	float z12 = (z1+z2)/2;
	float z23 = (z2+z3)/2;
	float z31 = (z3+z1)/2;
	
	ContourLinear3(x1,y1,z1,x12,y12,z12,x31,y31,z31,depth);
	ContourLinear3(x31,y31,z31,x23,y23,z23,x3,y3,z3,depth);
	ContourLinear3(x12,y12,z12,x2,y2,z2,x23,y23,z23,depth);
	ContourLinear3(x31,y31,z31,x12,y12,z12,x23,y23,z23,depth);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Contour a triangular element using nearest neighbour interpolation.
   * The input is the 3 coordinates defining
   * the triangle, and the values at the triangle vertices. The order
   * of the vertices does not matter, the resulting triangle is always
   * uniquely definined by the vertex coordinates.
   * (This is not the case for a polygon with more vertices)
   *
   * Any of the input values may be missing, which implies the polygon
   * will not cause any contours to be output.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourNearest3(float x1, float y1, float z1,
										float x2, float y2, float z2,
										float x3, float y3, float z3)
  {
	// Abort if any of the values is missing. We test z first, since
	// it is most likely that z is missing, not x or y
	
	if(!IsValid(z1) ||
	   !IsValid(z2) ||
	   !IsValid(z3) ||
	   x1==kFloatMissing ||
	   x2==kFloatMissing ||
	   x3==kFloatMissing ||
	   y1==kFloatMissing ||
	   y2==kFloatMissing ||
	   y3==kFloatMissing   )
	  return;
	
	// If both limits are missing, cover the entire triangle, since
	// it contains only valid values now. The contours are never exact
	// when contouring missing values
	
	if(ContouringMissing())
	  {
		Add(NFmiEdge(x1,y1,x2,y2,false));
		Add(NFmiEdge(x2,y2,x3,y3,false));
		Add(NFmiEdge(x3,y3,x1,y1,false));
		return;
	  }
	
	// Establish where the edges reside with respect to the desired range
	// -1 implies below, 0 inside, 1 above. Also note that in the errorneous
	// case of having both limits missing, which should by definition
	// mean range (-infinity,infinity), the default insidedness value is
	// correct.
	
	VertexInsidedness c1 = Insidedness(z1);
	VertexInsidedness c2 = Insidedness(z2);
	VertexInsidedness c3 = Insidedness(z3);
	
	// Handle the rectangular areas nearest to each corner separately
	
	// Edge center coordinates
	
	float x12 = (x1+x2)/2;
	float x23 = (x2+x3)/2;
	float x31 = (x3+x1)/2;
	
	float y12 = (y1+y2)/2;
	float y23 = (y2+y3)/2;
	float y31 = (y3+y1)/2;
	
	// Triangle center coordinates
	
	float x0 = (x1+x2+x3)/3;
	float y0 = (y1+y2+y3)/3;
	
	if(c1==kInside)
	  {
		Add(NFmiEdge(x31,y31,x1,y1,true));
		Add(NFmiEdge(x1,y1,x12,y12,true));
	  }
	if(c2==kInside)
	  {
		Add(NFmiEdge(x12,y12,x2,y2,true));
		Add(NFmiEdge(x2,y2,x23,y23,true));
	  }
	if(c3==kInside)
	  {
		Add(NFmiEdge(x23,y23,x3,y3,true));
		Add(NFmiEdge(x3,y3,x31,y31,true));
	  }
	if( (c1==kInside) ^ (c2==kInside) )
	  Add(NFmiEdge(x12,y12,x0,y0,true));
	if( (c2==kInside) ^ (c3==kInside) )
	  Add(NFmiEdge(x23,y23,x0,y0,true));
	if( (c3==kInside) ^ (c1==kInside) )
	  Add(NFmiEdge(x31,y31,x0,y0,true));
	
  }

  // ----------------------------------------------------------------------
  /*!
   * Contour a triangular element using discrete interpolation.
   * The input is the 3 coordinates defining
   * the triangle, and the values at the triangle vertices. The order
   * of the vertices does not matter, the resulting triangle is always
   * uniquely definined by the vertex coordinates.
   * (This is not the case for a polygon with more vertices)
   *
   * Any of the input values may be missing, which implies the polygon
   * will not cause any contours to be output.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::ContourDiscrete3(float x1, float y1, float z1,
										 float x2, float y2, float z2,
										 float x3, float y3, float z3)
  {
	// The results are equivalent!
	NFmiContourTree::ContourNearest3(x1,y1,z1,x2,y2,z2,x3,y3,z3);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Handle the possible intersections at a single edge.
   * The input is vectors containing intersections so far,
   * and the coordinates and values at the edge to be studied.
   *
   * The inverse interpolation formula is obtained as follows:
   *
   * The edge is defined as a line as follows:
   *
   *	\f$ {x = x_1 + s (x_2 - x_1)} \atop {y = y_1 + s(y_2-y_11)}\f$
   *
   * where s between 0 and 1. The respective function values are
   * interpolated with
   *
   *	\f$ z = z_1 + s(z_2-z_1) \f$
   *
   * Given some specific desired value z, we solve s from the last
   * equation to be
   *
   *	\f$ s = (z-z_1)/(z_2-z_1) \f$
   *
   * which can be inserted into the first two formulas to get the
   * respective values of x and y.
   *
   * \note
   *	It is crucial that the calculation of the intersection
   *	points is independent of the order of the end points
   *	of the line segment being intersected. Otherwise there
   *	will be very small numerical differences between the
   *	intersection points calculated from adjacent cells.
   *	This causes duplicate detection to be much harder.
   *	A simple way to handle the problem is to sort the
   *	edges for numerical calculations, but use the original
   *	order when deciding output order of the intersected
   *	points.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::IntersectEdge(vector<float> & X,
									  vector<float> & Y,
									  vector<VertexExactness> & B,
									  float x1, float y1, float z1,
									  VertexInsidedness c1,
									  float x2, float y2, float z2,
									  VertexInsidedness c2
									  )
  {
	// Handle case of no intersections
	
	if(c1==c2 && c1!=kInside)	// That is, c1==c2==kBelow/kAbove
	  return;
	
	// The sorted values for intersection calculations:
	
	float xx1,yy1,zz1;
	float xx2,yy2,zz2;
	
	if(x1<x2 || (x1==x2 && y1<y2) )
	  {
		xx1=x1; yy1=y1; zz1=z1;
		xx2=x2; yy2=y2; zz2=z2;
	  }
	else
	  {
		xx1=x2; yy1=y2; zz1=z2;
		xx2=x1; yy2=y1; zz2=z1;
	  }
	
	// The intersection coordinates
	
	float slo, shi;
	float xlo, ylo, xhi, yhi;
	
	if(c1==kBelow || c2==kBelow)
	  {
		slo = (LoLimit()-zz1)/(zz2-zz1);
		xlo = xx1+slo*(xx2-xx1);
		ylo = yy1+slo*(yy2-yy1);
	  }
	if(c1==kAbove || c2==kAbove)
	  {
		shi = (HiLimit()-zz1)/(zz2-zz1);
		xhi = xx1+shi*(xx2-xx1);
		yhi = yy1+shi*(yy2-yy1);
	  }
	
	switch(c1)
	  {
	  case kBelow:
		X.push_back(xlo);
		Y.push_back(ylo);
		B.push_back(kLoLimit);
		break;
	  case kInside:
		X.push_back(x1);
		Y.push_back(y1);
		B.push_back(Exactness(z1));
		break;
	  case kAbove:
		X.push_back(xhi);
		Y.push_back(yhi);
		B.push_back(kHiLimit);
		break;
	  }
	switch(c2)
	  {
	  case kBelow:
		X.push_back(xlo);
		Y.push_back(ylo);
		B.push_back(kLoLimit);
		break;
	  case kInside:
		X.push_back(x2);
		Y.push_back(y2);
		B.push_back(Exactness(z2));
		break;
	  case kAbove:
		X.push_back(xhi);
		Y.push_back(yhi);
		B.push_back(kHiLimit);
		break;
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Add the edges formed by a set of points into the tree. The points
   * always form a closed polygon.
   *
   * \note An edge is a ghostline if it is not an exact edge.
   *       An edge is exact, if the function value at both end points
   *	 is equal to one of the limits. In terms of exactness, the
   *	 exactness must be the same so that the limit is the same,
   *	 but not kNeither so that a limit is actually occurring.
   */
  // ----------------------------------------------------------------------
  
  void NFmiContourTree::AddEdges(const vector<float> & X,
								 const vector<float> & Y,
								 const vector<VertexExactness> & B
								 )
  {
	for(unsigned int i=0; i<X.size(); i++)
	  {
		unsigned int j = (i+1) % X.size();
		Add(NFmiEdge(X[i],Y[i],X[j],Y[j],
					 B[i]==B[j] && B[i]!=kNeither));
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Convert a contour interpolation method name to a method enum
   * Returns kFmiContourMissingInterpolation for unrecognized name
   */
  // ----------------------------------------------------------------------
  
  NFmiContourTree::NFmiContourInterpolation NFmiContourTree::ContourInterpolationValue(const string & theName)
  {
	if(theName=="Nearest")
	  return kFmiContourNearest;
	else if(theName=="Linear")
	  return kFmiContourLinear;
	else if(theName=="Discrete")
	  return kFmiContourDiscrete;
	else
	  return kFmiContourMissingInterpolation;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * Convert a contour interpolation method enum to a method name
   * Returns a "Missing" if the name is unknown
   */
  // ----------------------------------------------------------------------
  
  const string NFmiContourTree::ContourInterpolationName
  (NFmiContourTree::NFmiContourInterpolation theInterpolation)
  {
	switch(theInterpolation)
	  {
	  case kFmiContourNearest:
		return string("Nearest");
	  case kFmiContourLinear:
		return string("Linear");
	  case kFmiContourDiscrete:
		return string("Discrete");
	  default:
		return string("Missing");
	  }
  }

} // namespace Imagine
  
// ----------------------------------------------------------------------
  
