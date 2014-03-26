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
 * \file     LibMemAlloc.cpp
 * \brief    memory allocation functions
 */

#include "LibMemAlloc.h"

namespace plaYUVer
{

/*!
 ************************************************************************
 * \brief
 *    Allocate 1D memory array -> Pel array1D[dim0
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
int get_mem1Dpel( Pel **array1D, int dim0 )
{
  if( ( *array1D = ( Pel* )mem_calloc( dim0, sizeof(Pel) ) ) == NULL )
    return 0;

  return ( sizeof(Pel*) + dim0 * sizeof(Pel) );
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 2D memory array -> Pel array2D[dim0][dim1]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************/
int get_mem2Dpel( Pel ***array2D, int dim0, int dim1 )
{
  int i;

  if( ( *array2D = ( Pel** )mem_malloc( dim0 * sizeof(Pel*) ) ) == NULL )
    return 0;
  if( ( *( *array2D ) = ( Pel* )mem_malloc( dim0 * dim1 * sizeof(Pel) ) ) == NULL )
    return 0;

  for( i = 1; i < dim0; i++ )
  {
    ( *array2D )[i] = ( *array2D )[i - 1] + dim1;
  }

  return dim0 * ( sizeof(Pel*) + dim1 * sizeof(Pel) );
}

/*!
 ************************************************************************
 * \brief
 *    Allocate 3D memory array -> Pel array3D[dim0][dim1][dim2]
 *
 * \par Output:
 *    memory size in bytes
 ************************************************************************
 */
int get_mem3Dpel( Pel ****array3D, int dim0, int dim1, int dim2 )
{
  int i, mem_size = dim0 * sizeof(Pel**);

  if( ( ( *array3D ) = ( Pel*** )malloc( dim0 * sizeof(Pel**) ) ) == NULL )
    return 0;

  mem_size += get_mem2Dpel( *array3D, dim0 * dim1, dim2 );

  for( i = 1; i < dim0; i++ )
    ( *array3D )[i] = ( *array3D )[i - 1] + dim1;

  return mem_size;
}

/*!
 ************************************************************************
 * \brief
 *    free 1D memory array
 *    which was allocated with get_mem1Dpel()
 ************************************************************************
 */
void free_mem1Dpel( Pel *array1D )
{
  if( array1D )
  {
    mem_free( array1D );
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 2D memory array
 *    which was allocated with get_mem2Dpel()
 ************************************************************************
 */
void free_mem2Dpel( Pel **array2D )
{
  if( array2D )
  {
    if( *array2D )
      mem_free( *array2D );

    mem_free( array2D );
  }
}

/*!
 ************************************************************************
 * \brief
 *    free 3D memory array
 *    which was allocated with get_mem3Dpel()
 ************************************************************************
 */
void free_mem3Dpel( Pel ***array3D )
{
  if( array3D )
  {
    free_mem2Dpel( *array3D );
    mem_free( array3D );
  }
}

}  // NAMESPACE
