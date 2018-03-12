/*    This file is a part of Calyp project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     CalypToolsCmdParser.h
 * \brief    Handle for command line
 */

#ifndef __CALYPTOOLSCMDPARSER_H__
#define __CALYPTOOLSCMDPARSER_H__

#include "config.h"
#include "lib/CalypOptions.h"

class CalypToolsCmdParser
{
public:
  CalypToolsCmdParser();
  ~CalypToolsCmdParser();

  void setLogLevel( enum CLP_LOG_LEVEL level ) { m_uiLogLevel = level; }
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
  void log( unsigned int level, const char* fmt, ... );

  int parseToolsArgs( int argc, char* argv[] );

  CalypOptions& Opts() { return m_cOptions; }

protected:
  CalypOptions m_cOptions;
  unsigned int m_uiLogLevel;

  /**
   * Command line opts for CalypTools
   */
  bool m_bShowHelp;
  bool m_bShowVersion;
  bool m_bQuiet;

  std::vector<ClpString> m_apcInputs;
  std::vector<ClpString> m_strResolution;
  std::vector<ClpString> m_strPelFmt;
  std::vector<unsigned int> m_uiBitsPerPixel;
  std::vector<ClpString> m_strEndianness;
  ClpString m_strOutput;
  long m_iFrames;

  int m_iRateReductionFactor;
  ClpString m_strQualityMetric;
  ClpString m_strModule;

  bool m_bListPelFmts;
  bool m_bListQuality;
  bool m_bListModules;

  void listModules();
  void listModuleHelp();
};

#endif  // __CALYPTOOLSCMDPARSER_H__
