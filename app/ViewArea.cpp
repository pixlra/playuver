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
 * \file     ViewArea.cpp
 * \brief    Function to manage image display
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#include <cmath>

#include <QCoreApplication>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QImage>
#include <QColor>
#include <QString>
#include <QPixmapCache>
#include <QRectF>
#include <QDebug>
#include "ViewArea.h"
#include "GridManager.h"

namespace plaYUVer
{

static const QColor selectionColor = Qt::cyan;
static const QColor imageMaskColor = Qt::green;
static const QColor eraserColor = Qt::red;

ViewArea::ViewArea( QWidget *parent ) :
        QWidget( parent )
{
  setMouseTracking( true );

  m_pcCurrFrame = NULL;
  m_pixmap = QPixmap();
  m_grid = GridManager();
  m_mask = QBitmap();
  m_selectedArea = QRect();
  m_dZoomFactor = 1;
  m_xOffset = 0;
  m_yOffset = 0;
  m_mode = NormalMode;
  m_eTool = NavigationTool;
  m_bGridVisible = false;
  m_snapToGrid = false;
  m_blockTrackEnable = false;
  m_visibleZoomRect = true;

  m_zoomWinTimer.setSingleShot( true );
  m_zoomWinTimer.setInterval( 2000 );
  connect( &m_zoomWinTimer, SIGNAL( timeout() ), this, SLOT( update() ) );
}

Void ViewArea::setImage( PlaYUVerFrame* pcFrame )
{
  m_pcCurrFrame = pcFrame;
  m_uiPixelHalfScale = 1 << ( m_pcCurrFrame->getBitsPel() - 1 );
  m_pcCurrFrame->fillRGBBuffer();
  QImage qimg = QImage( m_pcCurrFrame->getRGBBuffer(), m_pcCurrFrame->getWidth(), m_pcCurrFrame->getHeight(), QImage::Format_RGB32 );
  setImage( QPixmap::fromImage( qimg ) );
}

Void ViewArea::setImage( const QPixmap &pixmap )
{
  m_pixmap = pixmap;
  m_mask = QBitmap( pixmap.width(), pixmap.height() );
  m_mask.clear();
  updateSize();
  update();
  updateGeometry();

  initZoomWinRect();
}

Void ViewArea::clear()
{
  m_pixmap = QPixmap();
  m_mask = QBitmap();
  m_mode = NormalMode;
  updateSize();
  update();
  updateGeometry();
}

Void ViewArea::setTool( UInt view )
{
  switch( view )
  {
  case NavigationView:
    m_eTool = NavigationTool;
    break;
  case NormalSelectionView:
    m_eTool = SelectionTool;
    m_snapToGrid = false;
    m_blockTrackEnable = false;
    break;
  case BlockSelectionView:
    m_eTool = SelectionTool;
    m_snapToGrid = true;
    m_blockTrackEnable = true;
    break;
  }

  if( m_eTool != SelectionTool )
  {
    m_selectedArea = QRect();
    emit selectionChanged( m_selectedArea );
  }
  update();
}

Void ViewArea::setGridVisible( Bool enable )
{
  m_bGridVisible = enable;
  update();
}

Void ViewArea::clearMask()
{
  m_mask.clear();
}

/**
 * Zoom related function
 */
Void ViewArea::startZoomWinTimer()
{
  m_zoomWinTimer.start();
}

Void ViewArea::setZoomFactor( Double f )
{
  m_dZoomFactor = f;

  updateSize();
  startZoomWinTimer();
  update();
}

Double ViewArea::scaleZoomFactor( Double scale, QPoint center, QSize minimumSize )
{
  Double maxZoom = 100.0;
  Double minZoom = 0.01;  //( 1.0 / m_pixmap.width() );
  Double new_scale = 1.0;

  if( ( m_dZoomFactor == minZoom ) && ( scale < 1 ) )
    return new_scale;

  if( ( m_dZoomFactor == maxZoom ) && ( scale > 1 ) )
    return new_scale;

  Double zoomFactor = m_dZoomFactor * scale * 100.0;
  zoomFactor = round( zoomFactor );
  zoomFactor = zoomFactor / 100.0;
  scale = zoomFactor / m_dZoomFactor;

  if( !minimumSize.isNull() )
  {
    Double cw = m_pixmap.width() * m_dZoomFactor;
    Double ch = m_pixmap.height() * m_dZoomFactor;
    Double fw = m_pixmap.width() * zoomFactor;
    Double fh = m_pixmap.height() * zoomFactor;
    Double mw = minimumSize.width();
    Double mh = minimumSize.height();

    if( ( cw < mw ) && ( ch < mh ) && ( scale < 1 ) )
    {
      return new_scale;
    }

    if( ( fw < mw ) && ( fh < mh ) && ( scale < 1 ) )
    {
      Double wfactor = mw / fw;
      Double hfactor = mh / fh;

      if( wfactor < hfactor )
        scale = wfactor;
      else
        scale = hfactor;

      zoomFactor = zoomFactor * scale * 100.0;
      zoomFactor = floor( zoomFactor );
      zoomFactor = zoomFactor / 100.0;
      scale = zoomFactor / m_dZoomFactor;
    }
  }

  new_scale = scale;

  if( zoomFactor < minZoom )
  {
    zoomFactor = minZoom;
    new_scale = zoomFactor / m_dZoomFactor;
  }
  else
  {
    if( zoomFactor > maxZoom )
    {
      zoomFactor = maxZoom;
      new_scale = zoomFactor / m_dZoomFactor;
    }
  }

  setZoomFactor( zoomFactor );

  return new_scale;
}

//Void ViewArea::setMode( Int mode )
//{
//  if( m_mode == mode )
//    return;
//
//  m_mode = mode;
//  update();
//}
//
//Void ViewArea::setNormalMode()
//{
//  setMode( NormalMode );
//}
//
//Void ViewArea::setMaskMode()
//{
//  setMode( MaskMode );
//}
//
//Void ViewArea::setMaskColor( const QColor &color )
//{
//  m_maskColor = color;
//  update();
//}
//
//Void ViewArea::setMaskTool()
//{
//  setMode( MaskMode );
//  setTool( MaskTool );
//  m_blockTrackEnable = true;
//}
//
//Void ViewArea::setEraserTool()
//{
//  setMode( MaskMode );
//  setTool( EraserTool );
//  m_blockTrackEnable = true;
//}
//
//Void ViewArea::setSelectionTool()
//{
//  setMode( NormalMode );
//  setTool( SelectionTool );
//  m_blockTrackEnable = false;
//}
//
//Void ViewArea::setBlockSelectionTool()
//{
//  setMode( NormalMode );
//  setTool( BlockSelectionTool );
//  m_blockTrackEnable = true;
//}

Void ViewArea::setSnapToGrid( Bool enable )
{
  m_snapToGrid = enable;
}

//Void ViewArea::setSelectedArea( QRect &rect )
//{
//  if( rect.isNull() )
//  {
//    update();
//    return;
//  }
//
//  QRect updateRect;
// // setNormalMode();
//
//  if( m_selectedArea.isNull() )
//  {
//    m_selectedArea = rect.normalized();
//    updateRect = m_selectedArea;
//  }
//  else
//  {
//    updateRect = m_selectedArea;
//    m_selectedArea = rect.normalized();
//    updateRect = updateRect.united( m_selectedArea );
//  }
//  updateRect.adjust( 0, 0, 1, 1 );
//  update( updateRect.normalized() );
//}

Void ViewArea::initZoomWinRect()
{
  Int iMinX = 80;
  Int iMinY = 80;

  Int iMaxX = iMinX * 5;
  Int iMaxY = iMinY * 5;

  double dSizeRatio, dWinZoomRatio;

  Int iHorizontalImg = ( m_pixmap.width() > m_pixmap.height() ) ? 1 : 0;

  if( iHorizontalImg )
  {
    dSizeRatio = ( double )m_pixmap.width() / ( double )m_pixmap.height();
  }
  else
  {
    dSizeRatio = ( double )m_pixmap.height() / ( double )m_pixmap.width();
  }

  if( dSizeRatio > 5.0 )
  {
    if( iHorizontalImg )
    {
      dWinZoomRatio = ( double )( m_pixmap.width() * 1024 / iMaxX ) / 1000.0;
    }
    else
    {
      dWinZoomRatio = ( double )( m_pixmap.height() * 1024 / iMaxY ) / 1000.0;
    }
  }
  else
  {
    if( iHorizontalImg )
    {
      dWinZoomRatio = ( double )( m_pixmap.height() * 1024 / iMinY ) / 1000.0;
    }
    else
    {
      dWinZoomRatio = ( double )( m_pixmap.width() * 1024 / iMinX ) / 1000.0;
    }
  }

  m_dZoomWinRatio = dWinZoomRatio;
}

////////////////////////////////////////////////////////////////////////////////
//                            Geometry Updates 
////////////////////////////////////////////////////////////////////////////////
Void ViewArea::updateSize()
{
  Int w = m_pixmap.width() * m_dZoomFactor;
  Int h = m_pixmap.height() * m_dZoomFactor;
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

Void ViewArea::updateOffset()
{
  if( width() > m_pixmap.width() * m_dZoomFactor )
  {
    m_xOffset = ( width() - m_pixmap.width() * m_dZoomFactor ) / 2;
  }
  else
  {
    m_xOffset = 0;
  }
  if( height() > m_pixmap.height() * m_dZoomFactor )
  {
    m_yOffset = ( height() - m_pixmap.height() * m_dZoomFactor ) / 2;
  }
  else
  {
    m_yOffset = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
//                              Resize Event  
////////////////////////////////////////////////////////////////////////////////
Void ViewArea::resizeEvent( QResizeEvent *event )
{
  if( size().isEmpty() || m_pixmap.isNull() )
    return;

  updateOffset();
  startZoomWinTimer();
  update();
}
////////////////////////////////////////////////////////////////////////////////
//                              PaInt Event
////////////////////////////////////////////////////////////////////////////////

Void ViewArea::paintEvent( QPaintEvent *event )
{
  QRect winRect = event->rect();

  if( visibleRegion().isEmpty() )
    return;

  if( size().isEmpty() || m_pixmap.isNull() )
    return;

  QPainter paInter( this );

  // Save the actual paInter properties and scales the coordinate system.
  paInter.save();
  paInter.translate( m_xOffset, m_yOffset );
  paInter.scale( m_dZoomFactor, m_dZoomFactor );

  // This line is for fast paiting. Only visible area of the image is paInted.
  // We take the exposed rect from the event (that gives us scroll/expose optimizations for free – no need
  // to draw the whole pixmap if your widget is only partially exposed), and reverse map it with the paInter matrix.
  // That gives us the part of the pixmap that has actually been exposed.
  // See: http://blog.qt.digia.com/blog/2006/05/13/fast-transformed-pixmapimage-drawing/
  QRect exposedRect = paInter.worldTransform().inverted().mapRect( event->rect() ).adjusted( -1, -1, 1, 1 );
  // Draw the pixmap.
  paInter.drawPixmap( exposedRect, m_pixmap, exposedRect );

  // Draw the Grid if it's visible.
  if( m_bGridVisible )
  {
    // Do we need to draw the whole grid?
    // To know that, we need to perform a transformation of the rectangle 
    // area that the paInter needs to update - transform the windows
    // coordinates (origin at the top-left corner of the widget), to the 
    // relatives coordinates of the image at it's original size (origin at 
    // the top-left corner of the image).        
    QRect vr = windowToView( winRect );

    // Now we have the (to update) rectangle area on a coordinates system 
    // that has it's origin at the top-left corner of the image. That 
    // is, is referenced to the not scaled image.
    // To know what image area we need to update, just Intersects the
    // rectangle area with the image area. 
    vr &= QRect( 0, 0, m_pixmap.width(), m_pixmap.height() );

    // Set up for the grid drawer.
    paInter.setRenderHint( QPainter::Antialiasing );

    // Draw grid.
    m_grid.drawGrid( m_pixmap, vr, &paInter );
  }

  paInter.restore();

  // Draw a border around the image.
  /*  if( m_xOffset || m_yOffset )
   {
   paInter.setPen( Qt::black );
   paInter.drawRect( m_xOffset - 1, m_yOffset - 1, m_pixmap.width() * m_dZoomFactor + 1, m_pixmap.height() * m_dZoomFactor + 1 );
   }*/

  // Draw pixel values in grid
  if( m_dZoomFactor >= 50.0 )
  {
    Int imageWidth = m_pixmap.width();
    Int imageHeight = m_pixmap.height();
    PlaYUVerFrame::Pixel sPixelValue;

    QFont font( "Helvetica" );
    font.setPixelSize( 12 );
    paInter.setFont( font );

    QRect vr = windowToView( winRect );
    vr &= QRect( 0, 0, imageWidth, imageHeight );

    for( Int i = vr.x(); i <= vr.right(); i++ )
    {
      for( Int j = vr.y(); j <= vr.bottom(); j++ )
      {
        QPoint pixelTopLeft( i, j );

        QRect pixelRect( viewToWindow( pixelTopLeft ), QSize( m_dZoomFactor, m_dZoomFactor ) );

        Int frFormat = m_pcCurrFrame->getColorSpace();

        if( frFormat == PlaYUVerFrame::COLOR_YUV )
        {
          sPixelValue = m_pcCurrFrame->getPixelValue( pixelTopLeft.x(), pixelTopLeft.y() );

          if( sPixelValue.Y() < m_uiPixelHalfScale )
            paInter.setPen( QColor( Qt::white ) );
          else
            paInter.setPen( QColor( Qt::black ) );

          paInter.drawText( pixelRect, Qt::AlignCenter,
              "Y: " + QString::number( sPixelValue.Y() ) + "\n" + "U: " + QString::number( sPixelValue.Cb() ) + "\n" + "V: "
                  + QString::number( sPixelValue.Cr() ) );
        }
        if( frFormat == PlaYUVerFrame::COLOR_GRAY )
        {
          sPixelValue = m_pcCurrFrame->getPixelValue( pixelTopLeft.x(), pixelTopLeft.y() );

          if( sPixelValue.Y() < m_uiPixelHalfScale )
            paInter.setPen( QColor( Qt::white ) );
          else
            paInter.setPen( QColor( Qt::black ) );

          paInter.drawText( pixelRect, Qt::AlignCenter, "Y: " + QString::number( sPixelValue.Y() ) );
        }

        if( ( frFormat == PlaYUVerFrame::COLOR_RGB ) )
        {
          sPixelValue = m_pcCurrFrame->getPixelValue( pixelTopLeft.x(), pixelTopLeft.y() );

          if( ( sPixelValue.R() + sPixelValue.G() + sPixelValue.B() ) < ( m_uiPixelHalfScale * 3 ) )
            paInter.setPen( QColor( Qt::white ) );
          else
            paInter.setPen( QColor( Qt::black ) );

          paInter.drawText( pixelRect, Qt::AlignCenter,
              "R: " + QString::number( sPixelValue.R() ) + "\n" + "G: " + QString::number( sPixelValue.G() ) + "\n" + "B: "
                  + QString::number( sPixelValue.B() ) );
        }
      }
    }

    QColor color( Qt::white );
    QPen mainPen = QPen( color, 1, Qt::SolidLine );
    paInter.setPen( mainPen );

    // Draw vertical line
    for( Int x = vr.x(); x <= ( vr.right() + 1 ); x++ )
    {
      // Always draw the full line otherwise the line stippling
      // varies with the location of view area and we get glitchy
      // patterns.
      paInter.drawLine( viewToWindow( QPoint( x, 0 ) ), viewToWindow( QPoint( x, imageHeight ) ) );
    }
    // Draw horizontal line
    for( Int y = vr.y(); y <= ( vr.bottom() + 1 ); y++ )
    {
      paInter.drawLine( viewToWindow( QPoint( 0, y ) ), viewToWindow( QPoint( imageWidth, y ) ) );
    }
  }

  // VISIBLE ZOOM RECT
#if 0
  if( m_visibleZoomRect && m_zoomWinTimer.isActive() )
  {
    double dRatio = m_dZoomWinRatio;

    QRect cImg = QRect(0, 0, m_pixmap.width(), m_pixmap.height() );
    QPoint cZWinRBpos = QPoint(winRect.bottomRight()) - QPoint(15,15);
    QRect cImgWinRect(0, 0, round((double)cImg.width()/dRatio), round((double)cImg.height()/dRatio) );
    cImgWinRect.moveBottomRight(cZWinRBpos);

    QRect vr = windowToView( winRect );
    QRect cVisibleImg = vr & cImg;
    //cVisibleImg.moveTopLeft(vr.topLeft());
    QRect cVisibleWinRect;
    cVisibleWinRect.setLeft(floor((double)cVisibleImg.x()/dRatio));
    cVisibleWinRect.setTop(floor((double)cVisibleImg.y()/dRatio));
    cVisibleWinRect.setRight(round((double)cVisibleImg.right()/dRatio));
    cVisibleWinRect.setBottom(round((double)cVisibleImg.bottom()/dRatio));

    paInter.fillRect(cImgWinRect, QBrush(QColor(128, 128, 128, 128)));
    paInter.setPen(QColor(50, 50, 50, 128));
    paInter.drawRect(cImgWinRect);

    cVisibleWinRect.moveTopLeft( cImgWinRect.topLeft() + cVisibleWinRect.topLeft() );

    cVisibleWinRect = cVisibleWinRect & cImgWinRect;

    if(cVisibleWinRect.left()<0)
    cVisibleWinRect.moveLeft(0);
    if(cVisibleWinRect.top()<0)
    cVisibleWinRect.moveTop(0);

    if(cVisibleWinRect.width()<=0)
    cVisibleWinRect.setWidth(1);
    if(cVisibleWinRect.height()<=0)
    cVisibleWinRect.setHeight(1);

    paInter.fillRect(cVisibleWinRect, QBrush(QColor(200, 200, 200, 128)));
    paInter.setPen(QColor(255, 255, 255, 128));
    paInter.drawRect(cVisibleWinRect);

    //qDebug() << "Debug VisibleZoomRect: " << winRect << vr << cImgWinRect << cVisibleWinRect << cVisibleImg << dRatio;

  }
#endif

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
        paInter.setBrush( Qt::NoBrush );
      else
        paInter.setBrush( brush );

      paInter.setPen( Qt::darkCyan );

//             paInter.drawRect( sr/*m_selectedArea*/ );
    }
    else
    {
      // 1) Cover the image with a light gray except the selected area
      QColor fill( Qt::lightGray );
      fill.setAlpha( 150 );

      QBrush brush( fill );
      paInter.setBrush( fill );
      paInter.setPen( Qt::NoPen );
      QPainterPath myPath;
      QRect imgr = viewToWindow( QRect( 0, 0, m_pixmap.width(), m_pixmap.height() ) );

      myPath.addRect( imgr );
      myPath.addRect( sr );  //m_selectedArea
      paInter.drawPath( myPath );

      // 2) Draw the selection rectangle   
      paInter.setBrush( Qt::NoBrush );
      paInter.setPen( Qt::darkCyan );
//             paInter.drawRect( sr/*m_selectedArea*/ );
    }

    if( !ir.isNull() )
      paInter.drawRect( sr/*m_selectedArea*/);

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
  paInter.setPen( color );
  paInter.save();
  paInter.translate( m_xOffset, m_yOffset );
  paInter.scale( m_dZoomFactor, m_dZoomFactor );
  paInter.drawPixmap( QPoint( 0, 0 ), m_mask );
  paInter.restore();

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
        color = selectionColor;  // ?Problems!
      }

      color.setAlpha( 120 );
      brush = QBrush( color );

      if( m_blockTrackEnable )
        paInter.setBrush( Qt::NoBrush );
      else
        paInter.setBrush( brush );

      paInter.setPen( color );
      if( !ir.isNull() )
        paInter.drawRect( sr );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                              Mouse Events  
////////////////////////////////////////////////////////////////////////////////
Void ViewArea::wheelEvent( QWheelEvent *event )
{
  Double scale;
  Double usedScale;

  if( event->modifiers() & Qt::ControlModifier )
  {
    scale = 0.001 * event->delta();
    if( scale > 0 )
      scale = 1.25;
    else
      scale = 0.8;

    QWidget *p = parentWidget();
    QSize minimumSize = QSize( p->size().width() - 5, p->size().height() - 5 );
    usedScale = scaleZoomFactor( scale, event->pos(), minimumSize );
    if( usedScale != 1.0 )
    {
      emit zoomFactorChanged_byWheel( usedScale, event->pos() );
    }
  }
}

Void ViewArea::mousePressEvent( QMouseEvent *event )
{
  event->accept();

  QPoint vpos = windowToView( event->pos() );

  if( event->button() == Qt::MidButton )
  {

  }
  if( event->button() == Qt::LeftButton )
  {
    if( tool() == NavigationTool )
    {
      if( !( m_xOffset && m_yOffset ) )
        setCursor( Qt::ClosedHandCursor );

      m_lastWindowPos = event->pos();
    }
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
      // Find if the cursor is near a grid Intersection
      Bool isNear = m_grid.isNear( m_lastPos );
      isNear = true;
      if( isNear )
      {
        // The grid 'near Intersection' found when used isNear()
        m_lastPos = m_grid.nearPos();
      }
    }

    m_newShape = true;

    if( tool() == SelectionTool )
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
//    m_selectedArea = m_grid.rectContains( m_lastPos );

    QRect updateRect = viewToWindow( m_selectedArea );
    updateRect.adjust( 0, 0, 1, 1 );
    update( updateRect.normalized() );
  }
}

Void ViewArea::mouseMoveEvent( QMouseEvent *event )
{
  event->accept();

#ifndef _MSC_VER
  // Add this code line to aVoid slow navigation with some specific mouses
  // This seems to always appear on windows
  if( qApp->hasPendingEvents())
  return;
#endif

  QPoint actualPos = windowToView( event->pos() );
  QRect updateRect;

  emit positionChanged( actualPos );

  // If mouse left button pressed
  if( event->buttons() == Qt::LeftButton && m_lastPos != QPoint( -1, -1 ) )
  {
    // Grid tracking
    if( m_snapToGrid )
    {
      // Find if the cursor is near a grid Intersection
      Bool isNear = m_grid.isNear( actualPos );

      if( isNear )
      {
        // Return the last grid near Intersection found
        actualPos = m_grid.nearPos();
      }
    }

    if( tool() == NavigationTool )
    {
      QPoint offset = m_lastWindowPos - event->pos();

      startZoomWinTimer();
      emit scrollBarMoved( offset );
    }

    updateRect = viewToWindow( m_selectedArea );

    if( tool() == SelectionTool )
    {
      // If the selection is only vertical or horizontal then we have one
      // of the dimentions null.
      if( actualPos.x() == m_lastPos.x() && actualPos.y() == m_lastPos.y() )
      {
        m_selectedArea = QRect();
      }
      // Selection from top to bottom
      else if( m_lastPos.y() < actualPos.y() )
      {
        // From left to right
        if( m_lastPos.x() < actualPos.x() )
        {
          QPoint bottomR = actualPos;      // - QPoint( 1, 1 );
          m_selectedArea = QRect( m_lastPos, bottomR );
        }
        // From right to left
        else
        {
          QPoint topL( actualPos.x(), m_lastPos.y() );
          QPoint bottomR( m_lastPos.x(), actualPos.y() );

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
          QPoint bottomR( actualPos.x(), m_lastPos.y() );
          m_selectedArea = QRect( topL, bottomR );
        }
        // From right to left
        else
        {
          QPoint bottomR = m_lastPos;      // - QPoint( 1, 1 );
          m_selectedArea = QRect( actualPos, bottomR );
        }
      }
    }
    /*    else // if tool() == BlockSelectionTool || MaskTool || EraserTool
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
     */
    if( tool() == SelectionTool )
    {
      // Intercept the selected area with the image area to limit the
      // selection only to the image area, preventing it to come outside
      // the image.
      m_selectedArea &= QRect( 0, 0, m_pixmap.width(), m_pixmap.height() );

      // Update only the united area
//      updateRect = updateRect.united( viewToWindow( m_selectedArea ) );

      // "When rendering with a one pixel wide pen the QRect's
      // boundary line will be rendered to the right and below the
      // mathematical rectangle's boundary line.", in QT4 doc.
      // Our selection pen width is 1, let's adjust the rendering area.
//      updateRect.adjust( 0, 0, 1, 1 );

//      update( updateRect.normalized() );
      update();
    }
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

Void ViewArea::mouseReleaseEvent( QMouseEvent *event )
{
  event->accept();

  QPoint vpos = windowToView( event->pos() );

  if( event->button() == Qt::LeftButton && m_lastPos != QPoint( -1, -1 ) )
  {
    if( tool() == NavigationTool )
    {
      unsetCursor();
    }
    else if( tool() == SelectionTool )
    {
      // Normal Mode ------------------------------------------------------
      if( mode() == NormalMode )
      {
        if( vpos == m_lastPos )
        {
          m_selectedArea = QRect();
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
      m_lastPos = QPoint( -1, -1 );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

Bool ViewArea::isPosValid( const QPoint &pos ) const
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
  p.setX( static_cast<Int>( ( pt.x() - m_xOffset ) / m_dZoomFactor ) );
  p.setY( static_cast<Int>( ( pt.y() - m_yOffset ) / m_dZoomFactor ) );

  return p;
}

QRect ViewArea::windowToView( const QRect& rc ) const
{
  QRect r;

  r.setTopLeft( windowToView( rc.topLeft() ) );
//     r.setRight ( (Int)( ceil(( rc.right()  - m_xOffset)/m_dZoomFactor  )));
//     r.setBottom( (Int)( ceil(( rc.bottom()- m_yOffset)/m_dZoomFactor  )));
//     r.setRight ( static_cast<Int>(( rc.right() - m_xOffset ) / m_dZoomFactor +1));
//     r.setBottom( static_cast<Int>(( rc.bottom() - m_xOffset ) / m_dZoomFactor+1));
  r.setBottomRight( windowToView( rc.bottomRight() ) );
  return r;
}

QPoint ViewArea::viewToWindow( const QPoint& pt ) const
{
  QPoint p;

  p.setX( static_cast<Int>( pt.x() * m_dZoomFactor + m_xOffset ) );
  p.setY( static_cast<Int>( pt.y() * m_dZoomFactor + m_yOffset ) );

  return p;
}

QRect ViewArea::viewToWindow( const QRect& rc ) const
{
  QRect r;

  r.setTopLeft( viewToWindow( rc.topLeft() ) );
//     r.setRight ( (Int)( ceil(( rc.right() +1+m_xOffset )*m_dZoomFactor ) - 1 ));
//     r.setBottom( (Int)( ceil(( rc.bottom()+1+m_yOffset )*m_dZoomFactor ) - 1 ));
//     r.setRight ( (Int)( ceil(( rc.right()+0.5)*m_dZoomFactor  )+ m_xOffset )-1);
//     r.setBottom( (Int)( ceil(( rc.bottom()+0.5)*m_dZoomFactor ) +m_yOffset )-1);
// qDebug()<<"Right = "<< r.right();
//     r.setRight ( static_cast<Int>(( rc.right()+1) * m_dZoomFactor + m_xOffset -1) );
//     r.setBottom( static_cast<Int>(( rc.bottom()+1) * m_dZoomFactor + m_yOffset -1));
  r.setBottomRight( viewToWindow( rc.bottomRight() ) );

  return r;
}

////////////////////////////////////////////////////////////////////////////////
//                           Masks Management
////////////////////////////////////////////////////////////////////////////////
Void ViewArea::updateMask( const QRect &rect )
{
  switch( tool() )
  {
  case MaskTool:
  {
    // Add rect to the mask
    QPainter paInter( &m_mask );
    paInter.setBrush( Qt::color1 );
    paInter.setPen( Qt::NoPen );
    paInter.drawRect( rect );
    paInter.end();
    break;
  }
  case EraserTool:
  {
    // Clears rect area in the mask
    QPainter paInter( &m_mask );
    paInter.setBrush( Qt::color0 );
    paInter.setPen( Qt::NoPen );
    paInter.drawRect( rect );
    paInter.end();
    break;
  }
  default: /* Do Nothing */
    ;
  }
}

}  // NameSpace plaYUVer
