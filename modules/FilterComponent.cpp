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
 * \file     FilterComponent.cpp
 * \brief    Filter component frame module
 */

#include "FilterComponent.h"

#include <cstring>

FilterComponentModule::FilterComponentModule()
{
  /* Module Definition */
  m_iModuleAPI = CLP_MODULE_API_2;
  m_iModuleType = CLP_FRAME_PROCESSING_MODULE;
  m_uiNumberOfFrames = 1;
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_NOTHING;
  m_pchModuleCategory = "Filtering";

  m_pcFilteredFrame = NULL;
}

bool FilterComponentModule::createFilter( unsigned int uiWidth, unsigned int uiHeight, unsigned int bitsPixel )
{
  m_pcFilteredFrame = NULL;
  m_pcFilteredFrame = new CalypFrame( uiWidth, uiHeight, CLP_GRAY, bitsPixel );
  return true;
}

CalypFrame* FilterComponentModule::filterComponent( CalypFrame* InputFrame, int Component )
{
  ClpPel*** pppOutputPelYUV = m_pcFilteredFrame->getPelBufferYUV();
  ClpPel*** pppInputPelYUV = InputFrame->getPelBufferYUV();
  memcpy( pppOutputPelYUV[CLP_LUMA][0], pppInputPelYUV[Component][0],
          m_pcFilteredFrame->getWidth() * m_pcFilteredFrame->getHeight() * sizeof( ClpPel ) );
  return m_pcFilteredFrame;
}

void FilterComponentModule::destroy()
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = NULL;
}

FilterComponentLuma::FilterComponentLuma()
{
  /* Module Definition */
  m_pchModuleName = "FilterLuma";                       // Name
  m_pchModuleLongName = "Luma Component";               // Name on the Gui
  m_pchModuleTooltip = "Filter Y matrix of YUV frame";  // Description
}

bool FilterComponentLuma::create( std::vector<CalypFrame*> apcFrameList )
{
  return createFilter( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), apcFrameList[0]->getBitsPel() );
}

CalypFrame* FilterComponentLuma::process( std::vector<CalypFrame*> apcFrameList )
{
  return filterComponent( apcFrameList[0], CLP_LUMA );
}

FilterComponentChromaU::FilterComponentChromaU()
{
  m_pchModuleName = "FilterChromaU";
  m_pchModuleLongName = "U/Cb Component";
  m_pchModuleTooltip = "Filter U matrix of YUV frame";
}

bool FilterComponentChromaU::create( std::vector<CalypFrame*> apcFrameList )
{
  if( apcFrameList[0]->getNumberChannels() > 1 )
  {
    return createFilter( apcFrameList[0]->getWidth( CLP_CHROMA_U ), apcFrameList[0]->getHeight( CLP_CHROMA_U ),
                         apcFrameList[0]->getBitsPel() );
  }
  return false;
}

CalypFrame* FilterComponentChromaU::process( std::vector<CalypFrame*> apcFrameList )
{
  return filterComponent( apcFrameList[0], CLP_CHROMA_U );
}

FilterComponentChromaV::FilterComponentChromaV()
{
  /* Module Definition */
  m_pchModuleName = "FilterChromaV";
  m_pchModuleLongName = "V/Cr Component";
  m_pchModuleTooltip = "Filter V matrix of YUV frame";
}

bool FilterComponentChromaV::create( std::vector<CalypFrame*> apcFrameList )
{
  if( apcFrameList[0]->getNumberChannels() > 1 )
  {
    return createFilter( apcFrameList[0]->getWidth( CLP_CHROMA_V ), apcFrameList[0]->getHeight( CLP_CHROMA_V ),
                         apcFrameList[0]->getBitsPel() );
  }
  return false;
}

CalypFrame* FilterComponentChromaV::process( std::vector<CalypFrame*> apcFrameList )
{
  return filterComponent( apcFrameList[0], CLP_CHROMA_V );
}
