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
 * \file     EightBitsSampling.cpp
 * \brief    Binarize frame module
 */

#include "EightBitsSampling.h"

EightBitsSampling::EightBitsSampling()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Conversions";
  m_pchModuleName = "EightBitsSampling";
  m_pchModuleTooltip = "Sub-sampling frame to 8bpp";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;

  m_pcSubSampledFrame = NULL;
}

Bool EightBitsSampling::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  if( apcFrameList[0]->getBitsPel() > 8 )
  {
    m_pcSubSampledFrame = NULL;
    m_pcSubSampledFrame = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), apcFrameList[0]->getPelFormat(), 8 );
    return true;
  }
  return false;
}

PlaYUVerFrame* EightBitsSampling::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  PlaYUVerFrame* pcFrame = apcFrameList[0];
  UInt uiShiftBits = pcFrame->getBitsPel() - 8;
  Pel* pPelInput = pcFrame->getPelBufferYUV()[0][0];
  Pel* pPelSubSampled = m_pcSubSampledFrame->getPelBufferYUV()[0][0];
  Pel pelValue;

  for( UInt i = 0; i < pcFrame->getHeight() * pcFrame->getWidth(); i++ )
  {
    pelValue = *pPelInput++;
    pelValue = pelValue >> uiShiftBits;
    *pPelSubSampled++ = pelValue;
  }
  for( UInt i = 0; i < pcFrame->getChromaLength() * 2; i++ )
  {
    pelValue = *pPelInput++;
    pelValue = pelValue >> uiShiftBits;
    *pPelSubSampled++ = pelValue;
  }
  return m_pcSubSampledFrame;
}

Void EightBitsSampling::destroy()
{
  if( m_pcSubSampledFrame )
    delete m_pcSubSampledFrame;
  m_pcSubSampledFrame = NULL;
}
