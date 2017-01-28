/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \brief    Pixel handling
 */

#include <cstdio>
#include <cmath>
#include "PlaYUVerFrame.h"
#include "PlaYUVerPixel.h"

PlaYUVerPixel::PlaYUVerPixel() : m_iColorSpace( COLOR_INVALID )
{
  m_iColorSpace = COLOR_INVALID;
  PixelComponents[0] = 0;
  PixelComponents[1] = 0;
  PixelComponents[2] = 0;
  PixelComponents[3] = 0;
}

PlaYUVerPixel::PlaYUVerPixel( const Int& ColorSpace )
{
  m_iColorSpace = ColorSpace == COLOR_GRAY ? COLOR_YUV : ColorSpace;
  PixelComponents[0] = 0;
  PixelComponents[1] = 0;
  PixelComponents[2] = 0;
  PixelComponents[3] = 0;
}

PlaYUVerPixel::PlaYUVerPixel( const Int& ColorSpace, const Pel& c0, const Pel& c1, const Pel& c2 )
{
  m_iColorSpace = ColorSpace == COLOR_GRAY ? COLOR_YUV : ColorSpace;
  PixelComponents[0] = c0;
  PixelComponents[1] = c1;
  PixelComponents[2] = c2;
  PixelComponents[3] = 0;
}

PlaYUVerPixel::~PlaYUVerPixel()
{
}

Int PlaYUVerPixel::ColorSpaceType()
{
  return m_iColorSpace;
}

Pel* PlaYUVerPixel::Components()
{
  return PixelComponents;
}

Pel PlaYUVerPixel::Y() const
{
  return PixelComponents[0];
}
Pel PlaYUVerPixel::Cb() const
{
  return PixelComponents[1];
}
Pel PlaYUVerPixel::Cr() const
{
  return PixelComponents[2];
}

Pel& PlaYUVerPixel::Y()
{
  return PixelComponents[0];
}
Pel& PlaYUVerPixel::Cb()
{
  return PixelComponents[1];
}
Pel& PlaYUVerPixel::Cr()
{
  return PixelComponents[2];
}
Pel& PlaYUVerPixel::R()
{
  return PixelComponents[0];
}
Pel& PlaYUVerPixel::G()
{
  return PixelComponents[1];
}
Pel& PlaYUVerPixel::B()
{
  return PixelComponents[2];
}

Pel PlaYUVerPixel::operator[]( const Int& channel ) const
{
  return PixelComponents[channel];
}
Pel& PlaYUVerPixel::operator[]( const Int& channel )
{
  return PixelComponents[channel];
}

PlaYUVerPixel PlaYUVerPixel::operator+( const PlaYUVerPixel& in )
{
  PlaYUVerPixel out( m_iColorSpace, Y() + in.Y(), Cb() + in.Cb(), Cr() + in.Cr() );
  return out;
}

PlaYUVerPixel PlaYUVerPixel::operator-( const PlaYUVerPixel& in )
{
  PlaYUVerPixel out( m_iColorSpace, Y() - in.Y(), Cb() - in.Cb(), Cr() - in.Cr() );
  return out;
}

PlaYUVerPixel PlaYUVerPixel::operator*( const Double& op )
{
  PlaYUVerPixel out( m_iColorSpace, Y() * op, Cb() * op, Cr() * op );
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
    YuvToRgb( Y(), Cb(), Cr(), outA, outB, outC );
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
