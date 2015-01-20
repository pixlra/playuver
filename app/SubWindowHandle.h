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
 * \file     SubWindowHandle.h
 * \brief    Sub windows handling
 */

#ifndef __SUBWINDOWHANDLE_H__
#define __SUBWINDOWHANDLE_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif

namespace plaYUVer
{

class SubWindowHandle: public QScrollArea
{
Q_OBJECT

private:
  QPoint m_cLastScroll;

  UInt m_uiCategory;
  QString m_cWindowName;

public:
  enum SubWindowCategories
  {
    VIDEO_SUBWINDOW = 0,
    MODULE_SUBWINDOW,
    PLOT_SUBWINDOW,
  };

  SubWindowHandle( QWidget *, UInt );
  ~SubWindowHandle();

  /**
   * Show the image at its original size
   */
  virtual Void normalSize() = 0;
  /**
   * Scale the image (zoomed in or out) to fit on the window.
   */
  virtual Void zoomToFit() = 0;
  /**
   * Scale the image (zoomed in or out) to speficied absolute zoom value (1.0 = original size).
   */
  virtual Void zoomToFactor( Double factor , QPoint center = QPoint() ) = 0;
  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  virtual Void scaleView( Double , QPoint center = QPoint() ) = 0;


  virtual Double getScaleFactor() = 0;

  Void setWindowName( QString );
  QString getWindowName();

  virtual Bool mayClose();

  UInt getCategory()
  {
    return m_uiCategory;
  }

protected:
  virtual void closeEvent( QCloseEvent *event ) = 0;
  QSize getScrollSize();

Q_SIGNALS:
  /**
   * Notify that zoom factor was changed by internal event (e.g. by mouse wheel)
   */
  void zoomFactorChanged_SWindow( const double, const QPoint );
  /**
   * Notify that scrollbar position was changed by internal event (e.g. by mouse panning )
   */
  void scrollBarMoved_SWindow( const QPoint );

  void updateStatusBar( const QString& );

public Q_SLOTS:
  void adjustScrollBarByScale( double scale, QPoint center );
  void adjustScrollBarByOffset( QPoint Offset );
  void updateLastScrollValue();

};

}  // NAMESPACE

#endif // __SUBWINDOWHANDLE_H__

