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
 * \file     MeasurePSNR.cpp
 * \brief    Measure PSNR module
 */

#include <cstdio>

#include "MeasurePSNR.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

static PlaYUVerModuleDefinition MeasurePSNRDef =
{
    FRAME_MEASUREMENT_MODULE,
    "Quality",
    "PSNR",
    "Measure the PSNR between two images",
    MODULE_REQUIRES_TWO_FRAMES,
    MODULE_REQUIRES_NEW_WINDOW,
    APPLY_WHILE_PLAYING,
};

MeasurePSNR::MeasurePSNR()
{
  m_pcFilteredFrame = NULL;
  setModuleDefinition( MeasurePSNRDef );
}

Void MeasurePSNR::create( PlaYUVerFrame* InputFrame )
{
  m_pcFilteredFrame = NULL;
  m_pcFilteredFrame = new PlaYUVerFrame( InputFrame->getWidth(), InputFrame->getHeight(), PlaYUVerFrame::GRAY );
}

PlaYUVerFrame* MeasurePSNR::process( PlaYUVerFrame* InputFrame )
{
  return m_pcFilteredFrame;
}

Void MeasurePSNR::destroy()
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = NULL;
}

}  // NAMESPACE

