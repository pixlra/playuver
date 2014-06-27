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
 * \brief    Frame Difference module
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
    MODULE_REQUIRES_SIDEBAR,
    APPLY_WHILE_PLAYING,
};

MeasurePSNR::MeasurePSNR()
{
  setModuleDefinition( MeasurePSNRDef );
}

Void MeasurePSNR::create( PlaYUVerFrame* Org )
{
}

Double MeasurePSNR::measure( PlaYUVerFrame* Org, PlaYUVerFrame* Rec )
{
  Pel* pInput1PelYUV = Org->getPelBufferYUV()[0][0];
  Pel* pInput2PelYUV = Rec->getPelBufferYUV()[0][0];

  Int aux_pel_1, aux_pel_2;
  Double diff = 0;

  for( UInt y = 0; y < Org->getHeight(); y++ )
    for( UInt x = 0; x < Org->getWidth(); x++ )
    {
      aux_pel_1 = *pInput1PelYUV++;
      aux_pel_2 = *pInput2PelYUV++;
      diff += abs( aux_pel_1 - aux_pel_2 );
    }

  return diff;
}

Void MeasurePSNR::destroy()
{

}

}  // NAMESPACE

