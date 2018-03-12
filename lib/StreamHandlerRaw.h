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
 * \file     StreamHandlerRaw.h
 * \ingroup  CalypStreamGrp
 * \brief    Abstract class for stream handling
 */

#ifndef __STREAMHANDLERRAW_H__
#define __STREAMHANDLERRAW_H__

#include "CalypStreamHandlerIf.h"

/**
 * \class StreamHandlerRaw
 * \brief    Class to handle raw video format
 */
class StreamHandlerRaw : public CalypStreamHandlerIf
{
  REGISTER_CALYP_STREAM_HANDLER( StreamHandlerRaw )

private:
  FILE* m_pFile; /**< The input file pointer >*/

public:
  StreamHandlerRaw() { m_pchHandlerName = "RawVideo"; }
  ~StreamHandlerRaw() {}
  bool openHandler( ClpString strFilename, bool bInput );
  void closeHandler();
  bool configureBuffer( CalypFrame* pcFrame );
  void calculateFrameNumber();
  bool seek( unsigned long long int iFrameNum );
  bool read( CalypFrame* pcFrame );
  bool write( CalypFrame* pcFrame );
};

#endif  // __STREAMHANDLERRAW_H__
