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
 * \file     StreamHandlerPortableMap.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Interface with opencv lib
 */

#include <cstdio>
#include "StreamHandlerPortableMap.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#include "LibMemory.h"

namespace plaYUVer
{

std::vector<PlaYUVerSupportedFormat> StreamHandlerPortableMap::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable Grayscale Map ", "pgm"  );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable Pix Map ", "ppm"  );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> StreamHandlerPortableMap::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
//   APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerPortableMap, Read );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

Bool StreamHandlerPortableMap::openHandler( std::string strFilename, Bool bInput )
{
  m_cFilename = strFilename;
  return true;
}

Void StreamHandlerPortableMap::closeHandler()
{

}

Bool StreamHandlerPortableMap::configureBuffer( PlaYUVerFrame* pcFrame )
{
  return true;
}

Void StreamHandlerPortableMap::getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, Int& rEndianness, Double& rFrameRate )
{
//   rWidth = m_uiWidth;
//   rHeight = m_uiHeight;
//   rInputFormat = m_iPixelFormat;
//   rBitsPerPel = m_uiBitsPerPixel;
//   rEndianness = m_iEndianness;
  rFrameRate = 1;
}

UInt64 StreamHandlerPortableMap::calculateFrameNumber()
{
  return 1;
}

Bool StreamHandlerPortableMap::seek( UInt64 iFrameNum )
{
  return true;
}

Bool StreamHandlerPortableMap::read( PlaYUVerFrame* pcFrame )
{
  return false;
}

Bool StreamHandlerPortableMap::write( PlaYUVerFrame* pcFrame )
{
  Bool bRet = false;
  cv::Mat* pcCvFrame = pcFrame->getCvMat();
  if( pcCvFrame )
  {
    bRet = cv::imwrite( m_cFilename.c_str(), *pcCvFrame );
    delete pcCvFrame;
  }
  return bRet;
}

}  // NAMESPACE
