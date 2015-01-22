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
 * \file     PlotWindowHandle.cpp
 * \brief    Window to handle plots
 */

#include "PlotSubWindow.h"

namespace plaYUVer
{

PlotSubWindow::PlotSubWindow( QWidget * parent ) :
        SubWindowHandle( parent, SubWindowHandle::PLOT_SUBWINDOW )
{
  setParent( parent );

  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Light );
  setVisible( false );

  m_cPlotArea = new QCustomPlot( this );
  setWidget( m_cPlotArea );

  Example();

}

Void PlotSubWindow::Example()
{
  // add two new graphs and set their look:
  m_cPlotArea->addGraph();
  m_cPlotArea->graph( 0 )->setPen( QPen( Qt::blue ) );  // line color blue for first graph
  m_cPlotArea->graph( 0 )->setBrush( QBrush( QColor( 0, 0, 255, 20 ) ) );  // first graph will be filled with translucent blue
  m_cPlotArea->addGraph();
  m_cPlotArea->graph( 1 )->setPen( QPen( Qt::red ) );  // line color red for second graph
  // generate some points of data (y0 for first, y1 for second graph):
  QVector<double> x( 250 ), y0( 250 ), y1( 250 );
  for( int i = 0; i < 250; ++i )
  {
    x[i] = i;
    y0[i] = exp( -i / 150.0 ) * cos( i / 10.0 );  // exponentially decaying cosine
    y1[i] = exp( -i / 150.0 );  // exponential envelope
  }
  // configure right and top axis to show ticks but no labels:
  // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
  m_cPlotArea->xAxis2->setVisible( true );
  m_cPlotArea->xAxis2->setTickLabels( false );
  m_cPlotArea->yAxis2->setVisible( true );
  m_cPlotArea->yAxis2->setTickLabels( false );
  // make left and bottom axes always transfer their ranges to right and top axes:
  connect( m_cPlotArea->xAxis, SIGNAL( rangeChanged(QCPRange) ), m_cPlotArea->xAxis2, SLOT( setRange(QCPRange) ) );
  connect( m_cPlotArea->yAxis, SIGNAL( rangeChanged(QCPRange) ), m_cPlotArea->yAxis2, SLOT( setRange(QCPRange) ) );
  // pass data points to graphs:
  m_cPlotArea->graph( 0 )->setData( x, y0 );
  m_cPlotArea->graph( 1 )->setData( x, y1 );
  // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
  m_cPlotArea->graph( 0 )->rescaleAxes();
  // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
  m_cPlotArea->graph( 1 )->rescaleAxes( true );
  // Note: we could have also just called m_cPlotArea->rescaleAxes(); instead
  // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
  m_cPlotArea->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables );

}

PlotSubWindow::~PlotSubWindow()
{
  delete m_cPlotArea;
}

Void PlotSubWindow::normalSize()
{

}

Void PlotSubWindow::zoomToFit()
{

}

Void PlotSubWindow::zoomToFactor( Double factor, QPoint center )
{

}

Void PlotSubWindow::scaleView( Double scale, QPoint center )
{

}

//QSize PlotSubWindow::sizeHint() const
//{
//
//}

}  // NAMESPACE
