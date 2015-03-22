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
 * \file     CommandLineHandle.h
 * \brief    Handle for command line
 */

#ifndef __COMMANDLINEHANDLE_H__
#define __COMMANDLINEHANDLE_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#include "lib/PlaYUVerCmdParser.h"
#include "lib/ProgramOptions.h"

namespace plaYUVer
{

class PlaYUVerStream;

class CommandLineHandle: public PlaYUVerCmdParser
{
  friend class PlaYUVerTools;
public:
  CommandLineHandle();
  ~CommandLineHandle();

  enum LOG_LEVEL
  {
    INFO = 0,
    WARNINGS = 1,
    RESULT = 2,
    ERROR = 3,
  };
  Void setLogLevel( enum LOG_LEVEL level )
  {
    m_uiLogLevel = level;
  }

  /**
   * Send the specified message to the log if the level is higher than or equal
   * to the current LogLevel. By default, all logging messages are sent to
   * stdout.
   *
   * @param level The importance level of the message expressed using a @ref
   *        lavu_log_constants "Logging Constant".
   * @param fmt The format string (printf-compatible) that specifies how
   *        subsequent arguments are converted to output.
   *
   * @note this function might not be safe in C++ (try to upgrade it)
   */
  Void log( UInt level, const char *fmt, ... );

  Int parseToolsArgs( Int argc, Char *argv[] );

private:
  Options m_cParserOptions;
  UInt m_uiLogLevel;

  /**
   * Command line opts for PlaYUVerTools
   */
  Bool m_bShowHelp;
  Bool m_bShowVersion;
  Bool m_bQuiet;
  std::vector<std::string> m_apcInputs;
  std::string m_strInput;
  std::string m_strOutput;
  std::string m_strResolution;
  std::string m_strPelFmt;
  Bool m_bListPelFmts;
  Int m_iFrames;

  Bool m_bListQuality;
  std::string m_strQualityMetric;
  Bool m_bListModules;
  std::string m_strModule;

  Void listModules();
  Void listModuleHelp();

};

}  // NAMESPACE

#endif // __COMMANDLINEHANDLE_H__
