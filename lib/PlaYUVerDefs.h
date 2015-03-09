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
 * \file     TypeDef.h
 * \ingroup  PlaYUVerLib
 * \brief    Define basic types, new types and enumerations
 */

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include <string>
#include <algorithm>

namespace plaYUVer
{

// ====================================================================================================================
// Basic type redefinition
// ====================================================================================================================

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

// ====================================================================================================================
// 64-bit integer type
// ====================================================================================================================

#ifdef _MSC_VER
typedef __int64 Int64;

#if _MSC_VER <= 1200 // MS VC6
typedef __int64 UInt64;   // MS VC6 does not support unsigned __int64 to double conversion
#else
typedef unsigned __int64 UInt64;
#endif

#else

typedef long long Int64;
typedef unsigned long long UInt64;

#endif

// ====================================================================================================================
// Type definition
// ====================================================================================================================

typedef UChar Pel;        ///< 8-bit pixel type
//typedef       Short           Pel;        ///< 16-bit pixel type

// ====================================================================================================================
// Limits definition
// ====================================================================================================================

#define MAX_UINT                    0xFFFFFFFFU ///< max. value of unsigned 32-bit integer
#define MAX_INT                     2147483647  ///< max. value of signed 32-bit integer
#define MAX_INT64                   0x7FFFFFFFFFFFFFFFLL  ///< max. value of signed 64-bit integer
#define MAX_DOUBLE                  1.7e+308    ///< max. value of double-type value

// ====================================================================================================================
// Macro functions
// ====================================================================================================================

#define DATA_ALIGN                  1                                                                 ///< use 32-bit aligned malloc/free
#if     DATA_ALIGN && _WIN32 && ( _MSC_VER > 1300 )
#define xMalloc( type, len )        _aligned_malloc( sizeof(type)*(len), 32 )
#define xFree( ptr )                _aligned_free  ( ptr )
#define xMemSet( type, len, ptr )
#else
#define xMemSet( type, len, ptr )   memset ( ptr, 0, (len)*sizeof(type) )
#define xMalloc( type, len )        malloc   ( sizeof(type)*(len) )
#define xFree( ptr )                free     ( ptr )
#endif

// ====================================================================================================================
// PlaYUVer definitions
// ====================================================================================================================

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

inline std::string lowercase( const std::string& in )
{
  std::string out;
  transform( in.begin(), in.end(), std::back_inserter( out ), tolower );
  return out;
}

}  // NAMESPACE

#endif // __TYPEDEF_H__
