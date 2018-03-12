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
 * \file     LibMemory.cpp
 * \ingroup  CalypLib
 * \brief    Memory allocation functions
 */

#ifndef __LIBMEMORY_H__
#define __LIBMEMORY_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define DATA_ALIGN 1  ///< use 32-bit aligned malloc/free
#if DATA_ALIGN && _WIN32 && ( _MSC_VER > 1300 )
#define xMalloc( len ) _aligned_malloc( len, 32 )
#define xFreeMem( ptr ) _aligned_free( ptr )
#else
#define xMalloc( len ) malloc( len )
#define xFreeMem( ptr ) free( ptr )
#endif
#define xMemSet( type, len, ptr ) memset( ptr, 0, ( len ) * sizeof( type ) )

static inline void* xMallocMem( size_t nitems )
{
  void* d;
  if( ( d = xMalloc( nitems ) ) == NULL )
  {
    printf( "malloc failed.\n" );
    return NULL;
  }
  return d;
}

static inline void* xCallocMem( size_t nitems, size_t size )
{
  size_t padded_size = nitems * size;
  void* d = xMallocMem( padded_size );
  memset( d, 0, (int)padded_size );
  return d;
}

template <typename T>
int getMem1D( T** array1D, int dim0 )
{
  if( ( *array1D = (T*)xCallocMem( dim0, sizeof( T ) ) ) == NULL )
    return 0;

  return ( sizeof( T* ) + dim0 * sizeof( T ) );
}

template <typename T>
int getMem2D( T*** array2D, int dim0, int dim1 )
{
  int i;

  if( ( *array2D = (T**)xMallocMem( dim0 * sizeof( T* ) ) ) == NULL )
    printf( "get_mem2Dint: array2D" );
  if( ( *( *array2D ) = (T*)xCallocMem( dim0 * dim1, sizeof( T ) ) ) == NULL )
    printf( "get_mem2Dint: array2D" );

  for( i = 1; i < dim0; i++ )
    ( *array2D )[i] = ( *array2D )[i - 1] + dim1;

  return dim0 * ( sizeof( T* ) + dim1 * sizeof( T ) );
}

template <typename T>
int getMem3D( T**** array3D, int dim0, int dim1, int dim2 )
{
  int i, mem_size = dim0 * sizeof( T** );

  if( ( ( *array3D ) = (T***)xMallocMem( dim0 * sizeof( T** ) ) ) == NULL )
    printf( "get_mem3Dint: array3D" );

  mem_size += getMem2D( *array3D, dim0 * dim1, dim2 );

  for( i = 1; i < dim0; i++ )
    ( *array3D )[i] = ( *array3D )[i - 1] + dim1;

  return mem_size;
}

template <typename T>
void freeMem1D( T*& array1D )
{
  if( array1D )
  {
    xFreeMem( array1D );
    array1D = NULL;
  }
}

template <typename T>
void freeMem2D( T**& array2D )
{
  if( *array2D )
  {
    if( *array2D )
      xFreeMem( *array2D );
    else
      printf( "free_mem2Dint: trying to free unused memory" );

    xFreeMem( array2D );
    array2D = NULL;
  }
  else
  {
    printf( "free_mem2Dint: trying to free unused memory" );
  }
}

template <typename T>
void freeMem3D( T***& array3D )
{
  if( array3D )
  {
    freeMem2D( *array3D );
    xFreeMem( array3D );
    array3D = NULL;
  }
  else
  {
    printf( "free_mem3Dint: trying to free unused memory" );
  }
}

#endif  //  __LIBMEMORY_H__
