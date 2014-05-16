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
 * \file     ViewArea.h
 * \brief    Function to manage image display
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#ifndef __VIEWAREA_H__
#define __VIEWAREA_H__

#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QColor>

#include "TypeDef.h"
#include "GridManager.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class InputStream;

/**
 *
 */
class ViewArea: public QWidget
{
Q_OBJECT

public:

  enum ViewMode
  {
    NormalMode, MaskMode,
  };

  ViewArea( QWidget *parent = 0 );

  void setImage( PlaYUVerFrame* pcFrame );
  void setImage( const QPixmap &pixmap );
  void setMode( ViewMode mode );
  void setMaskColor( const QColor &color = QColor() );

  /**
   * Clears any contents.
   */
  void clear();
  /**
   * Clears any mask content.
   */
  void clearMask();

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
  ViewMode mode() const
  {
    return m_mode;
  }
  enum eTool tool() const
  {
    return m_tool;
  }
  QColor maskColor() const
  {
    return m_maskColor;
  }

  void setInputStream( InputStream *stream );
  InputStream* getInputStream();

  Void zoomChangeEvent(Double factor, QPoint center);
  Double getZoomFactor();
  void setTool( enum eTool tool );

//     QSize sizeHint() const;

Q_SIGNALS:
  void selectionChanged( const QRect &rect );
  void positionChanged( const QPoint &pos, PlaYUVerFrame *frame );
  void moveScroll( QPoint offset );
  void zoomFactorChanged( double factor , QPoint center);

public Q_SLOTS:
  void setNormalMode();
  void setMaskMode();
  void setMaskTool();
  void setEraserTool();
  void setNormalSelectionTool();
  void setBlockSelectionTool();
  void setGridVisible( bool enable );
  void setSnapToGrid( bool enable );
  void setSelectedArea( QRect &rect );
  void setZoomFactor( double f );

protected:
  void paintEvent( QPaintEvent *event );
  void resizeEvent( QResizeEvent * event );
  void mousePressEvent( QMouseEvent *event );
  void mouseMoveEvent( QMouseEvent *event );
  void mouseReleaseEvent( QMouseEvent *event );
  void wheelEvent( QWheelEvent *event );

  void updateSize();
  void updateOffset();

private:

  bool isPosValid( const QPoint &pos ) const;
  void updateMask( const QRect &rect );

  QPoint windowToView( const QPoint& pt ) const;
  QRect windowToView( const QRect& rc ) const;

  QPoint viewToWindow( const QPoint& pt ) const;
  QRect viewToWindow( const QRect& rc ) const;

  PlaYUVerFrame *m_pcCurrFrame;
  QPixmap m_pixmap;
  QBitmap m_mask;
  QRect m_selectedArea;
  QPoint m_lastPos;
  QPoint m_lastWindowPos;
  GridManager m_grid;
  ViewMode m_mode;
  enum eTool m_tool;
  QColor m_maskColor;
  double m_zoomFactor;
  int m_xOffset;
  int m_yOffset;
  bool m_blockTrackEnable;
  bool m_newShape;
  bool m_gridVisible;
  bool m_snapToGrid;
  bool m_cursorInGrid;

  InputStream *m_pStream;

};

} // NameSpace plaYUVer

#endif // __VIEWAREA_H__
