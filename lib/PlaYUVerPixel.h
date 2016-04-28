/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
 *                                Joao Carreira   (jfmcarreira@gmail.com)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file     PlaYUVerPixel.h
 * \ingroup  PlaYUVerLib
 * \brief    Pixel handling
 */

#ifndef __PLAYUVERPIXEL_H__
#define __PLAYUVERPIXEL_H__

#include <iostream>
#include <cstdio>
#include <cassert>
#include <vector>
#include "PlaYUVerDefs.h"

namespace plaYUVer
{

#define CLAMP_YUV2RGB(X) X = X < 0 ? 0 : X > 255 ? 255 : X;
#define YUV2RGB(  iY, iU, iV, iR, iG, iB ) \
    iR = iY + ( ( 1436 * ( iV - 128) ) >> 10 ); \
    iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 ); \
    iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 ); \
    CLAMP_YUV2RGB(iR) CLAMP_YUV2RGB(iG) CLAMP_YUV2RGB(iB)


static inline Void rgbToYuv( Int iR, Int iG, Int iB, Int &iY, Int &iU, Int &iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

#define MAX_NUMBER_COMPONENTS 4

/**
  * \class    PlaYUVerPixel
  * \ingroup  PlaYUVerLib PlaYUVerLib_Frame
  * \brief    Pel handling class
  */
class PlaYUVerPixel
{
public:
  /** ColorSpace Enum
  * List of supported color spaces
  */
  enum ColorSpace
  {
    COLOR_INVALID = -1,  //!< Invalid
    COLOR_YUV = 0,   //!< YUV
    COLOR_RGB = 1,   //!< RGB
    COLOR_GRAY = 2,  //!< Grayscale
    COLOR_ARGB = 3,  //!< RGB + Alpha
  };

  PlaYUVerPixel();
  PlaYUVerPixel( Int ColorSpace );
  PlaYUVerPixel( Int ColorSpace, Pel c0, Pel c1, Pel c2 );

  Int ColorSpaceType()
  {
    return m_iColorSpace;
  }
  Pel* Components()
  {
    return PixelComponents;
  }
  Pel Y() const
  {
    return PixelComponents[0];
  }
  Pel Cb() const
  {
    return PixelComponents[1];
  }
  Pel Cr() const
  {
    return PixelComponents[2];
  }
  Pel& Y()
  {
    return PixelComponents[0];
  }
  Pel& Cb()
  {
    return PixelComponents[1];
  }
  Pel& Cr()
  {
    return PixelComponents[2];
  }
  Pel& R()
  {
    return PixelComponents[0];
  }
  Pel& G()
  {
    return PixelComponents[1];
  }
  Pel& B()
  {
    return PixelComponents[2];
  }
  Pel& A()
  {
    return PixelComponents[3];
  }

  PlaYUVerPixel operator+ ( const PlaYUVerPixel& );
  PlaYUVerPixel operator- ( const PlaYUVerPixel& );
  PlaYUVerPixel operator* ( const Double& );

  /**
   * Convert a Pixel to a new color space
   * @param inputPixel input pixel (PlaYUVerPixel)
   * @param eOutputSpace output color space
   * @return converted pixel
   */
  PlaYUVerPixel ConvertPixel( ColorSpace eOutputSpace );

private:
  Int m_iColorSpace;
  Pel PixelComponents[MAX_NUMBER_COMPONENTS];

};

}  // NAMESPACE

#endif // __PLAYUVERPIXEL_H__
