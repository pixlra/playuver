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
 * \file     FrameMask.cpp
 * \brief    Frame Difference module
 */

#include "FrameMask.h"

FrameMask::FrameMask()
{
  /* Module Definition */
  m_iModuleAPI = CLP_MODULE_API_2;
  m_iModuleType = CLP_FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleLongName = "Apply mask";
  m_pchModuleName = "FrameMask";
  m_pchModuleTooltip = "Applies a mask to the selected image (first image)";
  m_uiNumberOfFrames = 2;
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_NEW_WINDOW | CLP_MODULE_REQUIRES_OPTIONS | CLP_MODULE_USES_KEYS;

  m_cModuleOptions.addOptions() /**/
      ( "MaskWeigth", m_iWeight, "Influence of the mask [60%]" );

  m_iWeight = 6;

  m_pcFrameProcessed = NULL;
}

bool FrameMask::create( std::vector<CalypFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( unsigned int i = 1; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0], CalypFrame::MATCH_COLOR_SPACE_IGNORE_GRAY |
                                                            CalypFrame::MATCH_RESOLUTION |
                                                            CalypFrame::MATCH_BITS ) )
    {
      return false;
    }
  }

  m_iWeight = m_iWeight > 100 || m_iWeight < 0 ? 5 : m_iWeight;
  m_iWeight = m_iWeight > 10 ? m_iWeight / 10 : m_iWeight;

  int iPelFmt = 0;
  int iColorSpace = apcFrameList[0]->getColorSpace();
  if( iColorSpace == CLP_COLOR_GRAY )
  {
    iColorSpace = apcFrameList[1]->getPelFormat();
  }
  switch( iColorSpace )
  {
  case CLP_COLOR_GRAY:
    iPelFmt = CLP_GRAY;
    break;
  case CLP_COLOR_YUV:
    iPelFmt = CLP_YUV444P;
    break;
  case CLP_COLOR_RGB:
    iPelFmt = CLP_RGB24;
    break;
  }

  m_pcFrameProcessed = new CalypFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), iPelFmt,
                                       apcFrameList[0]->getBitsPel() );
  return true;
}

CalypFrame* FrameMask::process( std::vector<CalypFrame*> apcFrameList )
{
  CalypFrame* InputFrame = apcFrameList[0];
  CalypFrame* MaskFrame = apcFrameList[1];
  CalypPixel pixelImg;
  CalypPixel pixelMask;
  CalypPixel pixelOut;

  for( unsigned int y = 0; y < m_pcFrameProcessed->getHeight(); y++ )
  {
    for( unsigned int x = 0; x < m_pcFrameProcessed->getWidth(); x++ )
    {
      pixelImg = InputFrame->getPixel( x, y );
      pixelMask = MaskFrame->getPixel( x, y );
      pixelOut = pixelImg * ( 10 - m_iWeight ) + pixelMask * m_iWeight;
      pixelOut = pixelOut * 0.1;
      m_pcFrameProcessed->setPixel( x, y, pixelOut );
    }
  }
  return m_pcFrameProcessed;
}

bool FrameMask::keyPressed( enum Module_Key_Supported value )
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

void FrameMask::destroy()
{
  if( m_pcFrameProcessed )
  {
    delete m_pcFrameProcessed;
  }
  m_pcFrameProcessed = NULL;
}
