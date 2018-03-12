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
 * \file     QualityHandle.h
 * \brief    Definition of the quality measurement sidebar
 */

#ifndef __QUALITYMEASUREMENT_H__
#define __QUALITYMEASUREMENT_H__

#include <QtCore>
#include <QtWidgets>
#include "CommonDefs.h"
#include "QFuture"
#include "QualityMeasurementSidebar.h"
#include "config.h"

class SubWindowHandle;
class VideoSubWindow;

class QualityHandle : public QWidget
{
  Q_OBJECT
public:
  QualityHandle( QWidget*, SubWindowHandle* );
  ~QualityHandle();

  void createActions();
  QMenu* createMenu();
  QDockWidget* createDock();
  void updateMenus();

  void readSettings();
  void writeSettings();

  void update( VideoSubWindow* currSubWindow );

private:
  QWidget* m_pcParet;
  SubWindowHandle* m_pcMainWindowManager;

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
  int m_iQualityMetricIdx;

  QMenu* m_pcMenuQuality;
  QMenu* m_pcSubMenuQualityMetrics;

  QDockWidget* m_pcQualityHandleDock;
  QualityMeasurementSidebar* m_pcQualityHandleSideBar;

  QFuture<void> m_cMeasurementResult;
  void measureQuality( QVector<VideoSubWindow*> apcWindowList );

Q_SIGNALS:
  void changed();

private Q_SLOTS:
  void slotQualityMetricChanged( int );
  void slotSelectCurrentAsReference();
  void slotPlotQualitySingle();
  void slotPlotQualitySeveral();
};

#endif  // __QUALITYMEASUREMENTSIDEBAR_H__
