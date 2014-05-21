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
 * \file     ConfigureFormatDialog.h
 * \brief    Dialog box to set the sequence resolution
 */

#ifndef __CONFIGUREFORMATDIALOG_H__
#define __CONFIGUREFORMATDIALOG_H__

#include "config.h"
#include "PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QtCore>



class QPixmap;
class QColor;

namespace plaYUVer
{

#define ADD_STANDARD_RESOLUTION( name, width, height) \
    standardResolutionNames.push_back( QString( name ) ); \
    standardResolutionSizesList.push_back( QSize( width, height ) )

/**
 *
 */
class ConfigureFormatDialog: public QDialog
{
Q_OBJECT

public:

  ConfigureFormatDialog( QWidget *parent = 0 );
  Int runConfigureFormatDialog ( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );

private Q_SLOTS:
  void StandardResolutionSelection();

private:
  QStringList standardResolutionNames;
  QList<QSize> standardResolutionSizesList;

  QComboBox *m_comboBoxStandardResolution;
  QSpinBox *m_spinBoxWidth;
  QSpinBox *m_spinBoxheight;
  QComboBox *m_comboBoxPixelFormat;
  QSpinBox *m_spinBoxFrameRate;

  Void setStandardResolutionSizes()
  {
    ADD_STANDARD_RESOLUTION( "CIF (352x288)", 352, 288 );
    ADD_STANDARD_RESOLUTION( "VGA (640x480)", 640, 480 );
    ADD_STANDARD_RESOLUTION( "XVGA (1024x768)", 1024, 768 );
    ADD_STANDARD_RESOLUTION( "HD (1280x720)", 1280, 720 );
    ADD_STANDARD_RESOLUTION( "Full HD (1920x1080)", 1920, 1080 );
  }

};

}  // NAMESPACE

#endif // __CONFIGUREFORMATDIALOG_H__
