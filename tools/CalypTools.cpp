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
 * \file     CalypTools.cpp
 * \brief    Main definition of the CalypTools APp
 */

#include "CalypTools.h"
#include "config.h"

#include <climits>
#include <cstring>

#include "lib/CalypFrame.h"
#include "lib/CalypModuleIf.h"
#include "lib/CalypStream.h"
#include "modules/CalypModulesFactory.h"

CalypTools::CalypTools()
{
  m_bVerbose = true;
  m_uiOperation = INVALID_OPERATION;
  m_uiNumberOfFrames = -1;

  m_uiQualityMetric = -1;

  m_pcCurrModuleIf = NULL;
}

CalypTools::~CalypTools()
{
  for( unsigned int i = 0; i < m_apcInputStreams.size(); i++ )
  {
    m_apcInputStreams[i]->close();
  }
  for( unsigned int i = 0; i < m_apcOutputStreams.size(); i++ )
  {
    m_apcOutputStreams[i]->close();
  }
}

#define GET_PARAM( X, i ) X[X.size() > i ? i : X.size() - 1]

int CalypTools::openInputs()
{
  /**
   * Create input streams
   */
  if( Opts().hasOpt( "input" ) )
  {
    std::vector<ClpString> inputFileNames = m_apcInputs;

    ClpString resolutionString( "" );
    ClpString fmtString( "yuv420p" );
    unsigned int uiBitPerPixel = 8;
    unsigned int uiEndianness = 0;

    CalypStream* pcStream;
    for( unsigned int i = 0; i < inputFileNames.size() && i < MAX_NUMBER_INPUTS; i++ )
    {
      if( Opts().hasOpt( "size" ) )
      {
        resolutionString = GET_PARAM( m_strResolution, 0 );
      }
      if( Opts().hasOpt( "pel_fmt" ) )
      {
        fmtString = GET_PARAM( m_strPelFmt, i );
      }
      if( Opts().hasOpt( "bits_pel" ) )
      {
        uiBitPerPixel = GET_PARAM( m_uiBitsPerPixel, i );
      }
      if( Opts().hasOpt( "endianness" ) )
      {
        if( GET_PARAM( m_strEndianness, i ) == "big" )
        {
          uiEndianness = 0;
        }
        if( GET_PARAM( m_strEndianness, i ) == "little" )
        {
          uiEndianness = 1;
        }
      }
      pcStream = new CalypStream;
      try
      {
        if( !pcStream->open( inputFileNames[i], resolutionString, fmtString, uiBitPerPixel, uiEndianness, 1, true ) )
        {
          log( CLP_LOG_ERROR, "Cannot open input stream %s! ", inputFileNames[i].c_str() );
          return 2;
        }
        m_apcInputStreams.push_back( pcStream );
      }
      catch( const char* msg )
      {
        log( CLP_LOG_ERROR, "Cannot open input stream %s with the following error: \n%s\n", inputFileNames[i].c_str(),
             msg );
        return 2;
      }
    }
  }

  m_uiNumberOfFrames = UINT_MAX;
  if( Opts().hasOpt( "frames" ) )
  {
    m_uiNumberOfFrames = m_iFrames;
  }

  m_uiNumberOfComponents = UINT_MAX;
  for( unsigned int i = 0; i < m_apcInputStreams.size(); i++ )
  {
    m_uiNumberOfFrames = std::min( m_uiNumberOfFrames, m_apcInputStreams[i]->getFrameNum() );
    m_uiNumberOfComponents =
        std::min( m_uiNumberOfComponents, m_apcInputStreams[i]->getCurrFrame()->getNumberChannels() );
  }

  return 0;
}

