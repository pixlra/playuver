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
 * \file     ViewArea.h
 * \brief    Function to manage image display
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#ifndef __VIEWAREA_H__
#define __VIEWAREA_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"
#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QColor>
#include <QTimer>
#include "GridManager.h"
#include "lib/PlaYUVerFrame.h"

namespace plaYUVer
{

class PlaYUVerStream;

/**
 *
 */
class ViewArea: public QWidget
{
Q_OBJECT

public:

  enum ViewModes
  {
    NavigationView,
    NormalSelectionView,
    BlockSelectionView,
  };

  ViewArea( QWidget *parent = 0 );

  Void setImage( PlaYUVerFrame* pcFrame );
  Void setImage( const QPixmap &pixmap );

  Void setMode( Int mode );
  Void setMaskColor( const QColor &color = QColor() );

  /**
   * Clears any contents.
   */
  Void clear();

  /**
   * Select tool from the menu
   */
  Void setTool( UInt view );

  /**
   * Clears any mask content.
   */
  Void clearMask();

  QPixmap image() const
  {
    return m_pixmap;
  }
  QBitmap mask() const
  {
    return m_mask;
  }
  QRect selectedArea() const
  {
    return m_selectedArea;
  }
  GridManager gridManager() const
  {
    return m_grid;
  }

  QColor maskColor() const
  {
    return m_maskColor;
  }

  Double getZoomFactor()
  {
    return m_zoomFactor;
  }

  // Scale function. Return used scale value (it may change when it touches the min or max zoom value)
  Double scaleZoomFactor( Double scale, QPoint center, QSize minimumSize );

Q_SIGNALS:
  void selectionChanged( const QRect &rect );
  void positionChanged( const QPoint &pos );
  void scrollBarMoved( QPoint offset );
  void zoomFactorChanged_byWheel( const double factor, const QPoint center );

public Q_SLOTS:
//  Void setNormalMode();
//  Void setMaskMode();
//  Void setMaskTool();
//  Void setEraserTool();
//  Void setNormalSelectionTool();
//  Void setBlockSelectionTool();
  void setGridVisible( bool enable );
  void setSnapToGrid( bool enable );
//  void setSelectedArea( QRect &rect );

protected:
  Void paintEvent( QPaintEvent *event );
  Void resizeEvent( QResizeEvent * event );
  Void mousePressEvent( QMouseEvent *event );
  Void mouseMoveEvent( QMouseEvent *event );
  Void mouseReleaseEvent( QMouseEvent *event );
  Void wheelEvent( QWheelEvent *event );

  Void updateSize();
  Void updateOffset();

private:

  enum ViewMode
  {
    NormalMode,
    MaskMode,
  };

  enum Tool
  {
    NavigationTool,
    SelectionTool,
    MaskTool,
    EraserTool
  };

  ViewMode mode() const
  {
    return m_mode;
  }

  Tool tool() const
  {
    return m_eTool;
  }

  Void initZoomWinRect();
  Void startZoomWinTimer();
  Void setZoomFactor( Double );

  Bool isPosValid( const QPoint &pos ) const;
  Void updateMask( const QRect &rect );

  QPoint windowToView( const QPoint& pt ) const;
  QRect windowToView( const QRect& rc ) const;

  QPoint viewToWindow( const QPoint& pt ) const;
  QRect viewToWindow( const QRect& rc ) const;

  PlaYUVerFrame *m_pcCurrFrame;
  Pel m_uiPixelHalfScale;
  QPixmap m_pixmap;
  QBitmap m_mask;
  QRect m_selectedArea;
  QPoint m_lastPos;
  QPoint m_lastWindowPos;
  GridManager m_grid;
  ViewMode m_mode;
  Tool m_eTool;
  QColor m_maskColor;
  Double m_zoomFactor;
  Int m_xOffset;
  Int m_yOffset;
  Bool m_blockTrackEnable;
  Bool m_newShape;
  Bool m_gridVisible;
  Bool m_snapToGrid;
  Bool m_cursorInGrid;
  Bool m_visibleZoomRect;
  Double m_dZoomWinRatio;
  QTimer m_zoomWinTimer;

};

}  // NameSpace plaYUVer

#endif // __VIEWAREA_H__
