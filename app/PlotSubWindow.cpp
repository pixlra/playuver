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
 * \file     PlotWindowHandle.cpp
 * \brief    Window to handle plots
 */

#include "PlotSubWindow.h"
#include "qcustomplot.h"

Void PlotSubWindow::definePlotColors()
{
  m_arrayColorList.append( Qt::blue );
  m_arrayColorList.append( Qt::red );
  m_arrayColorList.append( Qt::green );
  m_arrayColorList.append( Qt::black );
  m_arrayColorList.append( Qt::cyan );
  m_arrayColorList.append( Qt::magenta );
  m_arrayColorList.append( Qt::yellow );
  m_arrayColorList.append( Qt::gray );
  m_arrayColorList.append( Qt::darkRed );
  m_arrayColorList.append( Qt::darkGreen );
  m_arrayColorList.append( Qt::darkRed );
  m_arrayColorList.append( Qt::darkBlue );
  m_arrayColorList.append( Qt::darkMagenta );
  m_arrayColorList.append( Qt::darkCyan );
  m_arrayColorList.append( Qt::darkYellow );
}

PlotSubWindow::PlotSubWindow( const QString& windowTitle, QWidget* parent ) : SubWindowAbstract( parent, SubWindowAbstract::PLOT_SUBWINDOW )
{
  definePlotColors();
  m_iNumberPlots = 0;
  m_dScaleFactor = 1;

  setWindowName( windowTitle );

  m_cPlotArea = new QCustomPlot( this );

  QSizePolicy sizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );
  sizePolicy.setHeightForWidth( m_cPlotArea->sizePolicy().hasHeightForWidth() );
  m_cPlotArea->setSizePolicy( sizePolicy );

  m_cPlotArea->setBackgroundRole( backgroundRole() );

  QBrush backgroundBrush = palette().brush( backgroundRole() );
  QBrush foregroundBrush = palette().brush( foregroundRole() );

  m_cPlotArea->setBackground( backgroundBrush );

  m_cPlotArea->xAxis->setLabelColor( palette().text().color() );
  m_cPlotArea->yAxis->setLabelColor( palette().text().color() );
  m_cPlotArea->xAxis->setTickLabelColor( palette().text().color() );
  m_cPlotArea->yAxis->setTickLabelColor( palette().text().color() );
  m_cPlotArea->xAxis->setBasePen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->yAxis->setBasePen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->xAxis->setTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->yAxis->setTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->xAxis->setSubTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->yAxis->setSubTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->xAxis2->setSubTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->yAxis2->setSubTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->xAxis2->setBasePen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->yAxis2->setBasePen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->xAxis2->setTickPen( QPen( foregroundBrush, 1 ) );
  m_cPlotArea->yAxis2->setTickPen( QPen( foregroundBrush, 1 ) );

  // configure right and top axis to show ticks but no labels:
  // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
  m_cPlotArea->axisRect( 0 )->setupFullAxesBox( true );
  m_cPlotArea->xAxis2->setVisible( true );
  m_cPlotArea->xAxis2->setTickLabels( false );
  m_cPlotArea->yAxis2->setVisible( true );
  m_cPlotArea->yAxis2->setTickLabels( false );
  // make left and bottom axes always transfer their ranges to right and top
  // axes:
  connect( m_cPlotArea->xAxis, SIGNAL( rangeChanged( QCPRange ) ), m_cPlotArea->xAxis2, SLOT( setRange( QCPRange ) ) );
  connect( m_cPlotArea->yAxis, SIGNAL( rangeChanged( QCPRange ) ), m_cPlotArea->yAxis2, SLOT( setRange( QCPRange ) ) );

  QFont legendFont = font();     // start out with MainWindow's font..
  legendFont.setPointSize( 9 );  // and make a bit smaller for legend
  m_cPlotArea->legend->setFont( legendFont );
  m_cPlotArea->legend->setBrush( backgroundBrush );
  m_cPlotArea->legend->setTextColor( palette().text().color() );
  m_cPlotArea->legend->setBorderPen( QPen( palette().brush( QPalette::Active, QPalette::Shadow ), 1 ) );
  m_cPlotArea->legend->setVisible( false );

  m_cPlotArea->setInteractions( QCP::iRangeZoom | QCP::iRangeDrag );  // QCP::iSelectPlottables

  m_cPlotPen.setStyle( Qt::SolidLine );
  m_cPlotPen.setColor( Qt::black );
  m_cPlotPen.setWidthF( 2 );

  setWidget( m_cPlotArea );
}

PlotSubWindow::~PlotSubWindow()
{
  delete m_cPlotArea;
}

