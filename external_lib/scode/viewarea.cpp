/***************************************************************************
 *   This file is a part of SCode project               		   *
 *   IT - Instituto de Telecomunicacoes   		                   *
 *   http://www.it.pt				                           *
 *									   *
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
#include <cmath>

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QImage>
#include <QColor>
#include <QString>
#include <QPixmapCache>
#include <QRectF>
#include <QDebug>

#include "viewarea.h"
#include "gridmanager.h"

namespace SCode
{

static const QColor selectionColor = Qt::cyan;
static const QColor imageMaskColor = Qt::green;
static const QColor eraserColor = Qt::red;

ViewArea::ViewArea( QWidget *parent ) :
    QWidget( parent )
{
  setAttribute( Qt::WA_StaticContents );
//     setAttribute( Qt::WA_NoBackground );
  setMouseTracking( true );

  m_pixmap = QPixmap();
  m_grid = GridManager();
  m_mask = QBitmap();
  m_selectedArea = QRect();
  m_zoomFactor = 1;
  m_xOffset = 0;
  m_yOffset = 0;
  m_mode = NormalMode;
  m_tool = NormalSelectionTool;
  m_gridVisible = false;
  m_snapToGrid = false;
  m_blockTrackEnable = false;
}

void ViewArea::setImage( const QPixmap &pixmap )
{
  m_pixmap = pixmap;
  m_mask = QBitmap( pixmap.width(), pixmap.height() );
  m_mask.clear();
  updateSize();
  update();
  updateGeometry();
}

void ViewArea::clear()
{
  m_pixmap = QPixmap();
  m_mask = QBitmap();
  m_mode = NormalMode;
  updateSize();
  update();
  updateGeometry();
}

void ViewArea::clearMask()
{
  m_mask.clear();
}

// QSize ViewArea::sizeHint() const
// {
//     int w = m_pixmap.width()*m_zoomFactor;
//     int h = m_pixmap.height()*m_zoomFactor;
//     return QSize( w, h );
// }

void ViewArea::setZoomFactor( double f )
{
  if( f == m_zoomFactor )
    return;

  m_zoomFactor = f;
  emit( zoomFactorChanged( m_zoomFactor ) );

  updateSize();
  repaint();
  updateGeometry();
}

void ViewArea::setMode( ViewMode mode )
{
  if( m_mode == mode )
    return;

  m_mode = mode;
  update();
}

void ViewArea::setNormalMode()
{
  setMode( NormalMode );
}

void ViewArea::setMaskMode()
{
  setMode( MaskMode );
}

void ViewArea::setMaskColor( const QColor &color )
{
  m_maskColor = color;
  update();
}

void ViewArea::setTool( Tool tool )
{
  m_tool = tool;
}

void ViewArea::setMaskTool()
{
  setMode( MaskMode );
  setTool( MaskTool );
  m_blockTrackEnable = true;
}

void ViewArea::setEraserTool()
{
  setMode( MaskMode );
  setTool( EraserTool );
  m_blockTrackEnable = true;
}

void ViewArea::setNormalSelectionTool()
{
  setMode( NormalMode );
  setTool( NormalSelectionTool );
  m_blockTrackEnable = false;
}

void ViewArea::setBlockSelectionTool()
{
  setMode( NormalMode );
  setTool( BlockSelectionTool );
  m_blockTrackEnable = true;
}

void ViewArea::setGridVisible( bool enable )
{
  m_gridVisible = enable;
  update();
}

void ViewArea::setSnapToGrid( bool enable )
{
  m_snapToGrid = enable;
}

void ViewArea::setSelectedArea( QRect &rect )
{
  if( rect.isNull() )
  {
    update();
    return;
  }

  QRect updateRect;
  setNormalMode();

  if( m_selectedArea.isNull() )
  {
    m_selectedArea = rect.normalized();
    updateRect = m_selectedArea;
  }
  else
  {
    updateRect = m_selectedArea;
    m_selectedArea = rect.normalized();
    updateRect = updateRect.united( m_selectedArea );
  }
  updateRect.adjust( 0, 0, 1, 1 );
  update( updateRect.normalized() );
}
////////////////////////////////////////////////////////////////////////////////
//                            Geometry Updates 
////////////////////////////////////////////////////////////////////////////////
void ViewArea::updateSize()
{
  int w = m_pixmap.width() * m_zoomFactor;
  int h = m_pixmap.height() * m_zoomFactor;
  setMinimumSize( w, h );

  QWidget *p = parentWidget();
  if( p )
  {
    // If the parent size is bigger than the minimum area to view the 
    // image, resize() will call resizeEvent(); otherwise, we need to 
    // perform the necessary updates (updateOffset). 
    resize( p->width(), p->height() );
  }

  if( w <= width() && h <= height() )
    updateOffset();
}

void ViewArea::updateOffset()
{
  if( width() > m_pixmap.width() * m_zoomFactor )
  {
    m_xOffset = ( width() - m_pixmap.width() * m_zoomFactor ) / 2;
  }
  else
  {
    m_xOffset = 0;
  }

  if( height() > m_pixmap.height() * m_zoomFactor )
  {
    m_yOffset = ( height() - m_pixmap.height() * m_zoomFactor ) / 2;
  }
  else
  {
    m_yOffset = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
//                              Resize Event  
////////////////////////////////////////////////////////////////////////////////
void ViewArea::resizeEvent( QResizeEvent */* event */)
{
  if( size().isEmpty() || m_pixmap.isNull() )
    return;

  updateOffset();
}
////////////////////////////////////////////////////////////////////////////////
//                              Paint Event  
////////////////////////////////////////////////////////////////////////////////

