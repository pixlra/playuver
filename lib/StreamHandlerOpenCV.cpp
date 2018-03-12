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
 * \file     StreamHandlerOpenCV.cpp
 * \brief    interface with opencv lib
 */

#include "StreamHandlerOpenCV.h"

#include "CalypFrame.h"
#include "LibMemory.h"

#include <cstdio>
#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::VideoCapture;

std::vector<CalypStreamFormat> StreamHandlerOpenCV::supportedReadFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
#if( CV_MAJOR_VERSION >= 3 )
  REGIST_CALYP_SUPPORTED_ABSTRACT_FMT( &StreamHandlerOpenCV::Create, "Device", "/dev/video*" );
#endif
  END_REGIST_CALYP_SUPPORTED_FMT;
}

std::vector<CalypStreamFormat> StreamHandlerOpenCV::supportedWriteFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Portable PixMap ", "ppm" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Portable Network Graphics", "png" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Joint Photographic Experts Group", "jpg" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerOpenCV::Create, "Windows Bitmap", "bmp" );
  END_REGIST_CALYP_SUPPORTED_FMT;
}

StreamHandlerOpenCV::StreamHandlerOpenCV()
{
  m_pchHandlerName = "OpenCV";
  pcVideoCapture = NULL;
}

bool StreamHandlerOpenCV::openHandler( ClpString strFilename, bool bInput )
{
  m_cFilename = strFilename;
  if( bInput )
  {
#if( CV_VERSION_MAJOR >= 3 )
    /*
     * Special filename to handle webcam input
     */
    if( m_cFilename.find( "/dev/video" ) != ClpString::npos )
    {
      int iDeviceId = 0;
      if( m_cFilename.find( "/dev/video0" ) != ClpString::npos )
        iDeviceId = 0;
      else if( m_cFilename.find( "/dev/video1" ) != ClpString::npos )
        iDeviceId = 1;

      m_strFormatName = "DEV";
      m_strCodecName = "Raw Video";
      pcVideoCapture = new VideoCapture( iDeviceId );
      m_uiWidth = pcVideoCapture->get( CV_CAP_PROP_FRAME_WIDTH );
      m_uiHeight = pcVideoCapture->get( CV_CAP_PROP_FRAME_HEIGHT );
      m_dFrameRate = 25;
    }
    else
#endif
    {
      m_strCodecName = m_strFormatName = clpUppercase( strFilename.substr( strFilename.find_last_of( "." ) + 1 ) );
      Mat cvMat = cv::imread( m_cFilename );
      m_uiWidth = cvMat.cols;
      m_uiHeight = cvMat.rows;
      m_dFrameRate = 1;
    }
    m_uiBitsPerPixel = 8;
    m_iPixelFormat = CalypFrame::findPixelFormat( "BGR" );

    m_uiTotalNumberFrames = 1;
    if( pcVideoCapture )
    {
      m_uiTotalNumberFrames = 2;
    }
  }

  return true;
}

void StreamHandlerOpenCV::closeHandler()
{
  if( pcVideoCapture )
  {
    pcVideoCapture->release();
    delete pcVideoCapture;
    pcVideoCapture = NULL;
  }
}

bool StreamHandlerOpenCV::configureBuffer( CalypFrame* pcFrame )
{
  return true;
}

bool StreamHandlerOpenCV::seek( unsigned long long int iFrameNum )
{
  // m_uiCurrFrameFileIdx =
  return true;
}

bool StreamHandlerOpenCV::read( CalypFrame* pcFrame )
{
  bool bRet = false;
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
  if( bRet )
    m_uiCurrFrameFileIdx++;
  return bRet;
}

bool StreamHandlerOpenCV::write( CalypFrame* pcFrame )
{
  bool bRet = false;
  Mat cvFrame;
  bRet = pcFrame->toMat( cvFrame );
  if( bRet )
  {
    bRet = cv::imwrite( m_cFilename, cvFrame );
  }
  return bRet;
}
