/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     main.cpp
 * \brief    main file
 */

#include <lib/PlaYUVerCmdParser.h>
#include <lib/PlaYUVerFrame.h>
#include <lib/PlaYUVerStream.h>

using namespace plaYUVer;
using namespace std;

int main( int argc, char *argv[] )
{

  try
  {

    po::options_description toolsOpt;
    toolsOpt.add( PlaYUVerCmdParser::GetCommandOpts() );
    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, toolsOpt ), vm );
    po::notify( vm );

    if( vm.count( "help" ) )
    {
      cout << toolsOpt << "\n";
      return 0;
    }

    if( vm.count( "compression" ) )
    {
      cout << "Compression level was set to " << vm["compression"].as<double>() << ".\n";
    }
    else
    {
      cout << "Compression level was not set.\n";
    }
  }
  catch( exception& e )
  {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch( ... )
  {
    cerr << "Exception of unknown type!\n";
  }

  PlaYUVerFrame pcFrameBuffer = new PlaYUVerFrame( 1024, 768, PlaYUVerFrame::YUV420p );
  return 0;
}