Void PlotSubWindow::normalSize()
{
  m_cPlotArea->xAxis->setRange( m_aAxisRange[HORIZONTAL][0], m_aAxisRange[HORIZONTAL][1] );
  m_cPlotArea->yAxis->setRange( m_aAxisRange[VERTICAL][0], m_aAxisRange[VERTICAL][1] );
  m_cPlotArea->replot();
  m_dScaleFactor = 1;
}

Void PlotSubWindow::zoomToFit()
{
  normalSize();
}

Void PlotSubWindow::zoomToFactor( Double factor, QPoint center ) {}

Void PlotSubWindow::scaleView( Double scale, QPoint center )
{
  scale -= 1;
  scale /= 10;
  scale += 1;

  m_dScaleFactor *= scale;
  //
  //  UInt aNewAxisRange[2][2];
  //  for( UInt axis = 0; axis < 2; axis++ )
  //    for( UInt dim = 0; dim < 2; dim++ )
  //      aNewAxisRange[axis][dim] = m_aAxisRange[axis][dim] / m_dScaleFactor;
  //
  //  m_cPlotArea->xAxis->setRange( aNewAxisRange[HORIZONTAL][0],
  //  aNewAxisRange[HORIZONTAL][1] );
  //  m_cPlotArea->yAxis->setRange( aNewAxisRange[VERTICAL][0],
  //  aNewAxisRange[VERTICAL][1] );

  m_cPlotArea->xAxis->scaleRange( scale, 0 );
  m_cPlotArea->yAxis->scaleRange( scale, 0 );
  m_cPlotArea->replot();
}

QSize PlotSubWindow::sizeHint() const
{
  QSize maxSize;
  QWidget* p = parentWidget();
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

QSize PlotSubWindow::sizeHint( const QSize& maxSize ) const
{
  return maxSize * 2 / 3;
}

Void PlotSubWindow::setAxisName( const QString& nameAxisX, const QString& nameAxisY )
{
  m_cPlotArea->xAxis->setLabel( nameAxisX );
  m_cPlotArea->yAxis->setLabel( nameAxisY );
}

Void PlotSubWindow::setAxisRange( const QLine& axisLimits )
{
  setAxisRange( HORIZONTAL, axisLimits.x1(), axisLimits.x2() );
  setAxisRange( VERTICAL, axisLimits.y1(), axisLimits.y2() );
}

Void PlotSubWindow::setAxisRange( PlotSubWindow::Axis eAxis, const Int& axisStart, const Int& axisEnd )
{
  if( eAxis == HORIZONTAL )
  {
    m_cPlotArea->xAxis->setRange( axisStart, axisEnd );
    // horizontalScrollBar()->setRange( axisStart * 100, axisEnd * 100 );
    m_aAxisRange[HORIZONTAL][0] = axisStart;
    m_aAxisRange[HORIZONTAL][1] = axisEnd;
  }
  if( eAxis == VERTICAL )
  {
    m_cPlotArea->yAxis->setRange( axisStart, axisEnd );
    // verticalScrollBar()->setRange( axisStart * 100, axisEnd * 100 );
    m_aAxisRange[VERTICAL][0] = axisStart;
    m_aAxisRange[VERTICAL][1] = axisEnd;
  }
  m_cPlotArea->replot();
}

Void PlotSubWindow::addPlot( const QVector<Double>& arrayX, const QVector<Double>& arrayY, const QString& key )
{
  QCPGraph* newPlot = m_cPlotArea->addGraph();
  QColor plotColor = m_arrayColorList.at( 0 );
  if( m_iNumberPlots < m_arrayColorList.size() )
  {
    plotColor = m_arrayColorList.at( m_iNumberPlots );
  }
  m_cPlotPen.setColor( plotColor );
  newPlot->setPen( m_cPlotPen );  // line style
  // pass data points to graphs:
  newPlot->setData( arrayX, arrayY );
  // let the ranges scale themselves so graph 0 fits perfectly in the visible
  // area:
  newPlot->rescaleAxes( m_iNumberPlots > 1 ? true : false );

  if( !key.isEmpty() )
  {
    m_cPlotArea->legend->setVisible( true );
    newPlot->setName( key );
  }

  m_aAxisRange[HORIZONTAL][0] = m_cPlotArea->xAxis->range().lower;
  m_aAxisRange[HORIZONTAL][1] = m_cPlotArea->xAxis->range().upper;
  m_aAxisRange[VERTICAL][0] = m_cPlotArea->yAxis->range().lower;
  m_aAxisRange[VERTICAL][1] = m_cPlotArea->yAxis->range().upper;

  m_iNumberPlots++;
}
