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
 * \file     PlaYUVerToolsCmdParser.cpp
 * \brief    Handle for command line
 */

#include "PlaYUVerToolsCmdParser.h"

#include <cstdarg>
#include <cstring>

#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerModuleIf.h"
#include "lib/PlaYUVerStream.h"
#include "modules/PlaYUVerModuleFactory.h"

PlaYUVerToolsCmdParser::PlaYUVerToolsCmdParser()
{
  m_uiLogLevel = 0;
  m_bQuiet = false;
  m_iFrames = -1;

  m_cOptions.addDefaultOptions();
}

PlaYUVerToolsCmdParser::~PlaYUVerToolsCmdParser() {}

Void PlaYUVerToolsCmdParser::log( UInt level, const char* fmt, ... )
{
  if( level >= m_uiLogLevel )
  {
    std::va_list args;
    va_start( args, fmt );
    vfprintf( stdout, fmt, args );
    va_end( args );
  }
}

Int PlaYUVerToolsCmdParser::parseToolsArgs( Int argc, Char* argv[] )
{
  Int iRet = 0;

  m_cOptions.addOptions()                                                                /**/
      ( "quiet,q", m_bQuiet, "disable verbose" )( "input,i", m_apcInputs, "input file" ) /**/
      ( "output,o", m_strOutput, "output file" )                                         /**/
      ( "size,s", m_strResolution, "size (WxH)" )                                        /**/
      ( "pel_fmt,p", m_strPelFmt, "pixel format" )                                       /**/
      ( "bits_pel", m_uiBitsPerPixel, "bits per pixel" )                                 /**/
      ( "endianness", m_strEndianness, "File endianness (big, little)" )                 /**/
      ( "frames,f", m_iFrames, "number of frames to parse" )                             /**/
      ( "quality", m_strQualityMetric, "select a quality metric" )                       /**/
      ( "module", m_strModule, "select a module (use internal name)" )                   /**/
      ( "save", "save a specific frame" )                                                /**/
      ( "rate-reduction", m_iRateReductionFactor, "reduce the frame rate" );             /**/

  if( !m_cOptions.parse( argc, argv ) )
  {
    iRet = 1;
  }

  if( m_bQuiet )
  {
    m_uiLogLevel = LOG_RESULT;
  }

  if( m_cOptions.hasOpt( "module" ) && m_cOptions.hasOpt( "help" ) )
  {
    listModuleHelp();
    iRet = 1;
  }
  else if( m_cOptions.hasOpt( "help" ) )
  {
    printf( "Usage: %s modules/quality/save [options] -input=input_file "
            "[--output=output_file]\n",
            argv[0] );
    m_cOptions.doHelp( std::cout );
    iRet = 1;
  }
  return iRet;
}

Void PlaYUVerToolsCmdParser::listModuleHelp()
{
  String moduleName = m_strModule;
  PlaYUVerModuleIf* pcCurrModuleIf = NULL;

  PlaYUVerModuleFactoryMap& PlaYUVerModuleFactoryMap = PlaYUVerModuleFactory::Get()->getMap();
  PlaYUVerModuleFactoryMap::iterator it = PlaYUVerModuleFactoryMap.begin();
  for( UInt i = 0; it != PlaYUVerModuleFactoryMap.end(); ++it, i++ ) {
    if( strcmp( it->first, moduleName.c_str() ) == 0 )
    {
      pcCurrModuleIf = it->second();
      break;
    }
  }
  if( pcCurrModuleIf )
  {
    printf( "Usage: playuverTools --module=%s options:\n", it->first );
    pcCurrModuleIf->m_cModuleOptions.doHelp( std::cout );
  }
}
