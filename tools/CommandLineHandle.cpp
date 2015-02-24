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

#include "CommandLineHandle.h"
#include "lib/PlaYUVerFrame.h"

namespace plaYUVer
{

CommandLineHandle::CommandLineHandle()
{

}

CommandLineHandle::~CommandLineHandle()
{

}

Int CommandLineHandle::ParseToolsArgs()
{
  Int iRet = 0;

  po::options_description qualityOpts( "Quality" );
  qualityOpts.add_options() /**/
  ( "quality", po::value<std::string>(), "select a quality metric" ) /**/
  ( "quality_metrics", "list supported quality metrics" );

  po::options_description moduleOpts( "Module" );
  moduleOpts.add_options() /**/
  ( "module", po::value<std::string>(), "select a module" ) /**/
  ( "module_list", "list supported modules" );

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

  return iRet;
}

}  // NAMESPACE