void ViewArea::paintEvent( QPaintEvent *event )
{
  QRect winRect = event->rect();

  if( size().isEmpty() || m_pixmap.isNull() )
    return;

  QPainter painter( this );

  // Save the actual painter properties and scales the coordinate system. 
  painter.save();
  painter.translate( m_xOffset, m_yOffset );
  painter.scale( m_zoomFactor, m_zoomFactor );

  // Draw the pixmap.
  painter.drawPixmap( QPoint( 0, 0 ), m_pixmap );

  // Draw the Grid if it's visible.
  if( m_gridVisible )
  {
    // Do we need to draw the whole grid?
    // To know that, we need to perform a transformation of the rectangle 
    // area that the painter needs to update - transform the windows 
    // coordinates (origin at the top-left corner of the widget), to the 
    // relatives coordinates of the image at it's original size (origin at 
    // the top-left corner of the image).        
    QRect vr = windowToView( winRect );

    // Now we have the (to update) rectangle area on a coordinates system 
    // that has it's origin at the top-left corner of the image. That 
    // is, is referenced to the not scaled image.
    // To know what image area we need to update, just intersects the 
    // rectangle area with the image area. 
    vr &= QRect( 0, 0, m_pixmap.width(), m_pixmap.height() );

    // Set up for the grid drawer.
    painter.setRenderHint( QPainter::Antialiasing );

    // Draw grid.
    m_grid.drawGrid( m_pixmap, vr, &painter );
  }

  painter.restore();

  // Draw a border around the image.
  if( m_xOffset || m_yOffset )
  {
    painter.setPen( Qt::black );
    painter.drawRect( m_xOffset - 1, m_yOffset - 1, m_pixmap.width() * m_zoomFactor + 1, m_pixmap.height() * m_zoomFactor + 1 );
  }

  QRect sr = viewToWindow( m_selectedArea );
  QRect ir = sr & winRect;

  if( ( mode() == NormalMode ) && !selectedArea().isNull() )
  {
    if( m_newShape || m_blockTrackEnable )
    {
      // Set the tool color
      QColor selectColor = selectionColor;

      selectColor.setAlpha( 120 );

      QBrush brush( selectColor );

      if( m_blockTrackEnable )
        painter.setBrush( Qt::NoBrush );
      else
        painter.setBrush( brush );

      painter.setPen( Qt::darkCyan );

//             painter.drawRect( sr/*m_selectedArea*/ );
    }
    else
    {
      // 1) Cover the image with a light gray except the selected area
      QColor fill( Qt::lightGray );
      fill.setAlpha( 150 );

      QBrush brush( fill );
      painter.setBrush( fill );
      painter.setPen( Qt::NoPen );
      QPainterPath myPath;
      QRect imgr = viewToWindow( QRect( 0, 0, m_pixmap.width(), m_pixmap.height() ) );

      myPath.addRect( imgr );
      myPath.addRect( sr/*m_selectedArea*/);
      painter.drawPath( myPath );

      // 2) Draw the selection rectangle   
      painter.setBrush( Qt::NoBrush );
      painter.setPen( Qt::darkCyan );
//             painter.drawRect( sr/*m_selectedArea*/ ); 
    }

    if( !ir.isNull() )
      painter.drawRect( sr/*m_selectedArea*/);

  }

  // Mask Mode ----------------------------------------------------------

  if( mode() != MaskMode )
    return;

  QColor color;
  QBrush brush;

  // Draw Mask
  // Set the tool color
  if( m_maskColor.isValid() )
    color = m_maskColor;
  else
    color = imageMaskColor;

  color.setAlpha( 120 );
  painter.setPen( color );
  painter.save();
  painter.translate( m_xOffset, m_yOffset );
  painter.scale( m_zoomFactor, m_zoomFactor );
  painter.drawPixmap( QPoint( 0, 0 ), m_mask );
  painter.restore();

  if( !m_selectedArea.isNull() )
  {
    if( m_newShape || m_blockTrackEnable )
    {
      switch( tool() )
      {
      case MaskTool:
      {
        if( m_maskColor.isValid() )
          color = m_maskColor;
        else
          color = imageMaskColor;

        break;
      }
      case EraserTool:
      {
        color = eraserColor;
        break;
      }
      default:
        color = selectionColor; // ?Problems!
      }

      color.setAlpha( 120 );
      brush = QBrush( color );

      if( m_blockTrackEnable )
        painter.setBrush( Qt::NoBrush );
      else
        painter.setBrush( brush );

      painter.setPen( color );
      if( !ir.isNull() )
        painter.drawRect( sr );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                              Mouse Events  
////////////////////////////////////////////////////////////////////////////////
void ViewArea::wheelEvent( QWheelEvent *event )
{
  double f;

  f = m_zoomFactor + 0.001 * event->delta();
  if( f < ( 32.0 / m_pixmap.width() ) )
    f = 32.0 / m_pixmap.width();

  setZoomFactor( f );

}

void ViewArea::mousePressEvent( QMouseEvent *event )
{
  event->accept();

  QPoint vpos = windowToView( event->pos() );

  if( event->button() == Qt::LeftButton )
  {
    // Is the mouse over the image? If yes, save the mouse position; 
    // otherwise, QPoint( -1, -1 ) indicates an invalid position.
    if( isPosValid( vpos ) )
    {
      m_lastPos = vpos;
    }
    else
    {
      m_lastPos = QPoint( -1, -1 );
      return;
    }

    // If grid tracking
    if( m_snapToGrid )
    {
      // Find if the cursor is near a grid intersection
      bool isNear = m_grid.isNear( m_lastPos );

      if( isNear )
      {
        // The grid 'near intersection' found when used isNear()
        m_lastPos = m_grid.nearPos();
      }
    }

    m_newShape = true;

    if( tool() == NormalSelectionTool )
    {
      m_blockTrackEnable = false;
      return;
    }

    if( mode() == MaskMode )
    {
      // Disable the block track to enable fill the block
      m_blockTrackEnable = false;
    }

    // Block selection 
    m_selectedArea = m_grid.rectContains( m_lastPos );

    QRect updateRect = viewToWindow( m_selectedArea );
    updateRect.adjust( 0, 0, 1, 1 );
    update( updateRect.normalized() );
  }
}

void ViewArea::mouseMoveEvent( QMouseEvent *event )
{
  event->accept();

  QPoint actualPos = windowToView( event->pos() );
  QRect updateRect;

  emit positionChanged( actualPos );

  // If mouse left button pressed
  if( event->buttons() == Qt::LeftButton && m_lastPos != QPoint( -1, -1 ) )
  {
    // Grid tracking
    if( m_snapToGrid )
    {
      // Find if the cursor is near a grid intersection
      bool isNear = m_grid.isNear( actualPos );

      if( isNear )
      {
        // Return the last grid near intersection found
        actualPos = m_grid.nearPos();
      }
    }

    updateRect = viewToWindow( m_selectedArea );

    if( tool() == NormalSelectionTool )
    {
      // If the selection is only vertical or horizontal then we have one
      // of the dimentions null.
      if( actualPos.x() == m_lastPos.x() || actualPos.y() == m_lastPos.y() )
      {
        m_selectedArea = QRect();
      }
      // Selection from top to bottom
      else if( m_lastPos.y() < actualPos.y() )
      {
        // From left to right
        if( m_lastPos.x() < actualPos.x() )
        {
          QPoint bottomR = actualPos - QPoint( 1, 1 );
          m_selectedArea = QRect( m_lastPos, bottomR );
        }
        // From right to left
        else
        {
          QPoint topL( actualPos.x(), m_lastPos.y() );
          QPoint bottomR( m_lastPos.x() - 1, actualPos.y() - 1 );

          m_selectedArea = QRect( topL, bottomR );
        }
      }
      // Selection from bottom to top
      else
      {
        // From left to right
        if( m_lastPos.x() < actualPos.x() )
        {
          QPoint topL( m_lastPos.x(), actualPos.y() );
          QPoint bottomR( actualPos.x() - 1, m_lastPos.y() - 1 );
          m_selectedArea = QRect( topL, bottomR );
        }
        // From right to left
        else
        {
          QPoint bottomR = m_lastPos - QPoint( 1, 1 );
          m_selectedArea = QRect( actualPos, bottomR );
        }
      }
    }
    else // if tool() == BlockSelectionTool || MaskTool || EraserTool
    {
      m_blockTrackEnable = false;
      // If cursor is inside the selected area, we most redraw 
      // the selection rect because it may be smaller.
      if( m_selectedArea.contains( actualPos ) )
      {
        m_selectedArea = m_grid.rectContains( m_lastPos );
      }

      m_selectedArea = m_selectedArea.united( m_grid.rectContains( actualPos ) );
    }

    // Intercept the selected area with the image area to limit the 
    // selection only to the image area, preventing it to come outside 
    // the image.
    m_selectedArea &= QRect( 0, 0, m_pixmap.width(), m_pixmap.height() );

    // Update only the united area
    updateRect = updateRect.united( viewToWindow( m_selectedArea ) );

    // "When rendering with a one pixel wide pen the QRect's
    // boundary line will be rendered to the right and below the 
    // mathematical rectangle's boundary line.", in QT4 doc.
    // Our selection pen width is 1, let's adjust the rendering area.             
    updateRect.adjust( 0, 0, 1, 1 );

    update( updateRect.normalized() );
    return;
  }

  // If no mouse button is pressed and bloks selection tool is set, then
  // track the bloks.
  if( m_blockTrackEnable )
  {
    updateRect = viewToWindow( m_selectedArea );

    if( isPosValid( actualPos ) )
    {
      m_selectedArea = m_grid.rectContains( actualPos );
    }
    else
    {
      m_selectedArea = QRect();
    }
    updateRect = updateRect.united( viewToWindow( m_selectedArea ) );
    updateRect.adjust( 0, 0, 1, 1 );
    update( updateRect );
    return;
  }
}

void ViewArea::mouseReleaseEvent( QMouseEvent *event )
{
  event->accept();

  QPoint vpos = windowToView( event->pos() );

  if( event->button() == Qt::LeftButton && m_lastPos != QPoint( -1, -1 ) )
  {
    // Normal Mode ------------------------------------------------------
    if( mode() == NormalMode )
    {
      if( vpos == m_lastPos )
      {
        m_selectedArea = QRect();

        if( tool() == BlockSelectionTool )
          m_blockTrackEnable = true;
      }
      emit selectionChanged( m_selectedArea );

    }

    // Mask Mode -----------------------------------------------------------
    else if( mode() == MaskMode )
    {
      if( !m_selectedArea.isNull() )
      {
        updateMask( m_selectedArea );
      }

      m_selectedArea = QRect();
      m_blockTrackEnable = true;
    }
    m_newShape = false;
    update();
    unsetCursor();
    m_lastPos = QPoint( -1, -1 );
  }
}
////////////////////////////////////////////////////////////////////////////////

bool ViewArea::isPosValid( const QPoint &pos ) const
{

  if( pos.x() < 0 || pos.y() < 0 || pos.x() >= m_pixmap.width() || pos.y() >= m_pixmap.height() )
    return false;
  else
    return true;
}

////////////////////////////////////////////////////////////////////////////////

QPoint ViewArea::windowToView( const QPoint& pt ) const
{
  QPoint p;
  p.setX( static_cast<int>( ( pt.x() - m_xOffset ) / m_zoomFactor ) );
  p.setY( static_cast<int>( ( pt.y() - m_yOffset ) / m_zoomFactor ) );

  return p;
}

QRect ViewArea::windowToView( const QRect& rc ) const
{
  QRect r;

  r.setTopLeft( windowToView( rc.topLeft() ) );
//     r.setRight ( (int)( ceil(( rc.right()  - m_xOffset)/m_zoomFactor  )));
//     r.setBottom( (int)( ceil(( rc.bottom()- m_yOffset)/m_zoomFactor  )));
//     r.setRight ( static_cast<int>(( rc.right() - m_xOffset ) / m_zoomFactor +1));
//     r.setBottom( static_cast<int>(( rc.bottom() - m_xOffset ) / m_zoomFactor+1));
  r.setBottomRight( windowToView( rc.bottomRight() ) );
  return r;
}

QPoint ViewArea::viewToWindow( const QPoint& pt ) const
{
  QPoint p;

  p.setX( static_cast<int>( pt.x() * m_zoomFactor + m_xOffset ) );
  p.setY( static_cast<int>( pt.y() * m_zoomFactor + m_yOffset ) );

  return p;
}

QRect ViewArea::viewToWindow( const QRect& rc ) const
{
  QRect r;

  r.setTopLeft( viewToWindow( rc.topLeft() ) );
//     r.setRight ( (int)( ceil(( rc.right() +1+m_xOffset )*m_zoomFactor ) - 1 ));
//     r.setBottom( (int)( ceil(( rc.bottom()+1+m_yOffset )*m_zoomFactor ) - 1 ));
//     r.setRight ( (int)( ceil(( rc.right()+0.5)*m_zoomFactor  )+ m_xOffset )-1);
//     r.setBottom( (int)( ceil(( rc.bottom()+0.5)*m_zoomFactor ) +m_yOffset )-1);
// qDebug()<<"Right = "<< r.right();
//     r.setRight ( static_cast<int>(( rc.right()+1) * m_zoomFactor + m_xOffset -1) );
//     r.setBottom( static_cast<int>(( rc.bottom()+1) * m_zoomFactor + m_yOffset -1));
  r.setBottomRight( viewToWindow( rc.bottomRight() ) );

  return r;
}

////////////////////////////////////////////////////////////////////////////////
//                           Masks Management
////////////////////////////////////////////////////////////////////////////////
void ViewArea::updateMask( const QRect &rect )
{
  switch( tool() )
  {
  case MaskTool:
  {
    // Add rect to the mask
    QPainter painter( &m_mask );
    painter.setBrush( Qt::color1 );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rect );
    painter.end();
    break;
  }
  case EraserTool:
  {
    // Clears rect area in the mask
    QPainter painter( &m_mask );
    painter.setBrush( Qt::color0 );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rect );
    painter.end();
    break;
  }
  default: /* Do Nothing */
    ;
  }
}

} // NameSpace SCode
