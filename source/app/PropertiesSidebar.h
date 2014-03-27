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

class QRect;

namespace plaYUVer
{

class PropertiesSidebarPrivate;

class PropertiesSidebar: public QWidget
{
  Q_OBJECT

public:

  PropertiesSidebar( QWidget* parent );
  ~PropertiesSidebar();

  //  void setData( SImage &img, const QRect &selectionArea = QRect() );   (Bug) Correction:
      void setData( InputStream* m_pcStream  );


  public slots:


  private:

      void updateInformations();
      void updateStatistiques();
      void setStopButton();
      void setOkButton();


  private slots:
    void slotMinValueChanged( int min );
    void  slotMaxValueChanged( int max );
      void slotUpdateInterval( int min, int max );
      void slotUpdateIntervRange( int range );

  private:

      enum ColorChannel
      {
          LuminosityChannel=0,
          RedChannel,
          GreenChannel,
          BlueChannel,
          AlphaChannel,
          ColorChannels
      };

      enum AllColorsColorType
      {
          AllColorsRed=0,
          AllColorsGreen,
          AllColorsBlue
      };

  PropertiesSidebarPrivate* d;
};

}   // NAMESPACE

#endif
