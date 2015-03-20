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
#include "ProgramOptions.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

PlaYUVerCmdParser::PlaYUVerCmdParser()
{
  Opts().addOptions()/**/
  ( "help", "produce help message" )/**/
  ( "version", "show version and exit" )/**/
  ( "pel_fmts", "list pixel formats" ) /**/
  ( "quality_metrics", "list supported quality metrics" )/**/
  ( "module_list", "list supported modules" ) /**/
  ( "module_list_full", "detailed list supported modules" );

}

PlaYUVerCmdParser::~PlaYUVerCmdParser()
{

}

Options& PlaYUVerCmdParser::Opts()
{
  return m_cParserOptions;
}

Bool PlaYUVerCmdParser::parse( Int argc, Char *argv[] )
{
  try
  {
    // m_cParserOptions.setDefaults();
    const std::list<const Char*>& argv_unhandled = scanArgv( m_cParserOptions, argc, ( const Char** )argv );

    for( std::list<const Char*>::const_iterator it = argv_unhandled.begin(); it != argv_unhandled.end(); it++ )
    {
      fprintf( stderr, "Unhandled argument ignored: `%s'\n", *it );
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

  if( m_cParserOptions["help"]->count() )
  {
    doHelp( std::cout, m_cParserOptions );
    bRet |= true;
  }

  if( m_cParserOptions["version"]->count() )
  {
    std::cout << "PlaYUVer version "
              << PLAYUVER_VERSION_STRING
              << "\n";
    bRet |= true;
  }
  if( m_cParserOptions["pel_fmts"]->count() )
  {
    printf( "PlaYUVer supported pixel formats: \n" );
    for( UInt i = 0; i < PlaYUVerFrame::supportedPixelFormatListNames().size(); i++ )
    {
      printf( "   %s\n", PlaYUVerFrame::supportedPixelFormatListNames()[i].c_str() );
    }
    bRet |= true;
  }
  return bRet;
}

}  // NAMESPACE