int CalypTools::Open( int argc, char* argv[] )
{
  int iRet = 0;

  if( ( iRet = parseToolsArgs( argc, argv ) ) > 0 )
  {
    return iRet;
  }

  if( openInputs() > 0 )
  {
    return 2;
  }

  if( Opts().hasOpt( "save" ) )
  {
    if( m_apcInputStreams.size() == 0 )
    {
      log( CLP_LOG_ERROR, "Invalid number of input streams! " );
      return 2;
    }
    long long int currFrames = 0;
    long long int numberOfFrames = LONG_MAX;
    for( unsigned int i = 0; i < m_apcInputStreams.size(); i++ )
    {
      currFrames = m_apcInputStreams[i]->getFrameNum();
      if( currFrames < numberOfFrames )
        numberOfFrames = currFrames;
    }
    m_iFrameNum = m_iFrames;
    if( !( m_iFrameNum >= 0 && m_iFrameNum < numberOfFrames ) )
    {
      log( CLP_LOG_ERROR, "Invalid frame number! Use --frame option " );
      return 2;
    }
    if( Opts().hasOpt( "output" ) )
      m_pcOutputFileNames.push_back( m_strOutput );
    if( m_pcOutputFileNames.size() != m_apcInputStreams.size() )
    {
      log( CLP_LOG_ERROR, "Invalid number of outputs! Each input must have an "
                          "output filename. " );
      return 2;
    }

    m_uiOperation = SAVE_OPERATION;
    m_fpProcess = &CalypTools::SaveOperation;
    log( CLP_LOG_INFO, "Calyp Save Frame\n" );
  }

  if( Opts().hasOpt( "rate-reduction" ) )
  {
    if( m_apcInputStreams.size() == 0 )
    {
      log( CLP_LOG_ERROR, "Invalid number of input streams! " );
      return 2;
    }
    long long int currFrames = 0;
    long long int numberOfFrames = LONG_MAX;
    for( unsigned int i = 0; i < m_apcInputStreams.size(); i++ )
    {
      currFrames = m_apcInputStreams[i]->getFrameNum();
      if( currFrames < numberOfFrames )
        numberOfFrames = currFrames;
    }
    if( m_iRateReductionFactor <= 0 )
    {
      log( CLP_LOG_ERROR, "Invalid frame rate reduction value!" );
      return 2;
    }

    if( Opts().hasOpt( "output" ) )
      m_pcOutputFileNames.push_back( m_strOutput );

    CalypFrame* pcInputFrame = m_apcInputStreams[0]->getCurrFrame();
    CalypStream* pcOutputStream = new CalypStream;
    try
    {
      pcOutputStream->open( m_pcOutputFileNames[0], pcInputFrame->getWidth(), pcInputFrame->getHeight(),
                            pcInputFrame->getPelFormat(), pcInputFrame->getBitsPel(), CLP_LITTLE_ENDIAN, 1,
                            false );
    }
    catch( const char* msg )
    {
      log( CLP_LOG_ERROR, "Cannot open input stream %s with the following error %s!\n", m_pcOutputFileNames[0].c_str(),
           msg );
      delete pcOutputStream;
      pcOutputStream = NULL;
      return 2;
    }
    m_apcOutputStreams.push_back( pcOutputStream );

    if( m_apcOutputStreams.size() != m_apcInputStreams.size() )
    {
      log( CLP_LOG_ERROR, "Invalid number of outputs! Each input must have an "
                          "output filename. " );
      return 2;
    }

    m_uiOperation = RATE_REDUCTION_OPERATION;
    m_fpProcess = &CalypTools::RateReductionOperation;
    log( CLP_LOG_INFO, "Calyp Frame Rate Reduction\n" );
  }

  /**
   * Check Quality operation
   */
  if( Opts().hasOpt( "quality" ) )
  {
    ClpString qualityMetric = m_strQualityMetric;
    if( m_apcInputStreams.size() < 2 )
    {
      log( CLP_LOG_ERROR, "Invalid number of inputs! " );
      return 2;
    }
    for( unsigned int i = 0; i < CalypFrame::supportedQualityMetricsList().size(); i++ )
    {
      if( clpLowercase( CalypFrame::supportedQualityMetricsList()[i] ) == clpLowercase( qualityMetric ) )
      {
        m_uiQualityMetric = i;
      }
    }
    if( m_uiQualityMetric == -1 )
    {
      log( CLP_LOG_ERROR, "Invalid quality metric! " );
      return 2;
    }
    m_uiOperation = QUALITY_OPERATION;
    m_fpProcess = &CalypTools::QualityOperation;
    log( CLP_LOG_INFO, "Calyp Quality\n" );
  }

  /**
   * Check Module operation
   */
  if( Opts().hasOpt( "module" ) )
  {
    ClpString moduleName = m_strModule;

    CalypModulesFactoryMap& moduleFactoryMap = CalypModulesFactory::Get()->getMap();
    CalypModulesFactoryMap::iterator it = moduleFactoryMap.begin();
    for( unsigned int i = 0; it != moduleFactoryMap.end(); ++it, i++ )
    {
      if( strcmp( it->first, moduleName.c_str() ) == 0 )
      {
        m_pcCurrModuleIf = it->second();
        break;
      }
    }
    if( !m_pcCurrModuleIf )
    {
      log( CLP_LOG_ERROR, "Invalid module! " );
      return 2;
    }

    if( m_apcInputStreams.size() != m_pcCurrModuleIf->m_uiNumberOfFrames )
    {
      log( CLP_LOG_ERROR, "Invalid number of inputs! " );
      return 2;
    }

    m_pcCurrModuleIf->m_cModuleOptions.parse( argc, argv );

    // Create Module
    bool moduleCreated = false;
    std::vector<CalypFrame*> apcFrameList;
    for( unsigned int i = 0; i < m_pcCurrModuleIf->m_uiNumberOfFrames; i++ )
    {
      apcFrameList.push_back( m_apcInputStreams[i]->getCurrFrame() );
    }
    if( m_pcCurrModuleIf->m_iModuleAPI == CLP_MODULE_API_2 )
    {
      moduleCreated = m_pcCurrModuleIf->create( apcFrameList );
    }
    else if( m_pcCurrModuleIf->m_iModuleAPI == CLP_MODULE_API_1 )
    {
      m_pcCurrModuleIf->create( m_apcInputStreams[0]->getCurrFrame() );
      moduleCreated = true;
    }
    if( !moduleCreated )
    {
      log( CLP_LOG_ERROR, "Module is not supported with the selected inputs! " );
      return 2;
    }

    if( m_pcCurrModuleIf->m_iModuleType == CLP_FRAME_PROCESSING_MODULE )
    {
      // Check outputs
      std::vector<ClpString> outputFileNames;
      if( Opts().hasOpt( "output" ) )
        outputFileNames.push_back( m_strOutput );

      if( outputFileNames.size() == 1 )
      {
        CalypFrame* pcModFrame;
        if( m_pcCurrModuleIf->m_iModuleAPI == CLP_MODULE_API_2 )
        {
          pcModFrame = m_pcCurrModuleIf->process( apcFrameList );
        }
        else
        {
          pcModFrame = applyFrameModule();
        }
        CalypStream* pcModStream = new CalypStream;
        try
        {
          pcModStream->open( outputFileNames[0], pcModFrame->getWidth(), pcModFrame->getHeight(),
                             pcModFrame->getPelFormat(), pcModFrame->getBitsPel(), CLP_LITTLE_ENDIAN, 1, false );
        }
        catch( const char* msg )
        {
          log( CLP_LOG_ERROR, "Cannot open input stream %s with the following error %s!\n", outputFileNames[0].c_str(),
               msg );
          delete pcModStream;
          pcModStream = NULL;
          return 2;
        }
        m_apcOutputStreams.push_back( pcModStream );
      }
      else
      {
        log( CLP_LOG_ERROR, "One output is required! " );
        return 2;
      }
    }

    m_uiOperation = MODULE_OPERATION;
    m_fpProcess = &CalypTools::ModuleOperation;
    log( CLP_LOG_INFO, "Calyp Module\n" );
  }

  if( m_uiOperation == INVALID_OPERATION )
  {
    log( CLP_LOG_ERROR, "No operation was selected! " );
    return 2;
  }
  return iRet;
}

