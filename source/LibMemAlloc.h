/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
 * \file     LibMemAlloc.h
 * \brief    memory allocation functions
 */

#ifndef __LIBMEMALLOC_H__
#define __LIBMEMALLOC_H__

#include "config.h"

#include <cstdlib>
#include <cstring>
#include "TypeDef.h"

namespace plaYUVer
{

extern int get_mem1Dpel( Pel **array2D, int dim0 );
extern int get_mem2Dpel( Pel ***array2D, int dim0, int dim1 );
extern int get_mem3Dpel( Pel ****array3D, int dim0, int dim1, int dim2 );

extern void free_mem1Dpel( Pel *array1D );
extern void free_mem2Dpel( Pel **array2D );
extern void free_mem3Dpel( Pel ***array3D );

/*!
 ************************************************************************
 * \brief
 *    allocate and set memory aligned at SSE_MEMORY_ALIGNMENT
 *
 ************************************************************************/
static inline void* mem_malloc( size_t nitems )
{
  void *d;
  if( ( d = malloc( nitems ) ) == NULL )
  {
    return NULL;
  }
  return d;
}
static inline void* mem_calloc( size_t nitems, size_t size )
{
  size_t padded_size = nitems * size;
  void *d = mem_malloc( padded_size );
  memset( d, 0, ( int )padded_size );
  return d;
}

static inline void mem_free( void *ptr )
{
  if( ptr != NULL )
  {
    free( ptr );
    ptr = NULL;
  }
}

}  // NAMESPACE

#endif // __LIBMEMALLOC_H__