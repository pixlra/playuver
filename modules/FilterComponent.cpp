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
 * \file     FilterComponent.cpp
 * \brief    Filter component frame module
 */

#include "FilterComponent.h"

#include <cstring>

FilterComponentModule::FilterComponentModule()
{
  m_pcFilteredFrame = NULL;
}

Bool FilterComponentModule::createFilter( UInt uiWidth, UInt uiHeight, UInt bitsPixel )
{
  m_pcFilteredFrame = NULL;
  m_pcFilteredFrame = new PlaYUVerFrame( uiWidth, uiHeight, PlaYUVerFrame::GRAY, bitsPixel );
  return true;
}

PlaYUVerFrame* FilterComponentModule::filterComponent( PlaYUVerFrame* InputFrame, Int Component )
{
  Pel*** pppOutputPelYUV = m_pcFilteredFrame->getPelBufferYUV();
  Pel*** pppInputPelYUV = InputFrame->getPelBufferYUV();
  memcpy( pppOutputPelYUV[LUMA][0], pppInputPelYUV[Component][0],
          m_pcFilteredFrame->getWidth() * m_pcFilteredFrame->getHeight() * sizeof( Pel ) );
  return m_pcFilteredFrame;
}

Void FilterComponentModule::destroy()
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = NULL;
}

FilterComponentLuma::FilterComponentLuma()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;              // Apply module to the frames or to
                                                        // the whole sequence.
                                                        // Currently only support for frame
  m_pchModuleCategory = "FilterComponent";              // Category (sub-menu)
  m_pchModuleName = "Luma";                             // Name
  m_pchModuleTooltip = "Filter Y matrix of YUV frame";  // Description
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;       // Number of Frames required
                                                        // (ONE_FRAME, TWO_FRAMES,
                                                        // THREE_FRAMES)
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;     // Module requirements
                                                        // (check
                                                        // PlaYUVerModulesIf.h).
  // Several requirements should be "or" between each others.
}

Bool FilterComponentLuma::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  return createFilter( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(),
                       apcFrameList[0]->getBitsPel() );
}

PlaYUVerFrame* FilterComponentLuma::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  return filterComponent( apcFrameList[0], LUMA );
}

FilterComponentChromaU::FilterComponentChromaU()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "FilterComponent";
  m_pchModuleName = "ChromaU";
  m_pchModuleTooltip = "Filter U matrix of YUV frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;
}

Bool FilterComponentChromaU::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  if( apcFrameList[0]->getNumberChannels() > 1 )
  {
    return createFilter( apcFrameList[0]->getChromaWidth(), apcFrameList[0]->getChromaHeight(),
                         apcFrameList[0]->getBitsPel() );
  }
  return false;
}

PlaYUVerFrame* FilterComponentChromaU::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  return filterComponent( apcFrameList[0], CHROMA_U );
}

FilterComponentChromaV::FilterComponentChromaV()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "FilterComponent";
  m_pchModuleName = "ChromaV";
  m_pchModuleTooltip = "Filter V matrix of YUV frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;
}

Bool FilterComponentChromaV::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  if( apcFrameList[0]->getNumberChannels() > 1 )
  {
    return createFilter( apcFrameList[0]->getChromaWidth(), apcFrameList[0]->getChromaHeight(),
                         apcFrameList[0]->getBitsPel() );
  }
  return false;
}

PlaYUVerFrame* FilterComponentChromaV::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  return filterComponent( apcFrameList[0], CHROMA_V );
}
