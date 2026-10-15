// ======================================================================
/*!
 * \brief Interface of class NFmiDataMatrixInfo
 */
// ======================================================================

#pragma once

#include <newbase/NFmiDataMatrix.h>

#include <memory>
#include <list>

namespace Imagine
{
// ----------------------------------------------------------------------
/*!
 * \brief Interface of class NFmiDataHints
 */
// ----------------------------------------------------------------------

class NFmiDataHints
{
 public:
  struct Rectangle
  {
    int x1;
    int y1;
    int x2;
    int y2;
    float minimum;
    float maximum;
    bool hasmissing;
  };

  using return_type = std::list<Rectangle>;

  ~NFmiDataHints();

  // Max subgrid size is 10x10
  NFmiDataHints(const NFmiDataMatrix<float>& theData, int theMaxSize = 10);

  return_type rectangles(float theLoLimit, float theHiLimit) const;

 private:
  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

  NFmiDataHints() = delete;
  NFmiDataHints& operator=(const NFmiDataHints& theMatrix) = delete;
  NFmiDataHints(const NFmiDataHints& theMatrix) = delete;
};

}  // namespace Imagine

// ======================================================================
