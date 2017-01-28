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
 * \file     StreamHandlerOpenCV.cpp
 * \brief    Interface with opencv lib
 */

// Self
#include "StreamHandlerOpenCV.h"

// System
#include <cstdio>
#include <opencv2/opencv.hpp>

// Own
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#include "LibMemory.h"

using cv::Mat;
using cv::VideoCapture;

std::vector<PlaYUVerSupportedFormat> StreamHandlerOpenCV::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  //   REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Device", "/dev/" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> StreamHandlerOpenCV::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Portable PixMap ", "ppm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Portable Network Graphics", "png" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Joint Photographic Experts Group", "jpg" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Windows Bitmap", "bmp" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

StreamHandlerOpenCV::StreamHandlerOpenCV()
{
  pcVideoCapture = NULL;
}

Bool StreamHandlerOpenCV::openHandler( String strFilename, Bool bInput )
{
  m_cFilename = strFilename;
  if( bInput )
  {
    /*
     * Special filename to handle webcam input
     */
    if( m_cFilename.find( "/dev/video" ) != String::npos )
    {
      m_strFormatName = "DEV";
      m_strCodecName = "Raw Video";
      pcVideoCapture = new VideoCapture( 0 );
      m_uiWidth = pcVideoCapture->get( CV_CAP_PROP_FRAME_WIDTH );
      m_uiHeight = pcVideoCapture->get( CV_CAP_PROP_FRAME_HEIGHT );
      m_dFrameRate = 25;
    }
    else
    {
      m_strCodecName = m_strFormatName = uppercase( strFilename.substr( strFilename.find_last_of( "." ) + 1 ) );
      Mat cvMat = cv::imread( m_cFilename );
      m_uiWidth = cvMat.cols;
      m_uiHeight = cvMat.rows;
      m_dFrameRate = 1;
    }
    m_uiBitsPerPixel = 8;
    m_iPixelFormat = PlaYUVerFrame::findPixelFormat( "BGR24" );
  }

  return true;
}

Void StreamHandlerOpenCV::closeHandler()
{
  if( pcVideoCapture )
  {
    pcVideoCapture->release();
    delete pcVideoCapture;
    pcVideoCapture = NULL;
  }
}

Bool StreamHandlerOpenCV::configureBuffer( PlaYUVerFrame* pcFrame )
{
  return true;
}

UInt64 StreamHandlerOpenCV::calculateFrameNumber()
{
  if( pcVideoCapture )
  {
    return 2;
  }
  return 1;
}

Bool StreamHandlerOpenCV::seek( UInt64 iFrameNum )
{
  return true;
}

Bool StreamHandlerOpenCV::read( PlaYUVerFrame* pcFrame )
{
  Bool bRet = false;
  if( pcVideoCapture )
  {
    Mat cvMat;
    bRet = pcVideoCapture->read( cvMat );
    if( bRet )
      bRet = pcFrame->fromMat( cvMat );
  }
  else
  {
    Mat cvMat = cv::imread( m_cFilename );
    bRet = pcFrame->fromMat( cvMat );
  }
  return bRet;
}

Bool StreamHandlerOpenCV::write( PlaYUVerFrame* pcFrame )
{
  Bool bRet = false;
  Mat cvFrame;
  bRet = pcFrame->toMat( cvFrame );
  if( bRet )
  {
    bRet = cv::imwrite( m_cFilename, cvFrame );
  }
  return bRet;
}
