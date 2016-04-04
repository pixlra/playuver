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

#include <cstring>
#include <climits>
#include "PlaYUVerTools.h"
#include "lib/PlaYUVerModuleIf.h"
#include "modules/PlaYUVerModuleFactory.h"

namespace plaYUVer
{

PlaYUVerTools::PlaYUVerTools()
{
  m_bVerbose = true;
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
  if( m_cCmdLineHandler.Opts()["input"]->count() )
  {
    std::string resolutionString( "" );
    if( m_cCmdLineHandler.Opts()["size"]->count() )
    {
      resolutionString = m_cCmdLineHandler.m_strResolution;
    }
    std::string fmtString( "yuv420p" );
    if( m_cCmdLineHandler.Opts()["pel_fmt"]->count() )
    {
      fmtString = m_cCmdLineHandler.m_strPelFmt;
    }
    UInt uiBitPerPixel = 8;
    if( m_cCmdLineHandler.Opts()["bits_pel"]->count() )
    {
      uiBitPerPixel = m_cCmdLineHandler.m_uiBitsPerPixel;
    }
    UInt uiEndianness = 0;
    if( m_cCmdLineHandler.Opts()["endianness"]->count() )
    {
      if( m_cCmdLineHandler.m_strEndianness == "big" )
      {
        uiEndianness = 0;
      }
      if( m_cCmdLineHandler.m_strEndianness == "little" )
      {
        uiEndianness = 1;
      }
    }

    std::vector<std::string> inputFileNames = m_cCmdLineHandler.m_apcInputs;

    PlaYUVerStream* pcStream;
    for( UInt i = 0; i < inputFileNames.size() && i < MAX_NUMBER_INPUTS; i++ )
    {
      pcStream = new PlaYUVerStream;
      try
      {
        if( !pcStream->open( inputFileNames[i], resolutionString, fmtString, uiBitPerPixel, uiEndianness, 1 ) )
        {
          m_cCmdLineHandler.log( LOG_ERROR, "Cannot open input stream %s! ", inputFileNames[i].c_str() );
          return 2;
        }
        m_apcInputStreams.push_back( pcStream );
      }
      catch( const char *msg )
      {
        m_cCmdLineHandler.log( LOG_ERROR, "Cannot open input stream %s with the following error: \n%s\n", inputFileNames[i].c_str(), msg );
        return 2;
      }
    }
  }

  m_uiNumberOfFrames = MAX_UINT;
  if( m_cCmdLineHandler.Opts()["frames"]->count() )
  {
    m_uiNumberOfFrames = m_cCmdLineHandler.m_iFrames;
  }

  m_uiNumberOfComponents = MAX_UINT;
  for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
  {
    if( m_apcInputStreams[i]->getFrameNum() < m_uiNumberOfFrames )
      m_uiNumberOfFrames = m_apcInputStreams[i]->getFrameNum();
    if( m_apcInputStreams[i]->getCurrFrame()->getNumberChannels() < m_uiNumberOfComponents )
      m_uiNumberOfComponents = m_apcInputStreams[i]->getCurrFrame()->getNumberChannels();
  }

  return 0;
}

Int PlaYUVerTools::Open( Int argc, Char *argv[] )
{
  Int iRet = 0;

  if( ( iRet = m_cCmdLineHandler.parseToolsArgs( argc, argv ) ) > 0 )
  {
    return iRet;
  }

  if( openInputs() > 0 )
  {
    return 2;
  }

  if( m_cCmdLineHandler.Opts()["save"]->count() )
  {
    if( m_apcInputStreams.size() == 0 )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid number of input streams! " );
      return 2;
    }
    Int64 currFrames = 0;
    Int64 numberOfFrames = LONG_MAX;
    for( UInt i = 0; i < m_apcInputStreams.size(); i++ )
    {
      currFrames = m_apcInputStreams[i]->getFrameNum();
      if( currFrames < numberOfFrames )
        numberOfFrames = currFrames;
    }
    m_iFrameNum = m_cCmdLineHandler.m_iFrames;
    if( !( m_iFrameNum >= 0 && m_iFrameNum < numberOfFrames ) )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid frame number! Use --frame option " );
      return 2;
    }
    if( m_cCmdLineHandler.Opts()["output"]->count() )
      m_pcOutputFileNames.push_back( m_cCmdLineHandler.m_strOutput );
    if( m_pcOutputFileNames.size() != m_apcInputStreams.size() )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid number of outputs! Each input must have an output filename. " );
      return 2;
    }

    m_uiOperation = SAVE_OPERATION;
    m_fpProcess = &PlaYUVerTools::SaveOperation;
    m_cCmdLineHandler.log( LOG_INFO, "PlaYUVer Save Frame\n" );
  }

  /**
   * Check Quality operation
   */
  if( m_cCmdLineHandler.Opts()["quality"]->count() )
  {
    std::string qualityMetric = m_cCmdLineHandler.m_strQualityMetric;
    if( m_apcInputStreams.size() < 2 )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid number of inputs! " );
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
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid quality metric! " );
      return 2;
    }
    m_uiOperation = QUALITY_OPERATION;
    m_fpProcess = &PlaYUVerTools::QualityOperation;
    m_cCmdLineHandler.log( LOG_INFO, "PlaYUVer Quality\n" );
  }

  /**
   * Check Module operation
   */
  if( m_cCmdLineHandler.Opts()["module"]->count() )
  {
    std::string moduleName = m_cCmdLineHandler.m_strModule;

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
    if( !m_pcCurrModuleIf )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid module! " );
      return 2;
    }

    if( m_apcInputStreams.size() != m_pcCurrModuleIf->m_uiNumberOfFrames )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Invalid number of inputs! " );
      return 2;
    }

    m_pcCurrModuleIf->m_cModuleOptions.scanArgv( argc, ( const Char** )argv );

    // Create Module
    Bool moduleCreated = false;
    std::vector<PlaYUVerFrame*> apcFrameList;
    for( UInt i = 0; i < m_pcCurrModuleIf->m_uiNumberOfFrames; i++ )
    {
      apcFrameList.push_back( m_apcInputStreams[i]->getCurrFrame() );
    }
    if( m_pcCurrModuleIf->m_iModuleAPI == MODULE_API_2 )
    {
      moduleCreated = m_pcCurrModuleIf->create( apcFrameList );
    }
    else if( m_pcCurrModuleIf->m_iModuleAPI == MODULE_API_1 )
    {
      m_pcCurrModuleIf->create( m_apcInputStreams[0]->getCurrFrame() );
      moduleCreated = true;
    }
    if( !moduleCreated )
    {
      m_cCmdLineHandler.log( LOG_ERROR, "Module is not supported with the selected inputs! " );
      return 2;
    }

    if( m_pcCurrModuleIf->m_iModuleType == FRAME_PROCESSING_MODULE )
    {
      // Check outputs
      std::vector<std::string> outputFileNames;
      if( m_cCmdLineHandler.Opts()["output"]->count() )
        outputFileNames.push_back( m_cCmdLineHandler.m_strOutput );

      if( outputFileNames.size() == 1 )
      {
        PlaYUVerFrame* pcModFrame;
        if( m_pcCurrModuleIf->m_iModuleAPI == MODULE_API_2 )
        {
          pcModFrame = m_pcCurrModuleIf->process( apcFrameList );
        }
        else
        {
          pcModFrame = applyFrameModule();
        }
        PlaYUVerStream* pcModStream = new PlaYUVerStream;
        if( !pcModStream->open( outputFileNames[0], pcModFrame->getWidth(), pcModFrame->getHeight(), pcModFrame->getPelFormat(), pcModFrame->getBitsPel(), 1, false ) )
        {
          delete pcModStream;
          pcModStream = NULL;
          return 2;
        }
        m_apcOutputStreams.push_back( pcModStream );
      }
      else
      {
        m_cCmdLineHandler.log( LOG_ERROR, "One output is required! " );
        return 2;
      }
    }

    m_uiOperation = MODULE_OPERATION;
    m_fpProcess = &PlaYUVerTools::ModuleOperation;
    m_cCmdLineHandler.log( LOG_INFO, "PlaYUVer Module\n" );
  }

  if( m_uiOperation == INVALID_OPERATION )
  {
    m_cCmdLineHandler.log( LOG_ERROR, "No operation was selected! " );
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

Int PlaYUVerTools::SaveOperation()
{
  Bool bRet = true;
  for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
  {
    bRet = m_apcInputStreams[s]->seekInput( m_iFrameNum );
    if( bRet == false )
    {
      return 2;
    }
    m_apcInputStreams[s]->saveFrame( m_pcOutputFileNames[s] );
  }
  return 0;
}

Int PlaYUVerTools::QualityOperation()
{
  PlaYUVerFrame* apcCurrFrame[MAX_NUMBER_INPUTS];
  Bool abEOF[MAX_NUMBER_INPUTS];
  Double adAverageQuality[MAX_NUMBER_INPUTS - 1][MAX_NUMBER_COMPONENTS];
  Double dQuality;

  m_cCmdLineHandler.log( LOG_INFO, "  Measuring Quality using %s ... \n", PlaYUVerFrame::supportedQualityMetricsList()[m_uiQualityMetric].c_str() );

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
    m_cCmdLineHandler.log( LOG_INFO, "   %3d", frame );
    for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
      apcCurrFrame[s] = m_apcInputStreams[s]->getCurrFrame();

    for( UInt s = 1; s < m_apcInputStreams.size(); s++ )
    {
      m_cCmdLineHandler.log( LOG_RESULT, "  " );
      for( UInt c = 0; c < m_uiNumberOfComponents; c++ )
      {
        dQuality = apcCurrFrame[s]->getQuality( m_uiQualityMetric, apcCurrFrame[0], c );
        adAverageQuality[s - 1][c] = ( adAverageQuality[s - 1][c] * Double( frame ) + dQuality ) / Double( frame + 1 );
        m_cCmdLineHandler.log( LOG_RESULT, "  %5.3f", dQuality );
      }
    }
    m_cCmdLineHandler.log( LOG_RESULT, "\n" );
    for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
    {
      abEOF[s] = m_apcInputStreams[s]->setNextFrame();
      if( !abEOF[s] )
      {
        m_apcInputStreams[s]->readFrame();
      }
    }
  }
  m_cCmdLineHandler.log( LOG_INFO, "\n  Mean Values: \n        " );
  for( UInt s = 0; s < m_apcInputStreams.size() - 1; s++ )
  {
    for( UInt c = 0; c < m_uiNumberOfComponents; c++ )
    {
      m_cCmdLineHandler.log( LOG_INFO, "  %5.3f", adAverageQuality[s][c] );
    }
    m_cCmdLineHandler.log( LOG_RESULT, "  " );
  }
  m_cCmdLineHandler.log( LOG_INFO, "\n" );
  return 0;
}

