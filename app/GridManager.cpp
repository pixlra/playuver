/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     GridManager.cpp
 * \brief    Function to create a grid in the images
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#include <QBitmap>
#include <QPainter>
#include <QRect>
#include <QRectF>
#include <QPoint>
#include <QColor>
#include <QPen>
#include <QDebug>

#include "GridManager.h"
#include "PlaYUVerSettings.h"

namespace plaYUVer
{

////////////////////////////////////////////////////////////////////////////////
//                              Constructor
////////////////////////////////////////////////////////////////////////////////
GridManager::GridManager()
{
  m_nearPos = QPoint();
}

////////////////////////////////////////////////////////////////////////////////
//                              Grid Drawer
////////////////////////////////////////////////////////////////////////////////
void GridManager::drawGrid( QPixmap &image, const QRect& area, QPainter *painter )
{
  PlaYUVerSettings settings;

  int offsetx = 0;                // Grid offset doesn't applies to SCODE
  int offsety = 0;                // project for now...

  int imageWidth = image.width();
  int imageHeight = image.height();
  int hSpacing = settings.gridHSpacing();
  int vSpacing = settings.gridVSpacing();
  QColor color = settings.gridColor();
  Style style = settings.gridStyle();

  // Set pen for solid line.
  QPen mainPen = QPen( color, 1, Qt::SolidLine );

  switch( style )
  {
  case IntersectionDot:
  case IntersectionCross:
  {
    painter->setPen( mainPen );
    // Draw crosses or dots on intersections 
    for( int x = offsetx; x <= area.right(); x += hSpacing )
    {
      if( x >= imageWidth )
        break;

      for( int y = offsety; y <= area.bottom(); y += vSpacing )
      {
        if( y >= imageHeight )
          break;

        if( x >= area.x() && y >= area.y() )
        {
          if( style == IntersectionDot )
            painter->drawPoint( x, y );             // Dot
          else
          {                                           // Crosses:
            painter->drawLine( x, y - 1, x, y + 1 );    // Ver.line
            painter->drawLine( x - 1, y, x + 1, y );    // Hor.line
          }
        }
      }
    }
    break;
  }
  case Dashed:
  {
    // Change pen to the dashed line 
    mainPen.setStyle( Qt::DashLine );
  }
  default: // Solid:
  {
    painter->setPen( mainPen );

    // Draw vertical line        
    for( int x = offsetx; x <= area.right(); x += hSpacing )
    {
      if( x >= imageWidth )
        break;

      if( x >= area.x() )
      {
        // Always draw the full line otherwise the line stippling 
        // varies with the location of view area and we get glitchy 
        // patterns.
        painter->drawLine( x, 0, x, imageHeight - 1 );
      }
    }
    // Draw horizontal line
    for( int y = offsety; y <= area.bottom(); y += vSpacing )
    {
      if( y >= imageHeight )
        break;

      if( y >= area.y() )
      {
        painter->drawLine( 0, y, imageWidth - 1, y );
      }
    }
    break;
  }
  }
}

////////////////////////////////////////////////////////////////////////////////
QRect GridManager::rectContains( const QPoint &pos ) const
{
  return rectContains( pos.x(), pos.y() );
}

QRect GridManager::rectContains( int x, int y ) const
{
  QRect rect;
  QPoint pos;
  int multiPosX;
  int multiPosY;

  PlaYUVerSettings settings;
  int hSpacing = settings.gridHSpacing();
  int vSpacing = settings.gridVSpacing();

  if( ( x < 0 ) || ( y < 0 ) )
  {
    return rect;
  }

  // Find the multiple factor for the fisrt (x,y) position of the grid that
  // is less than the given (x,y)
  multiPosX = x / hSpacing;
  multiPosY = y / vSpacing;

  // Grid intersection position (x,y) that is less than the given (x,y)
  pos = QPoint( multiPosX * hSpacing, multiPosY * vSpacing );

  rect = QRect( pos.x(), pos.y(), hSpacing, vSpacing );

  return rect;
}

bool GridManager::contains( int x, int y )
{
  int resX;
  int resY;

  PlaYUVerSettings settings;
  int hSpacing = settings.gridHSpacing();
  int vSpacing = settings.gridVSpacing();

  if( ( x < hSpacing ) || ( y < vSpacing ) || ( x < 0 ) || ( y < 0 ) )
    return false;

  if( ( x == 0 ) && ( y == 0 ) )
    return true;

  resX = 0;
  if( hSpacing )
    resX = x % hSpacing;

  resY = 0;
  if( vSpacing )
    resY = y % vSpacing;

  if( resX || resY )
    return false;
  else
    return true;
}

QPoint GridManager::nearPos( int x, int y )
{
  if( ( x != -1 ) && ( y != -1 ) )
    isNear( x, y );

  return m_nearPos;
}

bool GridManager::isNear( int x, int y )
{
  qreal rectWidth;
  qreal rectHeight;
  int multiPosX;
  int multiPosY;

  PlaYUVerSettings settings;
  int hSpacing = settings.gridHSpacing();
  int vSpacing = settings.gridVSpacing();

  if( ( x < 0 ) || ( y < 0 ) )
  {
    m_nearPos = QPoint( -1, -1 );
    return false;
  }

  // We go to consider an area with ray equal 25% of the distance between 
  // spaces of the gride. In this in case, we have one rectangle centered 
  // in 25% of this distance.

  rectWidth = 2 * 0.25 * hSpacing;
  rectHeight = 2 * 0.25 * vSpacing;

  QRectF rect( 0, 0, rectWidth, rectHeight );

  // Find the multiple factor for the fisrt (x,y) position of the grid that
  // is less than the given (x,y)
  multiPosX = x / hSpacing;
  multiPosY = y / vSpacing;

  QPointF gridPoint( 0, 0 );
  int gx;
  int gy;

  // In the worse case we have 4 points of the gride surrounding
  for( int j = 0; j < 2; j++ )
  {
    for( int i = 0; i < 2; i++ )
    {
      gx = ( int )( multiPosX * hSpacing + i * hSpacing );
      gy = ( int )( multiPosY * vSpacing + j * vSpacing );
      gridPoint.setX( gx );
      gridPoint.setY( gy );
      rect.moveCenter( gridPoint );

      if( rect.contains( x, y ) )
      {
        m_nearPos = QPoint( gx, gy );
        return true;
      }
    }
  }
  m_nearPos = QPoint( -1, -1 );
  return false;
}

bool GridManager::isNear( const QPoint &pos )
{
  return isNear( pos.x(), pos.y() );
}

} // NameSpace plaYUVer
