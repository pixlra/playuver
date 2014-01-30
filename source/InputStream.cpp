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
 * \file     InputStream.cpp
 * \brief    Input handling
 */

#include <cstdio>

#include <QtDebug>

#include "InputStream.h"
#include "LibMemAlloc.h"

namespace plaYUVer
{

InputStream::InputStream()
{
  m_iStatus = 0;

  m_pFile = NULL;
  m_pppcInputPel[0] = NULL;
  m_pppcInputPel[1] = NULL;
  m_pppcInputPel[2] = NULL;
  m_pppcRGBPel = NULL;

  m_iFileFormat = INVALID;
}

InputStream::~InputStream()
{
  if( m_pFile )
    fclose( m_pFile );

  if( m_pppcInputPel[0] )
    free_mem2Dpel( m_pppcInputPel[0] );
  if( m_pppcInputPel[1] )
    free_mem2Dpel( m_pppcInputPel[1] );
  if( m_pppcInputPel[2] )
    free_mem2Dpel( m_pppcInputPel[2] );

  if( m_pppcRGBPel )
    free_mem3Dpel( m_pppcRGBPel );

  m_iStatus = 0;
}

QString InputStream::supportedReadFormats()
{
  QString formats;
  formats = "*.yuv "   // Raw video
      ;
  return formats;
}

QStringList InputStream::supportedReadFormatsList()
{
  QStringList formats;
  formats << "Raw video (*.yuv)";

  return formats;
}

QStringList InputStream::supportedPixelFormatList()
{
  QStringList formats;
  formats << "YUV420" << "YUV400";
  return formats;
}

Void InputStream::init( QString filename, UInt width, UInt height, Int input_format )
{

  if( width <= 0 || height <= 0 )
  {
    //Error
    return;
  }

  m_cFilename = filename;
  m_pFile = fopen( m_cFilename.toLocal8Bit().data(), "rb" );
  if( m_pFile == NULL )
  {
    // Error
    return;
  }

  m_uiWidth = width;
  m_uiHeight = height;

  m_iFileFormat = YUVFormat;
  m_iPixelFormat = input_format;

  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;
  UInt64 alloc_memory;

  fseek( m_pFile, 0, SEEK_END );
  m_uiTotalFrameNum = ftell( m_pFile ) / ( frame_bytes_input );
  fseek( m_pFile, 0, SEEK_SET );

  if( m_iFileFormat == YUVFormat )
  {
    switch( m_iPixelFormat )
    {
    case YUV420:
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[1] ), m_uiHeight / 2, m_uiWidth / 2 );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[2] ), m_uiHeight / 2, m_uiWidth / 2 );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      break;
    case YUV400:
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      break;
    }
  }

  get_mem3Dpel( &m_pppcRGBPel, 3, m_uiHeight, m_uiWidth );
  if( !alloc_memory )
  {
    //Error
    return;
  }

  m_iStatus = 1;

  return;
}

static inline Pel iClip3Pel( Pel pel_value, Pel min_value, Pel max_value )
{
  if( pel_value > max_value )
    return max_value;
  if( pel_value < min_value )
    return min_value;
  return pel_value;
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

Void InputStream::YUV420toRGB()
{
  Int iY, iU, iV, iR, iG, iB;
  for( Int y = 0; y < m_uiHeight; y += 2 )
  {
    for( Int x = 0; x < m_uiWidth; x += 2 )
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

Void InputStream::readFrame()
{
  UInt64 bytes_read = 0;

  if( m_iStatus == 0 )
  {
    return;
  }
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;

  switch( m_iPixelFormat )
  {
  case YUV420:
    bytes_read = fread( &( m_pppcInputPel[0][0][0] ), sizeof(Pel), m_uiWidth * m_uiHeight, m_pFile );
    if( bytes_read != ( m_uiWidth * m_uiHeight ) )
    {
      m_iErrorStatus = READING;
      qDebug() << " Reading error !!!" << endl;
      return;
    }
    bytes_read = fread( &( m_pppcInputPel[1][0][0] ), sizeof(Pel), m_uiWidth * m_uiHeight / 4, m_pFile );
    if( bytes_read != ( m_uiWidth * m_uiHeight / 4 ) )
    {
      m_iErrorStatus = READING;
      qDebug() << " Reading error !!!" << endl;
      return;
    }
    bytes_read = fread( &( m_pppcInputPel[2][0][0] ), sizeof(Pel), m_uiWidth * m_uiHeight / 4, m_pFile );
    if( bytes_read != ( m_uiWidth * m_uiHeight / 4 ) )
    {
      m_iErrorStatus = READING;
      qDebug() << " Reading error !!!" << endl;
      return;
    }
    YUV420toRGB();
    break;
  }

  return;
}

QImage InputStream::getFrameQImage()
{
  // Create QImage
  QImage img( m_uiWidth, m_uiHeight, QImage::Format_RGB888 );

  UInt64 frame_bytes = m_uiWidth * m_uiHeight;
  Pel pelR, pelG, pelB;
  Pel* bufferRGB = &( m_pppcRGBPel[0][0][0] );

  if( sizeof(Pel) == sizeof(unsigned char) )
  {
    for( Int y = 0; y < m_uiHeight; y++ )
    {
      for( Int x = 0; x < m_uiWidth; x++ )
      {
        img.setPixel( x, y, qRgb( m_pppcRGBPel[0][y][x], m_pppcRGBPel[1][y][x], m_pppcRGBPel[2][y][x] ) );
        //img.setPixel( x, y, qRgb( 0, 0, 255 ) );
      }
    }
  }
  else
  {
    Q_ASSERT( 0 );
  }
  return img;
}

Void InputStream::seekInput( Int new_frame_num )
{
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;
  UInt64 nbytes_seek = frame_bytes_input * new_frame_num;
  fseek( m_pFile, nbytes_seek, SEEK_SET );
  m_uiCurrFrameNum = new_frame_num;
}

Bool InputStream::checkErrors( Int error_type )
{
  if( m_iErrorStatus == error_type )
  {
    m_iErrorStatus = NO_ERROR;
    return true;
  }
  return false;
}

}  // NAMESPACE
