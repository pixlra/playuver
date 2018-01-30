/*    This file is a part of PlaYUVer project
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
 * \file     SubWindowAbstract.h
 * \ingroup  PlaYUVerApp PlaYUVerApp_Subwindow
 * \brief    Abstract class to handle subwindows
 *
 * @defgroup PlaYUVerApp_Subwindow SubWindow definition
 * @{
 * \ingroup PlaYUVerApp
 *
 * Different SubWindow may be found in the PlaYUVerApp
 * Each one provide different functionality and serve
 * different purposes, ( e.g., displaying a video stream
 * or displaying a quality plot)
 *
 * @}
 */

#ifndef __SUBWINDOWABSTRACT_H__
#define __SUBWINDOWABSTRACT_H__

#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>
#include "PlaYUVerAppDefs.h"
#include "config.h"

class QHBoxLayout;
class QFocusEvent;
class QCloseEvent;

class PlaYUVerMdiSubWindow;

class SubWindowAbstract : public QWidget
{
  Q_OBJECT

private:
  QHBoxLayout* m_pcLayout;
  UInt m_uiCategory;
  QString m_cWindowName;

  PlaYUVerMdiSubWindow* m_cSubWindow;

public:
  enum SubWindowCategory
  {
    SUBWINDOW = 0,
    VIDEO_SUBWINDOW = 1,
    VIDEO_STREAM_SUBWINDOW = 2,
    MODULE_SUBWINDOW = 4,
    PLOT_SUBWINDOW = 8,
  };
  Q_DECLARE_FLAGS( SubWindowCategories, SubWindowCategory )

  SubWindowAbstract( QWidget*, UInt );
  ~SubWindowAbstract();

  /**
   * Show the image at its original size
   */
  virtual Void refreshSubWindow() = 0;

  /**
   * Show the image at its original size
   */
  virtual Void normalSize() = 0;
  /**
   * Scale the image (zoomed in or out) to fit on the window.
   */
  virtual Void zoomToFit() = 0;
  /**
   * Scale the image (zoomed in or out) to speficied absolute zoom value (1.0 =
   * original size).
   */
  virtual Void zoomToFactor( Double factor, QPoint center = QPoint() ) = 0;
  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  virtual Void scaleView( Double, QPoint center = QPoint() ) = 0;

  virtual Double getScaleFactor() = 0;

  /**
   * Size related functions
   */
  virtual QSize sizeHint() const;
  virtual QSize sizeHint( const QSize& ) const;

  /**
   * Window name
   */

  Void setWindowName( QString );
  QString getWindowName();

  virtual Bool mayClose();

  /**
   * Get category of the SubWindow
   * @return category value based on enum SubWindowCategories
   * @note This function should be used with enum SubWindowCategories
   *        with an & operation and it may belong to several categories
   */
  UInt getCategory() { return m_uiCategory; }
  /**
   * Check category of the SubWindow
   * @param checkCateory SubWindow category to check against;
   *        it should be enum SubWindowCategories type
   * @return true when SubWindow belong to checkCateory
   */
  Bool checkCategory( UInt checkCateory ) { return m_uiCategory & checkCateory; }
  Void setSubWindow( PlaYUVerMdiSubWindow* subWindow ) { m_cSubWindow = subWindow; }
  Void closeSubWindow();

protected:
  void focusInEvent( QFocusEvent* event );
  void closeEvent( QCloseEvent* event );

  Void setWidget( QWidget* widget );

Q_SIGNALS:
  /**
   * Notify that zoom factor was changed by internal event (e.g. by mouse wheel)
   */
  void zoomFactorChanged( const double, const QPoint );
  /**
   * Notify that scrollbar position was changed by internal event (e.g. by mouse
   * panning )
   */
  void scrollBarMoved( const double&, const double& );
  /**
   * Update stauts bars
   */
  void updateStatusBar( const QString& );

  void aboutToActivate( SubWindowAbstract* );
  void aboutToClose( SubWindowAbstract* );

public Q_SLOTS:
  void onDestroyed();
};

#endif  // __SUBWINDOWABSTRACT_H__
