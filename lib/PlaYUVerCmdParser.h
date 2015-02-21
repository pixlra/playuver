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
 * \file     PlaYUVerCmdParser.h
 * \brief    Command parser
 */

#include "PlaYUVerDefs.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

namespace plaYUVer
{

class PlaYUVerCmdParser
{
public:
  PlaYUVerCmdParser( Int argc, Char *argv[] );
  ~PlaYUVerCmdParser();

  Void addOptions( po::options_description );
  Bool parse();

  po::variables_map getOptionsMap()
  {
    return m_cOptionsMap;
  }

  static po::options_description GetCommandOpts();

private:
  Int m_iArgc;
  Char** m_apcArgv;
  po::options_description m_ParserOptions;
  po::variables_map m_cOptionsMap;
};

}  // NAMESPACE
