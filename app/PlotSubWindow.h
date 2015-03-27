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
 * \file     PlotSubWindow.h
 * \brief    Window to handle plots
 */

#ifndef __PLOTWINDOWHANDLE_H__
#define __PLOTWINDOWHANDLE_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "SubWindowHandle.h"

class QCustomPlot;

namespace plaYUVer
{

class PlotSubWindow: public SubWindowHandle
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
  Double m_aAxisRange[2][2];
  Double m_dScaleFactor;
  Int m_iNumberPlots;

public:

  PlotSubWindow( QWidget * parent, const QString& windowTitle );
  ~PlotSubWindow();

  Void definePlotColors();

  Void refreshSubWindow()
  {

  }

  Void setTool( UInt uiTool )
  {
  }

  /**
   * Virtual functions from SubWindowHandle
   */
  Void normalSize();
  Void zoomToFit();
  Void zoomToFactor( Double factor, QPoint center = QPoint() );
  Void scaleView( Double scale, QPoint center = QPoint() );

  Double getScaleFactor()
  {
    return m_dScaleFactor;
  }

  /**
   * Size related functions
   */
  QSize sizeHint() const;
  QSize sizeHint( const QSize & ) const;

  Void setAxisName( const QString& nameAxisX, const QString& nameAxisY );

  Void setAxisRange( const QLine& axisLimits );
  Void setAxisRange( PlotSubWindow::Axis eAxis, const Int& axisStart, const Int& axisEnd );

  Void setKey( const QString& key );

  Void addPlot( const QVector<Double> &arrayX, const QVector<Double> &arrayY, const QString& key = QString() );

};

}  // NAMESPACE

#endif // __PLOTWINDOWHANDLE_H__

