/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
 * \file     PlaYUVerFrame.cpp
 * \brief    Video Frame handling
 */
#include <cstdio>

#include "PlaYUVerFrame.h"
#include "LibMemAlloc.h"

#include <QImage>

namespace plaYUVer
{

QStringList PlaYUVerFrame::supportedPixelFormatList()
{
  QStringList formats;
  formats << "yuv420" << "yuv400";
  return formats;
}

PlaYUVerFrame::PlaYUVerFrame( UInt width, UInt height, Int pel_format )
{
  m_pppcInputPel[0] = NULL;
  m_pppcInputPel[1] = NULL;
  m_pppcInputPel[2] = NULL;
  m_pppcRGBPel = NULL;

  m_uiWidth = width;
  m_uiHeight = height;
  m_iPixelFormat = pel_format;

  switch( m_iPixelFormat )
  {
  case YUV420:
    get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
    get_mem2Dpel( &( m_pppcInputPel[1] ), m_uiHeight / 2, m_uiWidth / 2 );
    get_mem2Dpel( &( m_pppcInputPel[2] ), m_uiHeight / 2, m_uiWidth / 2 );
    break;
  case YUV422:
    get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
    get_mem2Dpel( &( m_pppcInputPel[1] ), m_uiHeight / 2, m_uiWidth );
    get_mem2Dpel( &( m_pppcInputPel[2] ), m_uiHeight / 2, m_uiWidth );
    break;
  case YUV400:
    get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
    break;
  case RGB:
    get_mem3Dpel( &m_pppcRGBPel, 3, m_uiHeight, m_uiWidth );
    break;
  }
  if( m_iPixelFormat != RGB )
    get_mem3Dpel( &m_pppcRGBPel, 3, m_uiHeight, m_uiWidth );

}

PlaYUVerFrame::~PlaYUVerFrame()
{
  if( m_pppcInputPel[0] )
    free_mem2Dpel( m_pppcInputPel[0] );
  if( m_pppcInputPel[1] )
    free_mem2Dpel( m_pppcInputPel[1] );
  if( m_pppcInputPel[2] )
    free_mem2Dpel( m_pppcInputPel[2] );

  if( m_pppcRGBPel )
    free_mem3Dpel( m_pppcRGBPel );
}

static inline void yuvToRgb( int iY, int iU, int iV, int &iR, int &iG, int &iB )
{
  iR = iY + 1402 * iV / 1000;
  iG = iY - ( 101004 * iU + 209599 * iV ) / 293500;
  iB = iY + 1772 * iU / 1000;

  if( iR < 0 )
    iR = 0;
  if( iG < 0 )
    iG = 0;
  if( iB < 0 )
    iB = 0;

  if( iR > 255 )
    iR = 255;
  if( iG > 255 )
    iG = 255;
  if( iB > 255 )
    iB = 255;
}

Void PlaYUVerFrame::YUV420toRGB()
{
  Int iY, iU, iV, iR, iG, iB;
  for( UInt y = 0; y < m_uiHeight; y += 2 )
  {
    for( UInt x = 0; x < m_uiWidth; x += 2 )
    {
      // Pixel (x, y).

      iY = m_pppcInputPel[0][y][x];
      iU = m_pppcInputPel[1][y >> 1][x >> 1];
      iV = m_pppcInputPel[2][y >> 1][x >> 1];
      iU -= 128;
      iV -= 128;

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y][x] = iR;
      m_pppcRGBPel[1][y][x] = iG;
      m_pppcRGBPel[2][y][x] = iB;

      // Pixel (x+1, y)

      iY = m_pppcInputPel[0][y][x + 1];

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y][x + 1] = iR;
      m_pppcRGBPel[1][y][x + 1] = iG;
      m_pppcRGBPel[2][y][x + 1] = iB;

      // Pixel (x, y+1)

      iY = m_pppcInputPel[0][y + 1][x];

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y + 1][x] = iR;
      m_pppcRGBPel[1][y + 1][x] = iG;
      m_pppcRGBPel[2][y + 1][x] = iB;

      // Pixel (x+1, y+1)

      iY = m_pppcInputPel[0][y + 1][x + 1];

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y + 1][x + 1] = iR;
      m_pppcRGBPel[1][y + 1][x + 1] = iG;
      m_pppcRGBPel[2][y + 1][x + 1] = iB;

    }
  }
}

Void PlaYUVerFrame::FrameFromBuffer( Pel *input_buffer, Int pel_format )
{
  if( m_iPixelFormat != pel_format )
  {
    return;
  }
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight;
  switch( m_iPixelFormat )
  {
  case YUV420:
    memcpy( &m_pppcInputPel[0][0][0], input_buffer, frame_bytes_input * sizeof(Pel) );
    memcpy( &m_pppcInputPel[1][0][0], input_buffer + frame_bytes_input, frame_bytes_input / 4 * sizeof(Pel) );
    memcpy( &m_pppcInputPel[2][0][0], input_buffer + frame_bytes_input * 5 / 4, frame_bytes_input / 4 * sizeof(Pel) );
    //YUV420toRGB();
    break;
  case YUV422:
    break;
  case YUV400:
    break;
  case RGB:
    break;
  }
}

Void PlaYUVerFrame::CopyFrom( PlaYUVerFrame* input_frame )
{
  if( m_iPixelFormat != input_frame->getPelFormat() )
  {
    return;
  }
  UInt64 num_bytes = m_uiWidth * m_uiHeight;
  switch( m_iPixelFormat )
  {
  case YUV420:
    memcpy( &m_pppcInputPel[0][0][0], &( input_frame->getPelBufferYUV()[0][0][0] ), num_bytes * sizeof(Pel) );
    memcpy( &m_pppcInputPel[1][0][0], &( input_frame->getPelBufferYUV()[1][0][0] ), num_bytes / 4 * sizeof(Pel) );
    memcpy( &m_pppcInputPel[2][0][0], &( input_frame->getPelBufferYUV()[2][0][0] ), num_bytes / 4 * sizeof(Pel) );
    //YUV420toRGB();
    break;
  case YUV422:
    break;
  case YUV400:
    break;
  case RGB:
    break;
  }
}

QImage PlaYUVerFrame::getQimage()
{
  QImage img( m_uiWidth, m_uiHeight, QImage::Format_RGB888 );
  Pel*** bufferRGB = getPelBufferRGB();

  if( sizeof(Pel) == sizeof(unsigned char) )
  {
    for( UInt y = 0; y < m_uiHeight; y++ )
    {
      for( UInt x = 0; x < m_uiWidth; x++ )
      {
        img.setPixel( x, y, qRgb( bufferRGB[0][y][x], bufferRGB[1][y][x], bufferRGB[2][y][x] ) );
      }
    }
  }
  else
  {
    Q_ASSERT( 0 );
  }
  return img;
}

UInt64 PlaYUVerFrame::getBytesPerFrame()
{
  switch( m_iPixelFormat )
  {
  case YUV420:
    return m_uiWidth * m_uiHeight * 1.5;
    break;
  case YUV422:
    return m_uiWidth * m_uiHeight * 2;
    break;
  case YUV400:
    return m_uiWidth * m_uiHeight;
    break;
  case RGB:
    return m_uiWidth * m_uiHeight * 3;
    break;
  default:
    return 0;
  }
  return 0;
}

Pel PlaYUVerFrame::getPixelValueFromYUV(const QPoint &pos, YUVcomponent color)
{
  return m_pppcInputPel[color][0][pos.y()*m_uiWidth+pos.x()];
}


}  // NAMESPACE