int CalypTools::Process()
{
  return ( this->*m_fpProcess )();
}

int CalypTools::Close()
{
  // Finish
  return 0;
}

int CalypTools::SaveOperation()
{
  bool bRet = true;
  for( unsigned int s = 0; s < m_apcInputStreams.size(); s++ )
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

int CalypTools::RateReductionOperation()
{
  bool abEOF;
  log( CLP_LOG_INFO, "\n Reducing frame rate by a factor of %d ... ", m_iRateReductionFactor );
  for( unsigned int frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {
    log( CLP_LOG_INFO, "\n Reading frame %d ... ", frame );
    if( ( frame % m_iRateReductionFactor ) == 0 )
    {
      log( CLP_LOG_INFO, "Writing", frame );
      m_apcOutputStreams[0]->writeFrame( m_apcInputStreams[0]->getCurrFrame() );
    }
    abEOF = m_apcInputStreams[0]->setNextFrame();
    if( !abEOF )
    {
      m_apcInputStreams[0]->readNextFrame();
    }
  }
  return 0;
}

int CalypTools::QualityOperation()
{
  const char* pchQualityMetricName = CalypFrame::supportedQualityMetricsList()[m_uiQualityMetric].c_str();
  CalypFrame* apcCurrFrame[MAX_NUMBER_INPUTS];
  bool abEOF[MAX_NUMBER_INPUTS];
  double adAverageQuality[MAX_NUMBER_INPUTS - 1][CalypPixel::getMaxNumberOfComponents()];
  double dQuality;

  ClpString metric_fmt = " ";
  switch( m_uiQualityMetric )
  {
  case CalypFrame::PSNR_METRIC:
    //"PSNR_0_0"
    metric_fmt += " %6.3f ";
    break;
  case CalypFrame::SSIM_METRIC:
    //"SSIM_0_0"
    metric_fmt += " %6.4f ";
    break;
  case CalypFrame::MSE_METRIC:
    //"MSE_0_0"
    metric_fmt += "%7.2f";
    break;
  default:
    metric_fmt += " %6.3f ";
  }
  metric_fmt += " ";

  log( CLP_LOG_INFO, "  Measuring Quality using %s ... \n", pchQualityMetricName );
  log( CLP_LOG_INFO, "# Frame   ", pchQualityMetricName );

  for( unsigned int s = 1; s < m_apcInputStreams.size(); s++ )
  {
    for( unsigned int c = 0; c < m_uiNumberOfComponents; c++ )
    {
      log( CLP_LOG_INFO, "%s_%d_%d  ", pchQualityMetricName, s, c );
    }
    log( CLP_LOG_INFO, "   " );
  }

  log( CLP_LOG_INFO, "\n" );

  for( unsigned int s = 0; s < m_apcInputStreams.size(); s++ )
  {
    abEOF[s] = false;
    for( unsigned int c = 0; c < m_uiNumberOfComponents; c++ )
    {
      adAverageQuality[s][c] = 0;
    }
  }
  for( unsigned int frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {
    log( CLP_LOG_INFO, "  %3d  ", frame );
    for( unsigned int s = 0; s < m_apcInputStreams.size(); s++ )
      apcCurrFrame[s] = m_apcInputStreams[s]->getCurrFrame();

    for( unsigned int s = 1; s < m_apcInputStreams.size(); s++ )
    {
      log( CLP_LOG_RESULT, "  " );
      for( unsigned int c = 0; c < m_uiNumberOfComponents; c++ )
      {
        dQuality = apcCurrFrame[s]->getQuality( m_uiQualityMetric, apcCurrFrame[0], c );
        adAverageQuality[s - 1][c] = ( adAverageQuality[s - 1][c] * double( frame ) + dQuality ) / double( frame + 1 );
        log( CLP_LOG_RESULT, metric_fmt.c_str(), dQuality );
      }
      log( CLP_LOG_RESULT, " " );
    }
    log( CLP_LOG_RESULT, "\n" );
    for( unsigned int s = 0; s < m_apcInputStreams.size(); s++ )
    {
      abEOF[s] = m_apcInputStreams[s]->setNextFrame();
      if( !abEOF[s] )
      {
        m_apcInputStreams[s]->readNextFrame();
      }
    }
  }
  log( CLP_LOG_INFO, "\n  Mean Values: \n         " );
  for( unsigned int s = 0; s < m_apcInputStreams.size() - 1; s++ )
  {
    for( unsigned int c = 0; c < m_uiNumberOfComponents; c++ )
    {
      log( CLP_LOG_INFO, metric_fmt.c_str(), adAverageQuality[s][c] );
    }
    log( CLP_LOG_RESULT, "   " );
  }
  log( CLP_LOG_INFO, "\n" );
  return 0;
}

CalypFrame* CalypTools::applyFrameModule()
{
  CalypFrame* pcProcessedFrame = NULL;
  if( m_pcCurrModuleIf->m_iModuleType == CLP_FRAME_PROCESSING_MODULE )
  {
    pcProcessedFrame = m_pcCurrModuleIf->process( m_apcInputStreams[0]->getCurrFrame() );
  }
  return pcProcessedFrame;
}

int CalypTools::ModuleOperation()
{
  std::vector<CalypFrame*> apcFrameList;
  log( CLP_LOG_INFO, "  Applying Module %s/%s ...\n", m_pcCurrModuleIf->m_pchModuleCategory,
       m_pcCurrModuleIf->m_pchModuleName );

  CalypFrame* pcProcessedFrame = NULL;
  double dMeasurementResult = 0.0;
  double dAveragedMeasurementResult = 0;
  bool abEOF[MAX_NUMBER_INPUTS];

  for( unsigned int s = 0; s < m_apcInputStreams.size(); s++ )
  {
    abEOF[s] = false;
  }

  for( unsigned int frame = 0; frame < m_uiNumberOfFrames; frame++ )
  {
    apcFrameList.clear();
    for( unsigned int i = 0; i < m_pcCurrModuleIf->m_uiNumberOfFrames; i++ )
    {
      apcFrameList.push_back( m_apcInputStreams[i]->getCurrFrame() );
    }
    if( m_pcCurrModuleIf->m_iModuleType == CLP_FRAME_PROCESSING_MODULE )
    {
      if( m_pcCurrModuleIf->m_iModuleAPI == CLP_MODULE_API_2 )
      {
        pcProcessedFrame = m_pcCurrModuleIf->process( apcFrameList );
      }
      else
      {
        pcProcessedFrame = applyFrameModule();
      }
      m_apcOutputStreams[0]->writeFrame( pcProcessedFrame );
    }
    else if( m_pcCurrModuleIf->m_iModuleType == CLP_FRAME_MEASUREMENT_MODULE )
    {
      if( m_pcCurrModuleIf->m_iModuleAPI == CLP_MODULE_API_2 )
      {
        dMeasurementResult = m_pcCurrModuleIf->measure( apcFrameList );
      }
      else
      {
        dMeasurementResult = m_pcCurrModuleIf->measure( m_apcInputStreams[0]->getCurrFrame() );
      }
      log( CLP_LOG_INFO, "   %3d", frame );
      log( CLP_LOG_RESULT, "  %8.3f \n", dMeasurementResult );
      dAveragedMeasurementResult =
          ( dAveragedMeasurementResult * double( frame ) + dMeasurementResult ) / double( frame + 1 );
    }

    for( unsigned int s = 0; s < m_apcInputStreams.size(); s++ )
    {
      abEOF[s] = m_apcInputStreams[s]->setNextFrame();
      if( !abEOF[s] )
      {
        m_apcInputStreams[s]->readNextFrame();
      }
    }
  }

  if( m_pcCurrModuleIf->m_iModuleType == CLP_FRAME_MEASUREMENT_MODULE )
  {
    log( CLP_LOG_INFO, "\n  Mean Value: \n        %8.3f\n", dAveragedMeasurementResult );
  }

  return 0;
}
