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
 * \file     PlaYUVerCmdParser.cpp
 * \brief    Command parser
 */

#include "config.h"
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include "PlaYUVerCmdParser.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

PlaYUVerCmdParser::PlaYUVerCmdParser( Int argc, Char *argv[] )
{
  m_iArgc = argc;
  m_apcArgv = argv;
  m_ParserOptions.add( GetCommandOpts() );
}

PlaYUVerCmdParser::~PlaYUVerCmdParser()
{

}

po::options_description PlaYUVerCmdParser::GetCommandOpts()
{
  po::options_description inputOpts( "Input" );
  inputOpts.add_options()/**/
  ( "input,i", po::value<std::vector<std::string> >(), "input file" ) /**/
  ( "output,o", po::value<std::vector<std::string> >(), "output file" ) /**/
  ( "size,s", po::value<std::string>(), "size (WxH)" ) /**/
  ( "pel_fmt", po::value<std::string>(), "pixel format" ) /**/
  ( "pel_fmts", "list pixel formats" ) /**/
  ( "frames,f", po::value<UInt>(), "number of frames" );

  po::options_description operationOpts( "Operation" );
  operationOpts.add_options() /**/
  ( "module", po::value<std::string>(), "select a module" ) /**/
  ( "quality", po::value<std::string>(), "select a quality metric" ) /**/
  ( "quality_metrics", "list supported quality metrics" );

  po::options_description commonOpts( "Common" );
  commonOpts.add_options()/**/
  ( "help", "produce help message" )/**/
  ( "version", "show version and exit" );
  commonOpts.add( inputOpts ).add( operationOpts );
  return commonOpts;
}

Void PlaYUVerCmdParser::addOptions( po::options_description opts )
{
  m_ParserOptions.add( opts );
}

Bool PlaYUVerCmdParser::parse()
{
  try
  {
    po::store( po::command_line_parser( m_iArgc, m_apcArgv ).options( m_ParserOptions ).allow_unregistered().run(), m_cOptionsMap );
    po::notify( m_cOptionsMap );

    if( m_cOptionsMap.count( "help" ) )
    {
      std::cout << m_ParserOptions
                << "\n";
      return false;
    }
    if( m_cOptionsMap.count( "version" ) )
    {
      std::cout << "PlaYUVer version "
                << PLAYUVER_VERSION_STRING
                << "\n";
      return false;
    }
    if( checkListingOpts() )
    {
      return false;
    }
  }
  catch( std::exception& e )
  {
    std::cerr << "error: "
              << e.what()
              << "\n";
    return false;
  }
  catch( ... )
  {
    std::cerr << "Exception of unknown type!\n";
  }
  return true;
}

Bool PlaYUVerCmdParser::checkListingOpts()
{
  Bool bRet = false;
  if( m_cOptionsMap.count( "pel_fmts" ) )
  {
    printf( "PlaYUVer supported pixel formats: \n" );
    for( UInt i = 0; i < PlaYUVerFrame::supportedPixelFormatListNames().size(); i++ )
    {
      printf( "   %s\n", PlaYUVerFrame::supportedPixelFormatListNames()[i].c_str() );
    }
    bRet |= true;
  }

  if( m_cOptionsMap.count( "quality_metrics" ) )
  {
    printf( "PlaYUVer supported quality metrics: \n" );
    for( UInt i = 0; i < PlaYUVerFrame::supportedQualityMetricsList().size(); i++ )
    {
      printf( "   %s\n", PlaYUVerFrame::supportedQualityMetricsList()[i].c_str() );
    }
    bRet |= true;
  }
  return bRet;
}

}  // NAMESPACE
