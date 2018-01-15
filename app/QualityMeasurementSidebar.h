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
 * \file     QualityMeasurementSidebar.h
 * \brief    Definition of the quality measurement sidebar
 */

#ifndef __QUALITYMEASUREMENTSIDEBAR_H__
#define __QUALITYMEASUREMENTSIDEBAR_H__

#include "HistogramWidget.h"
#include "PlaYUVerAppDefs.h"
#include "config.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerStream.h"

#include <QtWidgets>

class PlaYUVerSubWindowHandle;
class VideoSubWindow;

class QualityMeasurementSidebar : public QWidget
{
  Q_OBJECT
public:
  QualityMeasurementSidebar( QWidget*, PlaYUVerSubWindowHandle* );
  ~QualityMeasurementSidebar();

  QSize sizeHint() const;

  Void updateSideBar( Bool hasSubWindow );

  Void updateSubWindowList();
  Void updateCurrentWindow( VideoSubWindow* );
  Void updateSidebarData();
  Void updateQualityMetric( Int );

private:
  PlaYUVerSubWindowHandle* m_pcMainWindowManager;

  QVector<VideoSubWindow*> m_pcVideoWindowList;
  QStringList m_pcWindowListNames;
  QStringList m_pcSelectedWindowListNames;

  VideoSubWindow* m_pcCurrentVideoSubWindow;

  QComboBox* m_comboBoxMetric;

  QComboBox* m_comboBoxRef;
  QComboBox* m_comboBoxRec;

  QLabel* m_ppcLabelQualityLabel[3];
  QLabel* m_ppcLabelQualityValue[3];

  HistogramWidget* histogramWidget;

Q_SIGNALS:
  void signalQualityMetricChanged( int );

private Q_SLOTS:
  void slotReferenceChanged( int );
  void slotQualityMetricChanged( int );
};

#endif  // __QUALITYMEASUREMENTSIDEBAR_H__
