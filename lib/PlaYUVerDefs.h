/*    This file is a part of PlaYUVer project
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
 * \file     PlaYUVerDefs.h
 * \ingroup  PlaYUVerLib
 * \brief    Define basic types, new types and enumerations
 */

#ifndef __PLAYUVERDEFS_H__
#define __PLAYUVERDEFS_H__

#include <algorithm>
#include <string>
#include <vector>

typedef void Void;
typedef bool Bool;

typedef char Char;
typedef unsigned char UChar;
typedef short Short;
typedef unsigned short UShort;
typedef int Int;
typedef unsigned int UInt;
typedef double Double;
typedef float Float;

typedef unsigned char UInt8;
typedef unsigned int UInt16;

typedef UShort Pel;  ///< 16-bit pixel type
typedef UChar Byte;

typedef std::string String;

#ifdef _MSC_VER
typedef __int64 Int64;
#if _MSC_VER <= 1200     // MS VC6
typedef __int64 UInt64;  // MS VC6 does not support unsigned __int64 to double conversion
#else
typedef unsigned __int64 UInt64;
#endif
#else
typedef long long Int64;
typedef unsigned long long UInt64;
#endif

enum LOG_LEVEL
{
  LOG_INFO = 1,
  LOG_WARNINGS = 2,
  LOG_RESULT = 3,
  LOG_ERROR = 4,
};

enum YUVcomponent
{
  LUMA = 0,
  CHROMA_U,
  CHROMA_V,
};

enum RGBcomponent
{
  COLOR_R = 0,
  COLOR_G,
  COLOR_B,
  COLOR_A,
};

enum Endianness
{
  PLAYUVER_BIG_ENDIAN = 0,
  PLAYUVER_LITTLE_ENDIAN = 1,
};

inline String lowercase( const String& in )
{
  String out;
  transform( in.begin(), in.end(), std::back_inserter( out ), tolower );
  return out;
}

inline String uppercase( const String& in )
{
  String out;
  transform( in.begin(), in.end(), std::back_inserter( out ), toupper );
  return out;
}

struct PlaYUVerFailure : public std::exception
{
  String m_class_name;
  String m_error_msg;
  PlaYUVerFailure( String error_msg ) throw()
      : m_error_msg( error_msg ) {}
  PlaYUVerFailure( String class_name, String error_msg ) throw()
      : m_class_name( class_name ), m_error_msg( error_msg )
  {
  }
  ~PlaYUVerFailure() throw() {}
  const Char* what() const throw()
  {
    String* msg = new String( "[" + m_class_name + "] " + m_error_msg );
    return msg->c_str();
  }
};

#endif  // __PLAYUVERDEFS_H__
