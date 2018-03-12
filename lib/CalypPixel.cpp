/*    This file is a part of Calyp project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     CalypFrame.cpp
 * \brief    Video Frame handling
 */

#include "CalypFrame.h"

#include "PixelFormats.h"
#include "config.h"

#define MAX_NUMBER_COMPONENTS 4

struct CalypPixelPrivate
{
  CalypPixelPrivate( const int& ColorSpace )
  {
    iColorSpace = ColorSpace == CLP_COLOR_GRAY ?
                      CLP_COLOR_YUV :
                      ColorSpace;
    for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
    {
      PelComp[i] = 0;
    }
  }

  int iColorSpace;
  ClpPel PelComp[MAX_NUMBER_COMPONENTS];
};

static inline void yuvToRgb( const int& iY, const int& iU, const int& iV, int& iR, int& iG, int& iB )
{
  iR = iY + ( ( 1436 * ( iV - 128 ) ) >> 10 );
  iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 );
  iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 );
  iR = iR < 0 ? 0 : iR > 255 ? 255 : iR;
  iG = iG < 0 ? 0 : iG > 255 ? 255 : iG;
  iB = iB < 0 ? 0 : iB > 255 ? 255 : iB;
}

static inline void rgbToYuv( const int& iR, const int& iG, const int& iB, int& iY, int& iU, int& iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

int CalypPixel::getMaxNumberOfComponents()
{
  return MAX_NUMBER_COMPONENTS;
}

CalypPixel::CalypPixel( const int& ColorSpace )
    : d( new CalypPixelPrivate( ColorSpace ) )
{
}

CalypPixel::CalypPixel( const int& ColorSpace, const ClpPel& c0 )
    : d( new CalypPixelPrivate( ColorSpace ) )
{
  d->PelComp[0] = c0;
}

CalypPixel::CalypPixel( const int& ColorSpace, const ClpPel& c0, const ClpPel& c1, const ClpPel& c2 )
    : d( new CalypPixelPrivate( ColorSpace ) )
{
  d->PelComp[0] = c0;
  d->PelComp[1] = c1;
  d->PelComp[2] = c2;
}

CalypPixel::CalypPixel( const int& ColorSpace, const ClpPel& c0, const ClpPel& c1, const ClpPel& c2, const ClpPel& c3 )
    : d( new CalypPixelPrivate( ColorSpace ) )
{
  d->PelComp[0] = c0;
  d->PelComp[1] = c1;
  d->PelComp[2] = c2;
  d->PelComp[3] = c3;
}

CalypPixel::CalypPixel( const CalypPixel& other )
    : d( new CalypPixelPrivate( other.colorSpace() ) )
{
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PelComp[i] = other[i];
  }
}

CalypPixel::~CalypPixel()
{
  delete d;
}

int CalypPixel::colorSpace() const
{
  return d->iColorSpace;
}

ClpPel CalypPixel::operator[]( const int& channel ) const
{
  return d->PelComp[channel];
}
ClpPel& CalypPixel::operator[]( const int& channel )
{
  return d->PelComp[channel];
}

CalypPixel CalypPixel::operator=( const CalypPixel& other )
{
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PelComp[i] = other[i];
  }
  return *this;
}

CalypPixel CalypPixel::operator+( const CalypPixel& in )
{
  CalypPixel result;
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    result[i] = d->PelComp[i] + in[i];
  }
  return result;
}

CalypPixel CalypPixel::operator+=( const CalypPixel& in )
{
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PelComp[i] += in[i];
  }
  return *this;
}

CalypPixel CalypPixel::operator-( const CalypPixel& in )
{
  CalypPixel result;
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    result[i] = d->PelComp[i] - in[i];
  }
  return result;
}

CalypPixel CalypPixel::operator-=( const CalypPixel& in )
{
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PelComp[i] -= in[i];
  }
  return *this;
}

CalypPixel CalypPixel::operator*( const double& op )
{
  CalypPixel result;
  for( int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    result[i] = d->PelComp[i] * op;
  }
  return result;
}

CalypPixel CalypPixel::convertPixel( CalypColorSpace eOutputSpace )
{
  if( d->iColorSpace == eOutputSpace )
    return *this;

  int outA = 0;
  int outB = 0;
  int outC = 0;
  int outD = 0;
  if( d->iColorSpace == CLP_COLOR_YUV )
  {
    switch( eOutputSpace )
    {
    case CLP_COLOR_GRAY:
      outA = d->PelComp[0];
      break;
    case CLP_COLOR_RGBA:
      outD = 255;
    case CLP_COLOR_RGB:
      yuvToRgb( d->PelComp[0], d->PelComp[1], d->PelComp[2], outA, outB, outC );
      break;
    default:
      break;
    }
  }
  if( d->iColorSpace == CLP_COLOR_RGB )
  {
    switch( eOutputSpace )
    {
    case CLP_COLOR_GRAY:
      rgbToYuv( d->PelComp[0], d->PelComp[1], d->PelComp[2], outA, outB, outC );
    case CLP_COLOR_YUV:
      rgbToYuv( d->PelComp[0], d->PelComp[1], d->PelComp[2], outA, outB, outC );
      break;
    case CLP_COLOR_RGBA:
      outD = 255;
      break;
    default:
      break;
    }
  }
  return CalypPixel( eOutputSpace, outA, outB, outC, outD );
}
