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

  m_pcFrameMask = NULL;
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

  m_pcFrameMask = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), apcFrameList[0]->getPelFormat(), apcFrameList[0]->getBitsPel() );
  return true;
}

PlaYUVerFrame* FrameMask::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Pel* pInput1PelYUV = apcFrameList[0]->getPelBufferYUV()[0][0];
  Pel* pInput2PelYUV = apcFrameList[1]->getPelBufferYUV()[0][0];
  Pel* pOutputPelYUV = m_pcFrameMask->getPelBufferYUV()[0][0];
  Double aux_pel_1, aux_pel_2, aux_pel_mask;

  for( UInt y = 0; y < m_pcFrameMask->getHeight(); y++ )
    for( UInt x = 0; x < m_pcFrameMask->getWidth(); x++ )
    {
      aux_pel_1 = *pInput1PelYUV++;
      aux_pel_2 = *pInput2PelYUV++;
      aux_pel_mask = aux_pel_1 * ( 1 - m_dWeigth ) + aux_pel_2 * m_dWeigth;
      *pOutputPelYUV++ = aux_pel_mask;
    }

  for( UInt c = 1; c < m_pcFrameMask->getNumberChannels(); c++ )
    for( UInt y = 0; y < m_pcFrameMask->getChromaHeight(); y++ )
      for( UInt x = 0; x < m_pcFrameMask->getChromaWidth(); x++ )
      {
        aux_pel_1 = *pInput1PelYUV++;
        aux_pel_2 = *pInput2PelYUV++;
        aux_pel_mask = aux_pel_1 * ( 1 - m_dWeigth ) + aux_pel_2 * m_dWeigth;
        *pOutputPelYUV++ = aux_pel_mask;
      }

  return m_pcFrameMask;
}

Void FrameMask::destroy()
{
  if( m_pcFrameMask )
  {
    delete m_pcFrameMask;
  }
  m_pcFrameMask = NULL;
}

}  // NAMESPACE

