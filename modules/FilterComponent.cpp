/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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

#include <cstdio>
#include "FilterComponent.h"

namespace plaYUVer
{

FilterComponent::FilterComponent()
{
  m_pcFilteredFrame = NULL;
}

Void FilterComponent::createFilter( UInt uiWidth, UInt uiHeight )
{
  m_pcFilteredFrame = NULL;
  m_pcFilteredFrame = new PlaYUVerFrame( uiWidth, uiHeight, PlaYUVerFrame::GRAY );
}

PlaYUVerFrame* FilterComponent::filterComponent( PlaYUVerFrame* InputFrame, Int Component )
{
  Pel*** pppOutputPelYUV = m_pcFilteredFrame->getPelBufferYUV();
  Pel*** pppInputPelYUV = InputFrame->getPelBufferYUV();
  memcpy( pppOutputPelYUV[LUMA][0], pppInputPelYUV[Component][0], m_pcFilteredFrame->getWidth() * m_pcFilteredFrame->getHeight() * sizeof(Pel) );
  return m_pcFilteredFrame;
}

Void FilterComponent::destroy()
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = NULL;
}

FilterComponentLuma::FilterComponentLuma()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;              // Apply module to the frames or to the whole sequence.
                                                        // Currently only support for frame
  m_pchModuleCategory = "Filter Component";             // Category (sub-menu)
  m_pchModuleName = "Luma";                             // Name
  m_pchModuleTooltip = "Filter Y matrix of YUV frame";  // Description
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;       // Number of Frames required (ONE_FRAME, TWO_FRAMES, THREE_FRAMES)
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;     // Module requirements (check PlaYUVerModulesIf.h).
                                                        // Several requirements should be "or" between each others.
  m_bApplyWhilePlaying = APPLY_WHILE_PLAYING;           // Apply module while playing
}

Void FilterComponentLuma::create( PlaYUVerFrame* InputFrame )
{
  createFilter( InputFrame->getWidth(), InputFrame->getHeight() );
}

PlaYUVerFrame* FilterComponentLuma::process( PlaYUVerFrame* InputFrame )
{
  return filterComponent( InputFrame, LUMA );
}

FilterComponentChromaU::FilterComponentChromaU()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Filter Component";
  m_pchModuleName = "Chroma U";
  m_pchModuleTooltip = "Filter U matrix of YUV frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;
  m_bApplyWhilePlaying = !APPLY_WHILE_PLAYING;
}

Void FilterComponentChromaU::create( PlaYUVerFrame* InputFrame )
{
  createFilter( InputFrame->getChromaWidth(), InputFrame->getChromaHeight() );
}

PlaYUVerFrame* FilterComponentChromaU::process( PlaYUVerFrame* InputFrame )
{
  return filterComponent( InputFrame, CHROMA_U );
}

FilterComponentChromaV::FilterComponentChromaV()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Filter Component";
  m_pchModuleName = "Chroma V";
  m_pchModuleTooltip = "Filter V matrix of YUV frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;
  m_bApplyWhilePlaying = !APPLY_WHILE_PLAYING;
}

Void FilterComponentChromaV::create( PlaYUVerFrame* InputFrame )
{
  createFilter( InputFrame->getChromaWidth(), InputFrame->getChromaHeight() );
}

PlaYUVerFrame* FilterComponentChromaV::process( PlaYUVerFrame* InputFrame )
{
  return filterComponent( InputFrame, CHROMA_V );
}

}  // NAMESPACE

