// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Imagine::NFmiColorReduce
 */
// ======================================================================
/*!
 * \namespace Imagine::NFmiColorReduce
 *
 * \brief Tools for reducing the number of colors in an image
 *
 * The main idea in reducing colors in an image is to make it
 * smaller so that the image can be downloaded faster. Typically
 * one sets a maximum number of allowed colors, but often a better
 * alternative might be to reduce colors until the error would become
 * too great.
 *
 */
// ======================================================================

#include "NFmiColorReduce.h"
#include "NFmiImage.h"
#include "NFmiColorTools.h"
#include "NFmiNearTree.h"

#include <list>
#include <map>
#include <vector>

using namespace std;

// The actual public interfaces

namespace Imagine
{

  // ======================================================================
  // Local subroutines
  // ======================================================================

  namespace
  {

	//! Histogram information

	typedef map<NFmiColorTools::Color,int> Counter;

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the occurrance count of each color in the given image
	 *
	 * \param theImage The image
	 * \return The colormap with occurrance counts
	 */
	// ----------------------------------------------------------------------

	const Counter calc_counts(const NFmiImage & theImage)
	{
	  Counter counter;
	  Counter::iterator it = counter.end();
	  
	  for(int j=0; j<theImage.Height(); j++)
		for(int i=0; i<theImage.Width(); i++)
		  {
			NFmiColorTools::Color color = theImage(i,j);
			
			if(it == counter.end() || it->first != color)
			  {
				pair<Counter::iterator,bool> result = counter.insert(Counter::value_type(color,0));
				it = result.first;
			  }
			it->second++;
		  }
	  
	  return counter;

	}

	// -----------------------------------------------------------------------
	/*!
	 * \brief Structure containing color reduction information
	 */
	// ----------------------------------------------------------------------
	
	const float MAXERROR = 100000000;
  
	struct ColorInfo
	{
	  NFmiColorTools::Color color;
	  float ratio;		// percentage 0-1

	  float error;			// error estimate to nearest chosen color
	  int nearest;		// index of nearest chosen color
	  
	  ColorInfo(NFmiColorTools::Color theColor, float theRatio)
		: color(theColor)
		, ratio(theRatio)
		, error(MAXERROR)
		, nearest(-1)
	  { }

	}; // struct ColorInfo

	// ----------------------------------------------------------------------
	/*!
	 * \brief Initialize the given gamma correction table
	 */
	// ----------------------------------------------------------------------
	
	void init_gamma_table(vector<float> & theTable, bool & theFlag)
	{
	  const float gamma = 2.2;
	  const float coeff = 255/(pow(255,gamma));
	  
	  for(int i=0; i<256; i++)
		theTable[i] = coeff*pow(i,gamma);
	  
	  theFlag = true;

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate difference between two colors
	 *
	 * The code is from http://www.compuphase.com/cmetric.htm with
	 * an extra term added for alpha difference and more weigth given
	 * for the red color.
	 */
	// ----------------------------------------------------------------------
	
	inline
	float color_error(int r1, int g1, int b1, int a1,
					  NFmiColorTools::Color color)
	{
	  static vector<float> gamma_table(256,0);
	  static bool initialized = false;
	  
	  if(!initialized) init_gamma_table(gamma_table,initialized);
	  
	  float r2 = gamma_table[NFmiColorTools::GetRed(color)];
	  float g2 = gamma_table[NFmiColorTools::GetGreen(color)];
	  float b2 = gamma_table[NFmiColorTools::GetBlue(color)];
	  float a2 = gamma_table[NFmiColorTools::GetAlpha(color)];
	  
	  float rmean = (gamma_table[r1]+r2)/2;
	  float r = gamma_table[r1]-r2;
	  float g = gamma_table[g1]-g2;
	  float b = gamma_table[b1]-b2;
	  float a = gamma_table[a1]-a2;

	  return sqrt((((512+rmean)*r*r)/256) + 4*g*g + (((767-rmean)*b*b)/256) + a*a);

	}


  } // namespace anonymous


