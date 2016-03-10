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
 * \file     FrameMask.cpp
 * \brief    Frame Difference module
 */

#include "FrameMask.h"
#include <algorithm>

namespace plaYUVer
{

FrameMask::FrameMask()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleName = "FrameMask";
  m_pchModuleTooltip = "Applies a mask to the selected image (first image)";
  m_uiModuleRequirements = MODULE_REQUIRES_NEW_WINDOW;
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;

  m_dWeigth = 0.3;

  m_pcFrameProcessed = NULL;
}

Bool FrameMask::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 0; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[i], PlaYUVerFrame::MATCH_COLOR_SPACE |
                                       PlaYUVerFrame::MATCH_RESOLUTION | PlaYUVerFrame::MATCH_BITS ) )
    {
      return false;
    }
  }
  Int iPelFmt = apcFrameList[0]->getPelFormat();
  iPelFmt = apcFrameList[1]->getPelFormat() > iPelFmt ? apcFrameList[1]->getPelFormat() : iPelFmt;
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
      pixelOut = pixelImg * ( 1 - m_dWeigth ) + pixelMask * m_dWeigth;
      m_pcFrameProcessed->setPixelValue( x, y, pixelOut );
    }
  }
  return m_pcFrameProcessed;
}

Void FrameMask::destroy()
{
  if( m_pcFrameProcessed )
  {
    delete m_pcFrameProcessed;
  }
  m_pcFrameProcessed = NULL;
}

}  // NAMESPACE

