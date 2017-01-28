/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     StreamHandlerRaw.h
 * \brief    Abstract class for stream handling
 */

#ifndef __STREAMHANDLERRAW_H__
#define __STREAMHANDLERRAW_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerStreamHandlerIf.h"

/**
 * \class StreamHandlerRaw
 * \brief    Class to handle raw video format
 */
class StreamHandlerRaw : public PlaYUVerStreamHandlerIf
{
  REGISTER_STREAM_HANDLER( StreamHandlerRaw )

 private:
  FILE* m_pFile; /**< The input file pointer >*/

 public:
  StreamHandlerRaw() {}
  ~StreamHandlerRaw() {}
  Bool openHandler( String strFilename, Bool bInput );
  Void closeHandler();
  Bool configureBuffer( PlaYUVerFrame* pcFrame );
  UInt64 calculateFrameNumber();
  Bool seek( UInt64 iFrameNum );
  Bool read( PlaYUVerFrame* pcFrame );
  Bool write( PlaYUVerFrame* pcFrame );
};

#endif  // __STREAMHANDLERRAW_H__
