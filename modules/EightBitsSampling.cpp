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
 * \file     EightBitsSampling.cpp
 * \brief    Binarize frame module
 */

#include "EightBitsSampling.h"

EightBitsSampling::EightBitsSampling()
{
  /* Module Definition */
  m_iModuleAPI = CLP_MODULE_API_2;
  m_iModuleType = CLP_FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Conversions";
  m_pchModuleName = "EightBitsSampling";
  m_pchModuleLongName = "8 bit sub-sampling";
  m_pchModuleTooltip = "Sub-sampling frame to 8bpp";
  m_uiNumberOfFrames = 1;
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_NOTHING;

  m_pcSubSampledFrame = NULL;
}

bool EightBitsSampling::create( std::vector<CalypFrame*> apcFrameList )
{
  if( apcFrameList[0]->getBitsPel() > 8 )
  {
    m_pcSubSampledFrame = NULL;
    m_pcSubSampledFrame = new CalypFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(),
                                          apcFrameList[0]->getPelFormat(), 8 );
    return true;
  }
  return false;
}

CalypFrame* EightBitsSampling::process( std::vector<CalypFrame*> apcFrameList )
{
  CalypFrame* pcFrame = apcFrameList[0];
  unsigned int uiShiftBits = pcFrame->getBitsPel() - 8;
  ClpPel* pPelInput = pcFrame->getPelBufferYUV()[0][0];
  ClpPel* pPelSubSampled = m_pcSubSampledFrame->getPelBufferYUV()[0][0];
  ClpPel pelValue;

  for( unsigned int i = 0; i < pcFrame->getHeight() * pcFrame->getWidth(); i++ )
  {
    pelValue = *pPelInput++;
    pelValue = pelValue >> uiShiftBits;
    *pPelSubSampled++ = pelValue;
  }
  for( unsigned int i = 0; i < pcFrame->getChromaLength() * 2; i++ )
  {
    pelValue = *pPelInput++;
    pelValue = pelValue >> uiShiftBits;
    *pPelSubSampled++ = pelValue;
  }
  return m_pcSubSampledFrame;
}

void EightBitsSampling::destroy()
{
  if( m_pcSubSampledFrame )
    delete m_pcSubSampledFrame;
  m_pcSubSampledFrame = NULL;
}
