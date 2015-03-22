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
 * \file     CommandLineHandle.cpp
 * \brief    Handle for command line
 */

#include <cstring>
#include <cstdarg>
#include "CommandLineHandle.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerStream.h"
#include "modules/PlaYUVerModuleIf.h"
#include "modules/PlaYUVerModuleFactory.h"

namespace plaYUVer
{

CommandLineHandle::CommandLineHandle()
{
  m_uiLogLevel = 0;
  m_bQuiet = false;
  m_strPelFmt = std::string( "YUV420p" );
  m_iFrames = -1;
}

CommandLineHandle::~CommandLineHandle()
{

}

Void CommandLineHandle::log( UInt level, const char *fmt, ... )
{
  if( level >= m_uiLogLevel )
  {
    std::va_list args;
    va_start( args, fmt );
    vfprintf( stdout, fmt, args );
    va_end( args );
  }
}

Int CommandLineHandle::parseToolsArgs( Int argc, Char *argv[] )
{
  Int iRet = 0;

  Opts().addOptions()/**/
  ( "quiet,q", m_bQuiet, "disable verbose" );

  Opts().addOptions()/**/
  ( "input,i", m_apcInputs, "input file" ) /**/
  //( "input,i", m_strInput, "input file" ) /**/
  ( "output,o", m_strOutput, "output file" ) /**/
  ( "size,s", m_strResolution, "size (WxH)" ) /**/
  ( "pel_fmt", m_strPelFmt, "pixel format" ) /**/
  ( "frames,f", m_iFrames, "number of frames to parse" );

  Opts().addOptions()/**/
  ( "quality", m_strQualityMetric, "select a quality metric" ) /**/
  ( "module", m_strModule, "select a module (use internal name)" );

  config( argc, argv );
  if( !parse() )
  {
    iRet = 1;
  }

  if( m_bQuiet )
  {
    m_uiLogLevel = RESULT;
  }

  if( Opts()["module"]->count() && Opts()["help"]->count() )
  {
    listModuleHelp();
    iRet = 1;
  }
  else if( Opts()["help"]->count() )
  {
    printf( "Usage: %s modules/quality [options] -input=input_file [--output=output_file]\n", argv[0] );
    doHelp( std::cout, Opts() );
    iRet = 1;
  }

  if( Opts()["module_list"]->count() || Opts()["module_list_full"]->count() )
  {
    listModules();
    iRet = 1;
  }
  return iRet;
}

Void CommandLineHandle::listModules()
{
  Bool bDetailed = false;

  if( Opts()["module_list_full"]->count() )
    bDetailed = true;

  PlaYUVerModuleIf* pcCurrModuleIf;
  PlaYUVerModuleFactoryMap& PlaYUVerModuleFactoryMap = PlaYUVerModuleFactory::Get()->getMap();
  PlaYUVerModuleFactoryMap::iterator it = PlaYUVerModuleFactoryMap.begin();

  printf( "PlaYUVer available modules: \n" );
  //printf( "                                           " );
  printf( "   [Internal Name]               " );
  if( bDetailed )
  {
    printf( "   [Full Name]                             " );
    printf( "   [Type]        " );
    printf( "   [Description]" );
  }
  printf( " \n" );

  Char ModuleNameString[40];

  for( UInt i = 0; it != PlaYUVerModuleFactoryMap.end(); ++it, i++ )
  {
    printf( "   " );
    printf( "%-30s", it->first );
    if( bDetailed )
    {
      ModuleNameString[0] = '\0';
      pcCurrModuleIf = it->second();

      if( pcCurrModuleIf->m_pchModuleCategory )
      {
        strcat( ModuleNameString, pcCurrModuleIf->m_pchModuleCategory );
        strcat( ModuleNameString, "/" );
      }
      strcat( ModuleNameString, pcCurrModuleIf->m_pchModuleName );
      printf( "   %-40s", ModuleNameString );
      switch( pcCurrModuleIf->m_iModuleType )
      {
      case FRAME_PROCESSING_MODULE:
        printf( "   Processing    " );
        break;
      case FRAME_MEASUREMENT_MODULE:
        printf( "   Measurement   " );
        break;
      }
      printf( "   %s", pcCurrModuleIf->m_pchModuleTooltip );
      pcCurrModuleIf->Delete();
    }
    printf( "\n" );
  }
}

Void CommandLineHandle::listModuleHelp()
{
  std::string moduleName = m_strModule;
  PlaYUVerModuleIf* pcCurrModuleIf = NULL;

  PlaYUVerModuleFactoryMap& PlaYUVerModuleFactoryMap = PlaYUVerModuleFactory::Get()->getMap();
  PlaYUVerModuleFactoryMap::iterator it = PlaYUVerModuleFactoryMap.begin();
  for( UInt i = 0; it != PlaYUVerModuleFactoryMap.end(); ++it, i++ )
  {
    if( strcmp( it->first, moduleName.c_str() ) == 0 )
    {
      pcCurrModuleIf = it->second();
      break;
    }
  }
  if( pcCurrModuleIf )
  {
    printf( "Usage: playuverTools --module=%s options:\n", it->first );
    doHelp( std::cout, pcCurrModuleIf->m_cModuleOptions );
  }
}

}  // NAMESPACE
