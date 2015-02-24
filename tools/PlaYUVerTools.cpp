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
#include "modules/PlaYUVerModuleIf.h"
#include "modules/PlaYUVerModuleFactory.h"

namespace plaYUVer
{

PlaYUVerTools::PlaYUVerTools()
{
  m_uiOperation = INVALID_OPERATION;
  m_uiNumberOfFrames = -1;

  m_uiQualityMetric = -1;

  m_pcCurrModuleIf = NULL;
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

Int PlaYUVerTools::openInputs()
{
  /**
   * Create input streams
   */
  if( m_cCommandLineParser.getOptionsMap().count( "input" ) )
  {
    std::string resolutionString( "" );
    if( m_cCommandLineParser.getOptionsMap().count( "size" ) )
    {
      resolutionString = m_cCommandLineParser.getOptionsMap()["size"].as<std::string>();
    }
    std::string fmtString( "yuv420p" );
    if( m_cCommandLineParser.getOptionsMap().count( "pel_fmt" ) )
    {
      fmtString = m_cCommandLineParser.getOptionsMap()["pel_fmt"].as<std::string>();
    }

    std::vector<std::string> inputFileNames = m_cCommandLineParser.getOptionsMap()["input"].as<std::vector<std::string> >();

    PlaYUVerStream* pcStream;
    for( UInt i = 0; i < inputFileNames.size(); i++ )
    {
      pcStream = new PlaYUVerStream;
      try
      {
        if( !pcStream->open( inputFileNames[i], resolutionString, fmtString, 1 ) )
        {
          printf( "Cannot open input stream %s! ", inputFileNames[i].c_str() );
          return 2;
        }
        m_apcInputStreams.push_back( pcStream );
      }
      catch( const char *msg )
      {
        printf( "Cannot open input stream %s with the following error: \n%s\n", inputFileNames[i].c_str(), msg );
        return 2;
      }
    }
    m_uiNumberOfFrames = MAX_UINT;
    if( m_cCommandLineParser.getOptionsMap().count( "frames" ) )
    {
      m_uiNumberOfFrames = m_cCommandLineParser.getOptionsMap()["frames"].as<UInt>();
    }

    m_uiNumberOfComponents = MAX_UINT;
    for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
    {
      if( m_apcInputStreams[i]->getFrameNum() < m_uiNumberOfFrames )
        m_uiNumberOfFrames = m_apcInputStreams[i]->getFrameNum();
      if( m_apcInputStreams[i]->getCurrFrame()->getNumberChannels() < m_uiNumberOfComponents )
        m_uiNumberOfComponents = m_apcInputStreams[i]->getCurrFrame()->getNumberChannels();
    }
  }
  return 0;
}

Int PlaYUVerTools::openOutputs()
{
  if( m_cCommandLineParser.getOptionsMap().count( "output" ) )
  {

  }
  return 0;
}

Int PlaYUVerTools::Open( Int argc, Char *argv[] )
{
  Int iRet = 0;

  m_cCommandLineParser.Config( argc, argv );
  if( ( iRet = m_cCommandLineParser.parseToolsArgs() ) > 0 )
  {
    return iRet;
  }

  if( openInputs() > 0 )
  {
    return 2;
  }

  /**
   * Check Quality operation
   */
  if( m_cCommandLineParser.getOptionsMap().count( "quality" ) )
  {
    std::string qualityMetric = m_cCommandLineParser.getOptionsMap()["quality"].as<std::string>();
    if( m_apcInputStreams.size() != 2 )
    {
      printf( "Invalid number of inputs! " );
      return 2;
    }
    for( UInt i = 0; i < PlaYUVerFrame::supportedQualityMetricsList().size(); i++ )
    {
      if( lowercase( PlaYUVerFrame::supportedQualityMetricsList()[i] ) == lowercase( qualityMetric ) )
      {
        m_uiQualityMetric = i;
      }
    }
    if( m_uiQualityMetric == -1 )
    {
      printf( "Invalid quality metric! " );
      return 2;
    }
    m_uiOperation = QUALITY_OPERATION;
    m_fpProcess = &PlaYUVerTools::QualityOperation;
    printf( "PlaYUVer Quality\n" );
  }

  /**
   * Check Module operation
   */
  if( m_cCommandLineParser.getOptionsMap().count( "module" ) )
  {
    std::string moduleName = m_cCommandLineParser.getOptionsMap()["module"].as<std::string>();

    PlaYUVerModuleFactoryMap& PlaYUVerModuleFactoryMap = PlaYUVerModuleFactory::Get()->getMap();
    PlaYUVerModuleFactoryMap::iterator it = PlaYUVerModuleFactoryMap.begin();
    for( UInt i = 0; it != PlaYUVerModuleFactoryMap.end(); ++it, i++ )
    {
      if( strcmp( it->first, moduleName.c_str() ) == 0 )
      {
        m_pcCurrModuleIf = it->second();
        break;
      }
    }

    if( m_apcInputStreams.size() != m_pcCurrModuleIf->m_uiNumberOfFrames )
    {
      printf( "Invalid number of inputs! " );
      return 2;
    }

    m_pcCurrModuleIf->create( m_apcInputStreams[0]->getCurrFrame() );

    m_uiOperation = MODULE_OPERATION;
    m_fpProcess = &PlaYUVerTools::ModuleOperation;
    printf( "PlaYUVer Module\n" );
  }

  if( m_uiOperation == INVALID_OPERATION )
  {
    printf( "No operation was selected! " );
    return 2;
  }
  return iRet;
}

Int PlaYUVerTools::Process()
{
  return ( this->*m_fpProcess )();
}

Int PlaYUVerTools::Close()
{
// Finish
  return 0;
}

Int PlaYUVerTools::QualityOperation()
{
  PlaYUVerFrame* apcCurrFrame[m_apcInputStreams.size()];
  Bool abEOF[m_apcInputStreams.size()];
  Double adAverageQuality[m_apcInputStreams.size() - 1][m_uiNumberOfComponents];
  Double dQuality;

  printf( "  Measuring Quality using %s ... \n", PlaYUVerFrame::supportedQualityMetricsList()[m_uiQualityMetric].c_str() );

  for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
  {
    abEOF[s] = false;
    for( UInt c = 0; c < m_uiNumberOfComponents; c++ )
    {
      adAverageQuality[s][c] = 0;
    }
  }
  for( UInt frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {
    printf( "   %3d", frame );
    for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
      apcCurrFrame[s] = m_apcInputStreams[s]->getCurrFrame();

    for( UInt s = 1; s < m_apcInputStreams.size(); s++ )
    {
      printf( "  " );
      for( UInt c = 0; c < m_uiNumberOfComponents; c++ )
      {
        dQuality = apcCurrFrame[s]->getQuality( m_uiQualityMetric, apcCurrFrame[0], c );
        adAverageQuality[s - 1][c] = ( adAverageQuality[s - 1][c] * Double( frame ) + dQuality ) / Double( frame + 1 );
        printf( "  %5.3f", dQuality );
      }
    }
    printf( "\n" );
    for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
    {
      m_apcInputStreams[s]->setNextFrame();
      if( !abEOF[s] )
      {
        m_apcInputStreams[s]->readFrame();
        abEOF[s] = m_apcInputStreams[s]->checkErrors( PlaYUVerStream::LAST_FRAME );
      }
    }
  }
  printf( "\n  Mean Values: \n        " );
  for( UInt s = 0; s < m_apcInputStreams.size() - 1; s++ )
  {
    for( UInt c = 0; c < m_uiNumberOfComponents; c++ )
    {
      printf( "  %5.3f", adAverageQuality[s][c] );
    }
  }
  printf( "\n" );
  return 0;
}

Int PlaYUVerTools::ModuleOperation()
{
  printf( "  Applying Module %s/%s ... \n", m_pcCurrModuleIf->m_pchModuleCategory, m_pcCurrModuleIf->m_pchModuleName );

  PlaYUVerFrame* pcProcessedFrame = NULL;
  Double dMeasurementResult = 0.0;
  Double dAveragedMeasurementResult = 0;
  Bool abEOF[m_apcInputStreams.size()];

  for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
  {
    abEOF[s] = false;
  }

  for( UInt frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {

    if( m_pcCurrModuleIf->m_iModuleType == FRAME_PROCESSING_MODULE )
    {
      switch( m_pcCurrModuleIf->m_uiNumberOfFrames )
      {
      case MODULE_REQUIRES_ONE_FRAME:
        pcProcessedFrame = m_pcCurrModuleIf->process( m_apcInputStreams[0]->getCurrFrame() );
        break;
      case MODULE_REQUIRES_TWO_FRAMES:
        pcProcessedFrame = m_pcCurrModuleIf->process( m_apcInputStreams[0]->getCurrFrame(), m_apcInputStreams[1]->getCurrFrame() );
        break;
      }
    }
    else if( m_pcCurrModuleIf->m_iModuleType == FRAME_MEASUREMENT_MODULE )
    {
      switch( m_pcCurrModuleIf->m_uiNumberOfFrames )
      {
      case MODULE_REQUIRES_ONE_FRAME:
        dMeasurementResult = m_pcCurrModuleIf->measure( m_apcInputStreams[0]->getCurrFrame() );
        break;
      case MODULE_REQUIRES_TWO_FRAMES:
        dMeasurementResult = m_pcCurrModuleIf->measure( m_apcInputStreams[0]->getCurrFrame(), m_apcInputStreams[1]->getCurrFrame() );
        break;
      }
      printf( "   %3d  %8.3f \n", frame, dMeasurementResult );
      dAveragedMeasurementResult = ( dAveragedMeasurementResult * Double( frame ) + dMeasurementResult ) / Double( frame + 1 );
    }

    for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
    {
      m_apcInputStreams[s]->setNextFrame();
      if( !abEOF[s] )
      {
        m_apcInputStreams[s]->readFrame();
        abEOF[s] = m_apcInputStreams[s]->checkErrors( PlaYUVerStream::LAST_FRAME );
      }
    }
  }

  if( m_pcCurrModuleIf->m_iModuleType == FRAME_MEASUREMENT_MODULE )
  {
    printf( "\n  Mean Value: \n        %8.3f\n", dAveragedMeasurementResult );
  }

  return 0;
}

}  // NAMESPACE
