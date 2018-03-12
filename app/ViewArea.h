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
 * \file     ViewArea.h
 * \brief    Function to manage image display
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#ifndef __VIEWAREA_H__
#define __VIEWAREA_H__

#include "CommonDefs.h"
#include "GridManager.h"
#include "config.h"
#include "lib/CalypFrame.h"

#include <QBitmap>
#include <QColor>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

class CalypStream;

/**
 *
 */
class ViewArea : public QWidget
{
  Q_OBJECT

public:
  enum ViewModes
  {
    NavigationView,
    NormalSelectionView,
    BlockSelectionView,
  };

  ViewArea( QWidget* parent = 0 );

  void setImage( CalypFrame* pcFrame );
  void setImage( const QPixmap& pixmap );

  void setMode( int mode );
  void setMaskColor( const QColor& color = QColor() );

  /**
   * Clears any contents.
   */
  void clear();

  /**
   * Select tool from the menu
   */
  void setTool( unsigned int view );

  void setGridVisible( bool enable );

  /**
   * Clears any mask content.
   */
  void clearMask();

  QPixmap image() const { return m_pixmap; }
  QBitmap mask() const { return m_mask; }
  QRect selectedArea() const { return m_selectedArea; }
  GridManager gridManager() const { return m_grid; }
  QColor maskColor() const { return m_maskColor; }
  double getZoomFactor() { return m_dZoomFactor; }
  // Scale function. Return used scale value (it may change when it touches the
  // min or max zoom value)
  double scaleZoomFactor( double scale, QPoint center, QSize minimumSize );

Q_SIGNALS:
  void selectionChanged( const QRect& rect );
  void positionChanged( const QPoint& pos );
  void scrollBarMoved( QPoint offset );
  void zoomFactorChanged_byWheel( const double factor, const QPoint center );

public Q_SLOTS:
  //  void setNormalMode();
  //  void setMaskMode();
  //  void setMaskTool();
  //  void setEraserTool();
  //  void setNormalSelectionTool();
  //  void setBlockSelectionTool();
  void setSnapToGrid( bool enable );
  //  void setSelectedArea( QRect &rect );

protected:
  void paintEvent( QPaintEvent* event );
  void resizeEvent( QResizeEvent* event );
  void mousePressEvent( QMouseEvent* event );
  void mouseMoveEvent( QMouseEvent* event );
  void mouseReleaseEvent( QMouseEvent* event );
  void wheelEvent( QWheelEvent* event );

  void updateSize();
  void updateOffset();

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

  ViewMode mode() const { return m_mode; }
  Tool tool() const { return m_eTool; }
  void initZoomWinRect();
  void startZoomWinTimer();
  void setZoomFactor( double );

  bool isPosValid( const QPoint& pos ) const;
  void updateMask( const QRect& rect );

  QPoint windowToView( const QPoint& pt ) const;
  QRect windowToView( const QRect& rc ) const;

  QPoint viewToWindow( const QPoint& pt ) const;
  QRect viewToWindow( const QRect& rc ) const;

  CalypFrame* m_pcCurrFrame;

  ClpPel m_uiPixelHalfScale;

  Tool m_eTool;
  bool m_bGridVisible;

  QTimer m_zoomWinTimer;
  double m_dZoomWinRatio;
  double m_dZoomFactor;

  QPixmap m_pixmap;
  QBitmap m_mask;
  QRect m_selectedArea;
  QPoint m_lastPos;
  QPoint m_lastWindowPos;
  GridManager m_grid;
  ViewMode m_mode;

  QColor m_maskColor;
  int m_xOffset;
  int m_yOffset;
  bool m_blockTrackEnable;
  bool m_newShape;
  bool m_snapToGrid;
  bool m_cursorInGrid;
  bool m_visibleZoomRect;
};

#endif  // __VIEWAREA_H__
