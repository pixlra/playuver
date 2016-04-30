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
 * \file     PlaYUVerPixel.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Pixel handling
 */

#include <cstdio>
#include <cmath>
#include "PlaYUVerFrame.h"
#include "PlaYUVerPixel.h"

namespace plaYUVer
{

PlaYUVerPixel::PlaYUVerPixel()
{
  m_iColorSpace = COLOR_INVALID;
  PixelComponents[0] = 0;
  PixelComponents[1] = 0;
  PixelComponents[2] = 0;
  PixelComponents[3] = 0;
}

PlaYUVerPixel::PlaYUVerPixel( Int ColorSpace )
{
  m_iColorSpace = ColorSpace == COLOR_GRAY ? COLOR_YUV : ColorSpace;
  PixelComponents[0] = 0;
  PixelComponents[1] = 0;
  PixelComponents[2] = 0;
  PixelComponents[3] = 0;
}

PlaYUVerPixel::PlaYUVerPixel( Int ColorSpace, Pel c0, Pel c1, Pel c2 )
{
  m_iColorSpace = ColorSpace == COLOR_GRAY ? COLOR_YUV : ColorSpace;
  PixelComponents[0] = c0;
  PixelComponents[1] = c1;
  PixelComponents[2] = c2;
  PixelComponents[3] = 0;
}

PlaYUVerPixel PlaYUVerPixel::operator+ ( const PlaYUVerPixel& in )
{
  PlaYUVerPixel out( m_iColorSpace, Y() + in.Y(), Cb() + in.Cb(), Cr() + in.Cr() );
  return out;
}

PlaYUVerPixel PlaYUVerPixel::operator- ( const PlaYUVerPixel& in )
{
  PlaYUVerPixel out( m_iColorSpace, Y() - in.Y(), Cb() - in.Cb(), Cr() - in.Cr() );
  return out;
}

PlaYUVerPixel PlaYUVerPixel::operator* ( const Double& d )
{
  PlaYUVerPixel out( m_iColorSpace, Y() * d, Cb() * d, Cr() * d );
  return out;
}

PlaYUVerPixel PlaYUVerPixel::ConvertPixel( ColorSpace eOutputSpace )
{
  Int outA, outB, outC;
  PlaYUVerPixel outPixel( COLOR_INVALID, 0, 0, 0 );

  if( ColorSpace() == eOutputSpace || eOutputSpace == COLOR_ARGB || eOutputSpace == COLOR_GRAY )
    return *this;

  if( eOutputSpace == COLOR_RGB )
  {
    YUV2RGB( Y(), Cb(), Cr(), outA, outB, outC );
    outPixel.R() = outA;
    outPixel.G() = outB;
    outPixel.B() = outC;
  }
  if( eOutputSpace == COLOR_YUV )
  {
    rgbToYuv( R(), G(), B(), outA, outB, outC );
    outPixel.Y() = outA;
    outPixel.Cb() = outB;
    outPixel.Cr() = outC;
  }
  return outPixel;
}

}  // NAMESPACE