  // ======================================================================
  // The actual public interfaces
  // ======================================================================

  namespace NFmiColorReduce
  {
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the occurrance count of each color in the given image
	 *
	 * \param theImage The image
	 * \return The histogram object
	 */
	// ----------------------------------------------------------------------
	
	const Histogram CalcHistogram(const NFmiImage & theImage)
	{
	  Counter counter = calc_counts(theImage);
	  
	  Histogram histogram;
	  
	  for(Counter::const_iterator it = counter.begin();
		  it!=counter.end();
		  ++it)
		{
		  histogram.insert(Histogram::value_type(it->second,it->first));
		}
	  
	  return histogram;
	
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Reduce colors from the image adaptively
	 *
	 * Algorithm:
	 *
	 *   -# Calculate the occurrance count of each color
	 *   -# Choose colors one at a time until error is small enough
	 *
	 * The original selection algorithm used in gifsicle and xv is as follows:
	 *
	 *   -# Choose color 1 to be the most popular color
	 *   -# Choose next most popular colors until the associated
	 *      percentage drops below 1%
	 *   -# Choose at alternate turns either the most diverse color or the next
	 *      most popular color
	 *
	 * Both xv and gifsicle terminate when a preset number of colors have
	 * been chosen. However, we prefer the limit to be adaptive based
	 * on actual error of the chosen color.
	 *
	 * Our algorithm is:
	 *
	 *  -# Find the most diverse color
	 *  -# From a given radius from the diverse color, choose the most
	 *     popular one
	 *
	 * Also, we use the color distance formula given at
	 * http://www.compuphase.com/cmetric.htm
	 * to improve finding the diverse color.
	 *
	 * The first step is essential for preserving the color of smallish
	 * patches of color, since any changes in their color in animation
	 * are noticeable. Changes in isolated pixels are much less noticeable.
	 *
	 * The radius may or may not match the desired maximum error used in the
	 * termination criteria.
	 *
	 * \param theImage The image to modify
	 * \param theMaxError The allowed maximum error
	 */
	// ----------------------------------------------------------------------
	
	void AdaptiveReduce(NFmiImage & theImage, float theMaxError)
	{
	  using namespace Imagine::NFmiColorTools;
	  
	  // A sanity check on the maximum error
	  
	  if(theMaxError < 1)
		return;
	  
	  // Calculate the histogram
	  
	  Histogram histogram = CalcHistogram(theImage);
	  
	  // Start choosing
	  
	  typedef vector<ColorInfo> ChosenColors;
	  typedef list<ColorInfo> ColorList;
	  
	  ChosenColors chosen_colors;
	  ColorList todo_colors;
	  ColorList close_colors;
	  
	  // Populate the todo-colors
	  
	  const float npixels = static_cast<float>(theImage.Width()*theImage.Height());
	  
	  for(Histogram::const_iterator it = histogram.begin();
		  it != histogram.end();
		  ++it)
		{
		  todo_colors.push_back(ColorInfo(it->second,it->first/npixels));
		}
	  
	  // Choose the first color
	  
	  ColorInfo color = todo_colors.front();
	  color.nearest = 0;
	  chosen_colors.push_back(color);
	  todo_colors.pop_front();
	  
	  while(!todo_colors.empty())
		{
		  int red   = GetRed(color.color);
		  int green = GetGreen(color.color);
		  int blue  = GetBlue(color.color);
		  int alpha = GetAlpha(color.color);
		  
		  ColorList::iterator chosen = todo_colors.end();
		  float maxerror = 0;		// maximum true error remaining
		  
		  // Recalculate the errors, and simultaneously find
		  // the most errorneous color
		  
		  for(ColorList::iterator it = todo_colors.begin();
			  it != todo_colors.end();
			  )
			{
			  // Error compared to chosen color
			  
			  const float err = color_error(red,green,blue,alpha,it->color);
			  
			  // If it is smaller than the previously calculated error,
			  // update the information on the nearest color
			  
			  if(err < it->error)
				{
				  it->error = err;
				  it->nearest = chosen_colors.size()-1;

				  // Move the color away from the todo-list if the
				  // remaining error is small

				  if(err < theMaxError)
					{
					  close_colors.push_back(*it);
					  ColorList::iterator tmp = it;
					  ++it;
					  todo_colors.erase(tmp);
					}
				  else
					{
					  if(it->error > maxerror)
						{
						  chosen = it;
						  maxerror = it->error;
						}
					  ++it;
					}
				}
			  else
				++it;

			}

		  // If the next todo-color is popular enough, choose it
		  // no matter what
		  
		  if(todo_colors.front().ratio > 0.01)
			{
			  chosen = todo_colors.begin();
			}
		  else
			{
			  // Now, if the maximum error is small enough, we're done
			  
			  if(maxerror < theMaxError)
				break;
			  
			  // Otherwise find the most popular color in the neighbourhood
			  // of the most errorneous color. However, we ignore any colors
			  // which are closer to a selected color than to the diverse color.
			  // This aids in getting diverse colors representation in the colormap
			  
			  ColorList::iterator best = chosen;
			  red   = GetRed(chosen->color);
			  green = GetGreen(chosen->color);
			  blue  = GetBlue(chosen->color);
			  alpha = GetAlpha(chosen->color);
			  
			  for(ColorList::iterator jt = todo_colors.begin();
				  jt != todo_colors.end();
				  ++jt)
				{
				  float err = color_error(red,green,blue,alpha,jt->color);
				  
				  if(err < theMaxError &&
					 jt->ratio > best->ratio &&
					 jt->error > 0.5*err) // jt->error > 0.5*theMaxError)
					{
					  best = jt;
					}
				}
			  
			  // Choose the best one
			  
			  chosen = best;
			}
		  
		  color = *chosen;
		  color.nearest = chosen_colors.size();
		  chosen_colors.push_back(color);
		  todo_colors.erase(chosen);

		}
	  
	  // Now fix the colors in the image. To help us we build a map
	  // with colors as key and the respective chosen color index
	  // as the value
	  
	  typedef map<Color,int> ColorMap;
	  ColorMap colormap;
	  
	  for(ChosenColors::const_iterator jt = chosen_colors.begin();
		  jt != chosen_colors.end();
		  ++jt)
		{
		  colormap.insert(ColorMap::value_type(jt->color,jt->nearest));
		}

	  for(ColorList::const_iterator lt = close_colors.begin();
		  lt != close_colors.end();
		  ++lt)
		{
		  colormap.insert(ColorMap::value_type(lt->color,lt->nearest));
		}
	  
	  for(ColorList::const_iterator kt = todo_colors.begin();
		  kt != todo_colors.end();
		  ++kt)
		{
		  colormap.insert(ColorMap::value_type(kt->color,kt->nearest));
		}
	  
	  // Now for each image pixel, find the associated colormap index
	  // and replace the original color with chosen color

	  NFmiColorTools::Color last_color = NFmiColorTools::NoColor;
	  NFmiColorTools::Color last_choice = NFmiColorTools::NoColor;
	  
	  for(int j=0; j<theImage.Height(); j++)
		for(int i=0; i<theImage.Width(); i++)
		  {
			if(theImage(i,j) != last_color)
			  {
				ColorMap::const_iterator cit = colormap.find(theImage(i,j));
				if(cit == colormap.end())
				  throw runtime_error("Internal error in color reduction, failed to find color");
				last_color = theImage(i,j);
				last_choice = chosen_colors[cit->second].color;
			  }
			theImage(i,j) = last_choice;
		  }
	  
	}
	
  } // namespace NFmiColorReduce
} // namespace Imagine

// ======================================================================

