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
 * \file     PlotWindowHandle.cpp
 * \brief    Window to handle plots
 */

#include "PlotSubWindow.h"

namespace plaYUVer
{

Void PlotSubWindow::definePlotColors()
{
  m_arrayColorList.append( Qt::blue );
  m_arrayColorList.append( Qt::red );
  m_arrayColorList.append( Qt::green );
  m_arrayColorList.append( Qt::black );
  m_arrayColorList.append( Qt::cyan );
}

PlotSubWindow::PlotSubWindow( QWidget * parent ) :
        SubWindowHandle( parent, SubWindowHandle::PLOT_SUBWINDOW )
{
  setParent( parent );

  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Light );
  setVisible( false );

  m_cPlotArea = new QCustomPlot( this );
  setWidget( m_cPlotArea );

  // configure right and top axis to show ticks but no labels:
  // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
  m_cPlotArea->xAxis2->setVisible( true );
  m_cPlotArea->xAxis2->setTickLabels( false );
  m_cPlotArea->yAxis2->setVisible( true );
  m_cPlotArea->yAxis2->setTickLabels( false );
  // make left and bottom axes always transfer their ranges to right and top axes:
  connect( m_cPlotArea->xAxis, SIGNAL( rangeChanged(QCPRange) ), m_cPlotArea->xAxis2, SLOT( setRange(QCPRange) ) );
  connect( m_cPlotArea->yAxis, SIGNAL( rangeChanged(QCPRange) ), m_cPlotArea->yAxis2, SLOT( setRange(QCPRange) ) );

  definePlotColors();
  m_uiNumberPlots = 0;

  m_cPlotArea->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables );

}

Void PlotSubWindow::Example()
{
  // generate some points of data (y0 for first, y1 for second graph):
  QVector<double> x( 250 ), y0( 250 ), y1( 250 );
  for( int i = 0; i < 250; ++i )
  {
    x[i] = i;
    y0[i] = exp( -i / 150.0 ) * cos( i / 10.0 );  // exponentially decaying cosine
    y1[i] = exp( -i / 150.0 );  // exponential envelope
  }

  addPlot( x, y0 );
  addPlot( x, y1 );

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

QSize PlotSubWindow::sizeHint() const
{
  QSize maxSize;
  QWidget *p = parentWidget();
  if( p )
  {
    maxSize = p->size();
  }
  else
  {
    maxSize = QApplication::desktop()->availableGeometry().size();
  }
  return sizeHint( maxSize );
}

QSize PlotSubWindow::sizeHint( const QSize & maxSize ) const
{
  return maxSize;
}

Void PlotSubWindow::addPlot( const QVector<Double> &key, const QVector<Double> &value )
{
  QCPGraph *newPlot = m_cPlotArea->addGraph();
  newPlot->setPen( QPen( m_arrayColorList.at( m_uiNumberPlots ) ) );  // line color blue for first graph
  newPlot->setBrush( QBrush( QColor( 0, 0, 255, 20 ) ) );  // first graph will be filled with translucent blue
  // pass data points to graphs:
  newPlot->setData( key, value );
  // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
  newPlot->rescaleAxes( m_uiNumberPlots > 1 ? true : false );

  m_uiNumberPlots++;
}

}  // NAMESPACE
