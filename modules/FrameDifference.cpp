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
 * \file     FrameDifference.cpp
 * \brief    Frame Difference module
 */

#include "FrameDifference.h"
#include <algorithm>

namespace plaYUVer
{

FrameDifference::FrameDifference()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Measurements";
  m_pchModuleName = "FrameDifference";
  m_pchModuleTooltip = "Measure the difference between two images (Y plane),  Y1 - Y2, with max absolute diff of 128";
  m_uiModuleRequirements = MODULE_REQUIRES_NEW_WINDOW;
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;

  m_pcFrameDifference = NULL;
}

Bool FrameDifference::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  UInt bitsPixel = ( apcFrameList[0]->getBitsPel() + 1 ) & 0x0F;
  m_iMaxDiffValue = ( 1 << ( bitsPixel - 1 ) );
  m_pcFrameDifference = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), PlaYUVerFrame::GRAY, bitsPixel );
  return true;
}

PlaYUVerFrame* FrameDifference::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Pel* pInput1PelYUV =  apcFrameList[0]->getPelBufferYUV()[0][0];
  Pel* pInput2PelYUV = apcFrameList[1]->getPelBufferYUV()[0][0];
  Pel* pOutputPelYUV = m_pcFrameDifference->getPelBufferYUV()[0][0];
  Int aux_pel_1, aux_pel_2;
  Int diff = 0;

  for( UInt y = 0; y < m_pcFrameDifference->getHeight(); y++ )
    for( UInt x = 0; x < m_pcFrameDifference->getWidth(); x++ )
    {
      aux_pel_1 = *pInput1PelYUV++;
      aux_pel_2 = *pInput2PelYUV++;
      diff = aux_pel_1 - aux_pel_2;
      diff = std::min( diff, m_iMaxDiffValue );
      diff = std::max( diff, -m_iMaxDiffValue );
      diff += m_iMaxDiffValue + 1;
      *pOutputPelYUV++ = diff;
    }
  return m_pcFrameDifference;
}

Void FrameDifference::destroy()
{
  if( m_pcFrameDifference )
    delete m_pcFrameDifference;
  m_pcFrameDifference = NULL;
}

}  // NAMESPACE

