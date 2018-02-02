/*    This file is a part of PlaYUVer project
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
 * \file     FrameRotate.cpp
 * \brief    Frame Difference module
 */

#include "FrameRotate.h"

FrameRotate::FrameRotate()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleLongName = "Rotation";
  m_pchModuleName = "FrameRotate";
  m_pchModuleTooltip = "Rotates frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_OPTIONS;

  m_cModuleOptions.addOptions() /**/
      ( "Angle", m_iAngle, "Angle to rotate (0, 90, 180, 270)" );

  m_iAngle = 90;

  m_pcFrameProcessed = NULL;
}

Bool FrameRotate::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  UInt iWidth;
  UInt iHeight;

  switch( m_iAngle )
  {
  case 0:
  case 180:
    iWidth = apcFrameList[0]->getWidth();
    iHeight = apcFrameList[0]->getHeight();
    break;
  case 90:
  case 270:
    iHeight = apcFrameList[0]->getWidth();
    iWidth = apcFrameList[0]->getHeight();
    break;
  default:
    return false;
  }

  m_pcFrameProcessed = new PlaYUVerFrame( iWidth, iHeight, apcFrameList[0]->getPelFormat(),
                                          apcFrameList[0]->getBitsPel() );
  return true;
}

PlaYUVerFrame* FrameRotate::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  PlaYUVerFrame* InputFrame = apcFrameList[0];
  UInt inWidht = InputFrame->getWidth();
  UInt inHeight = InputFrame->getHeight();
  PlaYUVerPixel pixel;
  UInt inX;
  UInt inY;

  for( UInt y = 0; y < m_pcFrameProcessed->getHeight(); y++ )
  {
    for( UInt x = 0; x < m_pcFrameProcessed->getWidth(); x++ )
    {
      inX = x;
      inY = y;
      switch( m_iAngle )
      {
      case 90:
        inX = y;
        inY = inHeight - x - 1;
        break;
      case 180:
        inX = inWidht - x - 1;
        inY = inHeight - y - 1;
        break;
      case 270:
        inX = inWidht - y;
        inY = x;
        break;
      }
      pixel = InputFrame->getPixelValue( inX, inY );
      m_pcFrameProcessed->setPixelValue( x, y, pixel );
    }
  }
  return m_pcFrameProcessed;
}

Void FrameRotate::destroy()
{
  if( m_pcFrameProcessed )
  {
    delete m_pcFrameProcessed;
  }
  m_pcFrameProcessed = NULL;
}
