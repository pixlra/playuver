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
#ifndef VIEWAREA_H
#define VIEWAREA_H

#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QColor>

#include "gridmanager.h"

namespace SCode
{

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

  enum Tool
  {
    NormalSelectionTool, BlockSelectionTool, MaskTool, EraserTool
  };

  ViewArea( QWidget *parent = 0 );

  void setImage( const QPixmap &pixmap );
  void setMode( ViewMode mode );
  void setTool( Tool tool );
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
  Tool tool() const
  {
    return m_tool;
  }
  QColor maskColor() const
  {
    return m_maskColor;
  }

//     QSize sizeHint() const;

signals:
  void selectionChanged( const QRect &rect );
  void positionChanged( const QPoint &pos );
  void updateStatusBar( const QPoint &actualPos, int &luma, int &chromaU, int &chromaV );
  void zoomFactorChanged( double );

public slots:
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

  QPixmap m_pixmap;
  QBitmap m_mask;
  QRect m_selectedArea;
  QPoint m_lastPos;
  GridManager m_grid;
  ViewMode m_mode;
  Tool m_tool;
  QColor m_maskColor;
  double m_zoomFactor;
  int m_xOffset;
  int m_yOffset;
  bool m_blockTrackEnable;
  bool m_newShape;
  bool m_gridVisible;
  bool m_snapToGrid;
  bool m_cursorInGrid;

  bool flag;  //lukas

};

} // NameSpace SCode

#endif // VIEWAREA_H
