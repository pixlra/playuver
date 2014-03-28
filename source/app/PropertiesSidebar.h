/*    This file is a part of plaYUVerApp project
 *    Copyright (C) 2014  by plaYUVerApp developers
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
 * \file     PropertiesSidebar.h
 * \brief    Main side bar definition
 */

#ifndef PROPERTIESSIDEBAR_H
#define PROPERTIESSIDEBAR_H

#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#include <QWidget>
#endif
#include "InputStream.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class StreamPropertiesSideBar: public QWidget
{
Q_OBJECT
public:
  StreamPropertiesSideBar( QWidget* parent );
  ~StreamPropertiesSideBar();

  QSize sizeHint() const;

  Void setData( InputStream* m_pcStream );

private:
  InputStream* m_pcStream;

  QLabel* labelFormatValue;
  QLabel* labelSizeValue;
  QLabel* labelColorSpaceValue;
};

class FramePropertiesSideBar: public QWidget
{
Q_OBJECT
public:
  FramePropertiesSideBar( QWidget* parent );
  ~FramePropertiesSideBar();

  QSize sizeHint() const;

  Void setData( PlaYUVerFrame* m_pcStream );

private:
  PlaYUVerFrame *m_pcFrame;

  QLabel *labelMeanValue;
  QLabel *labelPixelsValue;
  QLabel *labelStdDevValue;
  QLabel *labelCountValue;
  QLabel *labelMedianValue;
  QLabel *labelPercentileValue;
};

}   // NAMESPACE

#endif
