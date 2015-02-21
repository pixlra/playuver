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
#include <string>
#include <vector>
#include "PlaYUVerCmdParser.h"

namespace plaYUVer
{

po::options_description PlaYUVerCmdParser::GetCommandOpts()
{
  po::options_description inputOpts( "Input" );
  inputOpts.add_options()
      ( "input,i", po::value<std::string>(), "input file" )
      ( "width,w", po::value<Int>(), "width" )
      ( "height,h", po::value<Int>(), "height" );

  po::options_description operationOpts( "Operation" );
  operationOpts.add_options()
      ( "module", po::value<std::string>(), "select a module" )
      ( "quality", po::value<std::string>(),"select a quality metric" );

  po::options_description commonOpts( "Common" );
  commonOpts.add_options()
      ( "help", "produce help message" );
  commonOpts.add( inputOpts ).add( operationOpts );
  return commonOpts;
}

}  // NAMESPACE
