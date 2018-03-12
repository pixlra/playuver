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
 * \file     PlotSubWindow.h
 * \brief    Window to handle plots
 */

#ifndef __PLOTWINDOWHANDLE_H__
#define __PLOTWINDOWHANDLE_H__

#include "CommonDefs.h"
#include "SubWindowAbstract.h"
#include "config.h"

#include <QtWidgets>

class QCustomPlot;

class PlotSubWindow : public SubWindowAbstract
{
  Q_OBJECT

private:
  QCustomPlot* m_cPlotArea;

  QVector<QColor> m_arrayColorList;
  QPen m_cPlotPen;

  enum Axis
  {
    HORIZONTAL,
    VERTICAL,
  };
  double m_aAxisRange[2][2];
  double m_dScaleFactor;
  int m_iNumberPlots;

public:
  PlotSubWindow( const QString& windowTitle, QWidget* parent = NULL );
  ~PlotSubWindow();

  void definePlotColors();

  void refreshSubWindow() {}
  /**
   * Virtual functions from SubWindowAbstract
   */
  void normalSize();
  void zoomToFit();
  void zoomToFactor( double factor, QPoint center = QPoint() );
  void scaleView( double scale, QPoint center = QPoint() );

  double getScaleFactor() { return m_dScaleFactor; }
  /**
   * Size related functions
   */
  QSize sizeHint() const;
  QSize sizeHint( const QSize& ) const;

  void setAxisName( const QString& nameAxisX, const QString& nameAxisY );

  void setAxisRange( const QLine& axisLimits );
  void setAxisRange( PlotSubWindow::Axis eAxis, const int& axisStart, const int& axisEnd );

  void setKey( const QString& key );

  void addPlot( const QVector<double>& arrayX, const QVector<double>& arrayY, const QString& key = QString() );
};

#endif  // __PLOTWINDOWHANDLE_H__
