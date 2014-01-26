/***************************************************************************
 *   This file is a part of SCode project                                  *
 *   IT - Instituto de Telecomunicacoes                                    *
 *   http://www.it.pt                                                      *
 *                                                                         *
 *   Copyright (C) 2008 by Ricardo N. Rocha Sardo                          *
 *   ricardo.sardo@co.it.pt                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SCODEGRID_H
#define SCODEGRID_H

#include <QRect>
#include <QColor>
#include <QPen>

class QPoint;
class QBitmap;
class QPainter;

namespace SCode
{

/**
 *
 */
class GridManager
{
public:

  enum Style
  {
    IntersectionDot = 0, IntersectionCross, Dashed, Solid
  };

  GridManager();

  void drawGrid( QPixmap &image, const QRect& area, QPainter *painter );

  /**
   * @returns a QRect defined by the grid that contains the point (x, y). 
   */
  QRect rectContains( int x, int y ) const;

  /**
   * This is an overloaded member function, provided for convenience.
   * @returns a QRect defined by the grid that contains the QPoint pos. 
   */
  QRect rectContains( const QPoint &pos ) const;

  /**
   * @returns @c true if the point (x, y) is over a grid crossing, otherwise 
   * returns @c false
   */
  bool contains( int x, int y );
  /**
   * @returns @c true if the point (x, y) is near a grid crossing, otherwise 
   * returns @c false.
   * @see @p nearPos()
   */
  bool isNear( int x, int y );

  /**
   * This is an overloaded member function, provided for convenience.
   * @returns @c true if the given point is near a grid crossing, otherwise 
   * returns @c false.
   * @see @p nearPos()
   */
  bool isNear( const QPoint &pos );

  /**
   * @returns the grid crossing position that is near the point (x,y). If
   * either the x or y are iqual to -1 (default) than returns the last near
   * grid position found by @p isNear().
   */
  QPoint nearPos( int x = -1, int y = -1 );

private:

  QPoint m_nearPos;
};

} // NameSpace SCode

#endif // SCODEGRID_H
