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

#include "MeasurePSNR.h"

namespace plaYUVer
{

static PlaYUVerModuleDefinition MeasurePSNRDef =
{
    FRAME_MEASUREMENT_MODULE,
    "Quality",
    "PSNR Y",
    "Measure the PSNR between two images",
    MODULE_REQUIRES_TWO_FRAMES,
    MODULE_REQUIRES_SIDEBAR,
    APPLY_WHILE_PLAYING, };

MeasurePSNR::MeasurePSNR()
{
  setModuleDefinition( MeasurePSNRDef );
}

Void MeasurePSNR::create( PlaYUVerFrame* Org )
{
}

Double MeasurePSNR::getMSE( PlaYUVerFrame* Org, PlaYUVerFrame* Rec, Int component )
{
  Pel* pInput1PelYUV = Org->getPelBufferYUV()[component][0];
  Pel* pInput2PelYUV = Rec->getPelBufferYUV()[component][0];

  Int aux_pel_1, aux_pel_2;
  Int diff = 0;
  Double ssd = 0;

  for( UInt y = 0; y < Org->getHeight(); y++ )
    for( UInt x = 0; x < Org->getWidth(); x++ )
    {
      aux_pel_1 = *pInput1PelYUV++;
      aux_pel_2 = *pInput2PelYUV++;
      diff = aux_pel_1 - aux_pel_2;
      ssd += ( Double )( diff * diff );
    }
  if( ssd == 0.0 )
  {
    return 0.0;
  }
  return ssd / ( Org->getWidth() * Org->getHeight() );
}

Double MeasurePSNR::measure( PlaYUVerFrame* Org, PlaYUVerFrame* Rec )
{
  Double psnrY = 100;
  Double mseY = MeasurePSNR::getMSE( Org, Rec, LUMA );

  if( mseY != 0 )
    psnrY = 10 * log10( 65025 / mseY );

  return psnrY;
}

Void MeasurePSNR::destroy()
{

}

}  // NAMESPACE

