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
 * \file     FrameMask.cpp
 * \brief    Frame Difference module
 */

#include "FrameMask.h"

FrameMask::FrameMask()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleName = "FrameMask";
  m_pchModuleTooltip = "Applies a mask to the selected image (first image)";
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;
  m_uiModuleRequirements = MODULE_REQUIRES_NEW_WINDOW | MODULE_REQUIRES_OPTIONS | MODULE_USES_KEYS;

  m_cModuleOptions.addOptions() /**/
      ( "MaskWeigth", m_iWeight, "Influence of the mask [60%]" );

  m_iWeight = 6;

  m_pcFrameProcessed = NULL;
}

Bool FrameMask::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 1; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0],
                                       PlaYUVerFrame::MATCH_COLOR_SPACE_IGNORE_GRAY | PlaYUVerFrame::MATCH_RESOLUTION | PlaYUVerFrame::MATCH_BITS ) )
    {
      return false;
    }
  }

  m_iWeight = m_iWeight > 100 || m_iWeight < 0 ? 5 : m_iWeight;
  m_iWeight = m_iWeight > 10 ? m_iWeight / 10 : m_iWeight;

  Int iPelFmt = 0;
  Int iColorSpace = apcFrameList[0]->getColorSpace();
  if( iColorSpace == PlaYUVerPixel::COLOR_GRAY )
  {
    iColorSpace = apcFrameList[1]->getPelFormat();
  }
  switch( iColorSpace )
  {
  case PlaYUVerPixel::COLOR_GRAY:
    iPelFmt = PlaYUVerFrame::GRAY;
    break;
  case PlaYUVerPixel::COLOR_YUV:
    iPelFmt = PlaYUVerFrame::YUV444p;
    break;
  case PlaYUVerPixel::COLOR_RGB:
    iPelFmt = PlaYUVerFrame::RGB24;
    break;
  }

  m_pcFrameProcessed = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), iPelFmt, apcFrameList[0]->getBitsPel() );
  return true;
}

PlaYUVerFrame* FrameMask::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  PlaYUVerFrame* InputFrame = apcFrameList[0];
  PlaYUVerFrame* MaskFrame = apcFrameList[1];
  PlaYUVerPixel pixelImg;
  PlaYUVerPixel pixelMask;
  PlaYUVerPixel pixelOut;

  for( UInt y = 0; y < m_pcFrameProcessed->getHeight(); y++ )
  {
    for( UInt x = 0; x < m_pcFrameProcessed->getWidth(); x++ )
    {
      pixelImg = InputFrame->getPixelValue( x, y );
      pixelMask = MaskFrame->getPixelValue( x, y );
      pixelOut = pixelImg * ( 10 - m_iWeight ) + pixelMask * m_iWeight;
      pixelOut = pixelOut * 0.1;
      m_pcFrameProcessed->setPixelValue( x, y, pixelOut );
    }
  }
  return m_pcFrameProcessed;
}

Bool FrameMask::keyPressed( enum Module_Key_Supported value )
{
  if( value == MODULE_KEY_UP && m_iWeight < 10 )
  {
    m_iWeight += 1;
    return true;
  }
  if( value == MODULE_KEY_DOWN && m_iWeight > 0 )
  {
    m_iWeight -= 1;
    return true;
  }
  return false;
}

Void FrameMask::destroy()
{
  if( m_pcFrameProcessed )
  {
    delete m_pcFrameProcessed;
  }
  m_pcFrameProcessed = NULL;
}
