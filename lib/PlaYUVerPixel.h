/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \brief    Pixel handling
 */

#ifndef __PLAYUVERPIXEL_H__
#define __PLAYUVERPIXEL_H__

#include <iostream>
#include <cstdio>
#include <cassert>
#include <vector>
#include "PlaYUVerDefs.h"


static inline Void YuvToRgb( const Int &iY, const Int &iU, const Int &iV, Int &iR, Int &iG, Int &iB )
{
  iR = iY + ( ( 1436 * ( iV - 128) ) >> 10 );
  iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 );
  iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 );
  iR = iR < 0 ? 0 : iR > 255 ? 255 : iR;
  iG = iG < 0 ? 0 : iG > 255 ? 255 : iG;
  iB = iB < 0 ? 0 : iB > 255 ? 255 : iB;
}

static inline Void rgbToYuv( const Int& iR, const Int& iG, const Int& iB, Int &iY, Int &iU, Int &iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

/**
  * \class    PlaYUVerPixel
  * \ingroup  PlaYUVerLib PlaYUVerLib_Frame
  * \brief    Pixel handling class
  */
class PlaYUVerPixel
{
public:

  static const UInt MAX_NUMBER_COMPONENTS = 5;

  /** ColorSpace list
  * List of supported color spaces
  */
  enum ColorSpace
  {
    COLOR_INVALID = -1, //!< Invalid
    COLOR_YUV = 0,      //!< YUV
    COLOR_RGB = 1,      //!< RGB
    COLOR_GRAY = 2,     //!< Grayscale
    COLOR_ARGB = 3,     //!< RGB + Alpha
    COLOR_MAX = 255,    //!< Accouunt for future formats
  };

  PlaYUVerPixel();
  PlaYUVerPixel( const Int& ColorSpace );
  PlaYUVerPixel( const Int& ColorSpace, const Pel& c0, const Pel& c1, const  Pel& c2 );
  ~PlaYUVerPixel();

  Int ColorSpaceType();

  Pel* Components();

  Pel Y() const;
  Pel Cb() const;
  Pel Cr() const;
  Pel R() const;
  Pel G() const;
  Pel B() const;

  Pel& Y();
  Pel& Cb();
  Pel& Cr();
  Pel& R();
  Pel& G();
  Pel& B();

  Pel operator[] ( const Int& channel ) const;
  Pel& operator[] ( const Int& channel );

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

#endif // __PLAYUVERPIXEL_H__
