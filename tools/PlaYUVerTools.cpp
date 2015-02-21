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
 * \file     PlaYUVerTools.cpp
 * \brief    Main definition of the PlaYUVerTools APp
 */

#include "PlaYUVerTools.h"

namespace plaYUVer
{

PlaYUVerTools::PlaYUVerTools()
{
  m_uiOperation = INVALID_OPERATION;
}

PlaYUVerTools::~PlaYUVerTools()
{
  for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
  {
    m_apcInputStreams[i]->close();
  }
  for( UInt i = 0; i < m_apcOutputStreams.size(); i++ )
  {
    m_apcOutputStreams[i]->close();
  }
}

Int PlaYUVerTools::Init( Int argc, Char *argv[] )
{

  PlaYUVerCmdParser cCommandLineParser( argc, argv );

  if( !cCommandLineParser.parse() )
  {
    return 1;
  }

  if( cCommandLineParser.getOptionsMap().count( "input" ) )
  {
    if( !cCommandLineParser.getOptionsMap().count( "size" ) )
    {
      printf( "Invalid resolution! " );
      return 2;
    }
    std::string resolutionString = cCommandLineParser.getOptionsMap()["size"].as<std::string>();
    std::string fmtString( "yuv420p" );
    if( cCommandLineParser.getOptionsMap().count( "pel_fmt" ) )
    {
      fmtString = cCommandLineParser.getOptionsMap()["pel_fmt"].as<std::string>();
    }

    std::vector<std::string> inputFileNames = cCommandLineParser.getOptionsMap()["input"].as<std::vector<std::string> >();

    PlaYUVerStream* pcStream;
    for( UInt i = 0; i < inputFileNames.size(); i++ )
    {
      pcStream = new PlaYUVerStream;
      if( !pcStream->open( inputFileNames[i], resolutionString, fmtString, 1 ) )
      {
        printf( "Error opening file %s! ", inputFileNames[i].c_str() );
        return 2;
      }
      m_apcInputStreams.push_back( pcStream );
    }
    m_uiNumberOfFrames = MAX_UINT;
    UInt currFrameNum;
    for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
    {
      currFrameNum = m_apcInputStreams[i]->getFrameNum();
      if( currFrameNum < m_uiNumberOfFrames )
        m_uiNumberOfFrames = currFrameNum;
    }
  }

  if( cCommandLineParser.getOptionsMap().count( "quality" ) )
  {
    m_uiOperationIndex = -1;
    std::string qualityMetric = cCommandLineParser.getOptionsMap()["quality"].as<std::string>();
    if( m_apcInputStreams.size() != 2 )
    {
      printf( "Invalid number of inputs! " );
      return 2;
    }
    for( UInt i = 0; i < PlaYUVerFrame::supportedQualityMetricsList().size(); i++ )
    {
      if( lowercase( PlaYUVerFrame::supportedQualityMetricsList()[i] ) == lowercase( qualityMetric ) )
      {
        m_uiOperationIndex = i;
      }
    }
    if( m_uiOperationIndex == -1 )
    {
      printf( "Invalid quality metric! " );
      return 2;
    }
    m_uiOperation = QUALITY_OPERATION;
  }

  if( m_uiOperation == INVALID_OPERATION )
  {
    printf( "No operation was selected! " );
    return 2;
  }
  return 0;
}

Void PlaYUVerTools::Process()
{
  switch( m_uiOperation )
  {
  case QUALITY_OPERATION:
    qualityOperation();
    break;
  }
}

Void PlaYUVerTools::qualityOperation()
{
  PlaYUVerFrame* apcCurrFrame[m_apcInputStreams.size()];
  Bool abEOF[m_apcInputStreams.size()];
  Double dQuality;

  for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
    abEOF[i] = false;

  for( UInt frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {
    printf( "   %3d", frame );
    for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
      apcCurrFrame[i] = m_apcInputStreams[i]->getCurrFrame();

    for( UInt i = 1; i < m_apcInputStreams.size(); i++ )
    {
      printf( "  " );
      for( Int component = 0; component < 3; component++ )
      {
        dQuality = apcCurrFrame[i]->getQuality( m_uiOperationIndex, apcCurrFrame[0], component );
        printf( " %5.3f", dQuality );
      }
    }
    printf( "\n" );
    for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
    {
      if( !abEOF[i] )
        m_apcInputStreams[i]->readFrame();
      abEOF[i] |= m_apcInputStreams[i]->checkErrors( PlaYUVerStream::LAST_FRAME );
      m_apcInputStreams[i]->setNextFrame();
    }
  }
}

}  // NAMESPACE
