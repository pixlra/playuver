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
 * \file     QualityHandle.h
 * \brief    Definition of the quality measurement sidebar
 */

#ifndef __QUALITYMEASUREMENT_H__
#define __QUALITYMEASUREMENT_H__

#include "config.h"
#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "VideoSubWindow.h"
#include "QualityMeasurementSidebar.h"

namespace plaYUVer
{

class VideoSubWindow;

class QualityHandle: public QWidget
{
Q_OBJECT
public:
  QualityHandle( QWidget*, QMdiArea * );
  ~QualityHandle();

  Void createActions();
  QMenu* createMenu();
  QDockWidget* createDock();
  Void updateMenus();

  Void readSettings();
  Void writeSettings();

  Void update( VideoSubWindow* currSubWindow );

private:
  QWidget* m_pcParet;
  QMdiArea* m_pcMainWindowMdiArea;
  enum
  {
    SELECT_REF_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;
  QActionGroup* m_actionGroupQualityMetric;
  QSignalMapper* m_mapperQualityMetric;
  Int m_iQualityMetricIdx;

  QMenu* m_pcMenuQuality;

  QDockWidget* m_pcQualityHandleDock;
  QualityMeasurementSidebar* m_pcQualityHandleSideBar;

Q_SIGNALS:
  void changed();

private Q_SLOTS:
  void slotQualityMetricChanged( int );
  void slotSelectReference();

};

}   // NAMESPACE

#endif // __QUALITYMEASUREMENTSIDEBAR_H__