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
#include "CommandLineHandle.h"
#include "lib/PlaYUVerFrame.h"
#include "modules/PlaYUVerModuleIf.h"
#include "modules/PlaYUVerModuleFactory.h"

namespace plaYUVer
{

CommandLineHandle::CommandLineHandle()
{

}

CommandLineHandle::~CommandLineHandle()
{

}

Int CommandLineHandle::parseToolsArgs()
{
  Int iRet = 0;

  po::options_description qualityOpts( "Quality" );
  qualityOpts.add_options() /**/
  ( "quality", po::value<std::string>(), "select a quality metric" ) /**/
  ( "quality_metrics", "list supported quality metrics" );

  po::options_description moduleOpts( "Module" );
  moduleOpts.add_options() /**/
  ( "module", po::value<std::string>(), "select a module (use internal name)" ) /**/
  ( "module_list", "list supported modules" ) /**/
  ( "module_list_full", "detailed list supported modules" );

  addOptions( qualityOpts );
  addOptions( moduleOpts );

  if( !parse() )
  {
    iRet = 1;
  }

  if( getOptionsMap().count( "quality_metrics" ) )
  {
    printf( "PlaYUVer supported quality metrics: \n" );
    for( UInt i = 0; i < PlaYUVerFrame::supportedQualityMetricsList().size(); i++ )
    {
      printf( "   %s\n", PlaYUVerFrame::supportedQualityMetricsList()[i].c_str() );
    }
    iRet = 1;
  }

  if( getOptionsMap().count( "module_list" ) || getOptionsMap().count( "module_list_full" ) )
  {
    listModules();
    iRet = 1;
  }

  return iRet;
}

Void CommandLineHandle::listModules()
{
  Bool bDetailed = false;

  if( getOptionsMap().count( "module_list_full" ) )
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

}  // NAMESPACE
