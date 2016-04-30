/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
#include "PlaYUVerProgramOptions.h"

#ifndef __PLAYUVERCMDPARSER_H__
#define __PLAYUVERCMDPARSER_H__

class PlaYUVerCmdParser
{
public:
  PlaYUVerCmdParser();
  ~PlaYUVerCmdParser();

  Void config( Int argc, Char *argv[] )
  {
    m_iArgc = argc;
    m_ppArgv = argv;
  }
  Bool parse();
  Bool parse( Options& opts, Int argc, Char *argv[] );
  std::list<const Char*>& getNoArgs();
  Options& Opts();
private:
  Int m_iArgc;
  Char** m_ppArgv;
  Options m_cParserOptions;
  std::list<const Char*> m_aUnhandledArgs;

  Bool checkListingOpts();
  Void listModules();
};

#endif // __PLAYUVERCMDPARSER_H__
