/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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

#include "PlaYUVerAppDefs.h"
#include "config.h"
#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "QFuture"
#include "QualityMeasurementSidebar.h"

class PlaYUVerSubWindowHandle;
class VideoSubWindow;

class QualityHandle : public QWidget
{
  Q_OBJECT
 public:
  QualityHandle( QWidget*, PlaYUVerSubWindowHandle* );
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
  PlaYUVerSubWindowHandle* m_pcMainWindowManager;

  enum
  {
    SELECT_CURR_REF_ACT,
    PLOT_QUALITY,
    PLOT_SEVERAL_QUALITY,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;
  QActionGroup* m_actionGroupQualityMetric;
  QSignalMapper* m_mapperQualityMetric;
  Int m_iQualityMetricIdx;

  QMenu* m_pcMenuQuality;
  QMenu* m_pcSubMenuQualityMetrics;

  QDockWidget* m_pcQualityHandleDock;
  QualityMeasurementSidebar* m_pcQualityHandleSideBar;

  QFuture<Void> m_cMeasurementResult;
  Void measureQuality( QVector<VideoSubWindow*> apcWindowList );

 Q_SIGNALS:
  void changed();

 private Q_SLOTS:
  void slotQualityMetricChanged( int );
  void slotSelectCurrentAsReference();
  void slotPlotQualitySingle();
  void slotPlotQualitySeveral();
};

#endif  // __QUALITYMEASUREMENTSIDEBAR_H__