PlaYUVerFrame* PlaYUVerTools::applyFrameModule()
{
  PlaYUVerFrame* pcProcessedFrame = NULL;
  if( m_pcCurrModuleIf->m_iModuleType == FRAME_PROCESSING_MODULE )
  {
    pcProcessedFrame = m_pcCurrModuleIf->process( m_apcInputStreams[0]->getCurrFrame() );
  }
  return pcProcessedFrame;
}

Int PlaYUVerTools::ModuleOperation()
{
  std::vector<PlaYUVerFrame*> apcFrameList;
  m_cCmdLineHandler.log( LOG_INFO, "  Applying Module %s/%s ...\n", m_pcCurrModuleIf->m_pchModuleCategory, m_pcCurrModuleIf->m_pchModuleName );

  PlaYUVerFrame* pcProcessedFrame = NULL;
  Double dMeasurementResult = 0.0;
  Double dAveragedMeasurementResult = 0;
  Bool abEOF[MAX_NUMBER_INPUTS];

  for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
  {
    abEOF[s] = false;
  }

  for( UInt frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {
    apcFrameList.clear();
    for( UInt i = 0; i < m_pcCurrModuleIf->m_uiNumberOfFrames; i++ )
    {
      apcFrameList.push_back( m_apcInputStreams[i]->getCurrFrame() );
    }
    if( m_pcCurrModuleIf->m_iModuleType == FRAME_PROCESSING_MODULE )
    {
      if( m_pcCurrModuleIf->m_iModuleAPI == MODULE_API_2 )
      {
        pcProcessedFrame = m_pcCurrModuleIf->process( apcFrameList );
      }
      else
      {
        pcProcessedFrame = applyFrameModule();
      }
      m_apcOutputStreams[0]->writeFrame( pcProcessedFrame );
    }
    else if( m_pcCurrModuleIf->m_iModuleType == FRAME_MEASUREMENT_MODULE )
    {
      if( m_pcCurrModuleIf->m_iModuleAPI == MODULE_API_2 )
      {
        dMeasurementResult = m_pcCurrModuleIf->measure( apcFrameList );
      }
      else
      {
        dMeasurementResult = m_pcCurrModuleIf->measure( m_apcInputStreams[0]->getCurrFrame() );
      }
      m_cCmdLineHandler.log( LOG_INFO, "   %3d", frame );
      m_cCmdLineHandler.log( LOG_RESULT, "  %8.3f \n", dMeasurementResult );
      dAveragedMeasurementResult = ( dAveragedMeasurementResult * Double( frame ) + dMeasurementResult ) / Double( frame + 1 );
    }

    for( UInt s = 0; s < m_apcInputStreams.size(); s++ )
    {
      abEOF[s] = m_apcInputStreams[s]->setNextFrame();
      if( !abEOF[s] )
      {
        m_apcInputStreams[s]->readFrame();
      }
    }
  }

  if( m_pcCurrModuleIf->m_iModuleType == FRAME_MEASUREMENT_MODULE )
  {
    m_cCmdLineHandler.log( LOG_INFO, "\n  Mean Value: \n        %8.3f\n", dAveragedMeasurementResult );
  }

  return 0;
}

}  // NAMESPACE
