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
 * \file     PlotSubWindow.h
 * \brief    Window to handle plots
 */

#ifndef __PLOTWINDOWHANDLE_H__
#define __PLOTWINDOWHANDLE_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "SubWindowHandle.h"
#include "qcustomplot.h"

namespace plaYUVer
{

class PlotSubWindow: public SubWindowHandle
{
Q_OBJECT

private:
  QCustomPlot* m_cPlotArea;

  Void Example();

public:
  PlotSubWindow( QWidget * parent = 0 );
  ~PlotSubWindow();

  /**
   * Virtual functions from SubWindowHandle
   */
  Void normalSize();
  Void zoomToFit();
  Void zoomToFactor( Double factor, QPoint center = QPoint() );
  Void scaleView( Double scale, QPoint center = QPoint() );

  Double getScaleFactor()
  {
    return 0;
  }

  /**
   * Size related functions
   */
//  QSize sizeHint() const;
//  QSize sizeHint( const QSize & ) const;

};

}  // NAMESPACE

#endif // __PLOTWINDOWHANDLE_H__

