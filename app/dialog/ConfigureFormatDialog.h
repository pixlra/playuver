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
 * \file     ConfigureFormatDialog.h
 * \brief    Dialog box to set the sequence resolution
 */

#ifndef __CONFIGUREFORMATDIALOG_H__
#define __CONFIGUREFORMATDIALOG_H__

#include <QDialog>
#include <QVector>

#include "CommonDefs.h"
#include "lib/CalypStream.h"

class QComboBox;
class QHBoxLayout;
class QLabel;
class QSpinBox;
class QString;
class QWidget;

typedef QVector<CalypStandardResolution> CalypStandardResolutionVector;
QDataStream& operator<<( QDataStream& out, const CalypStandardResolutionVector& array );
QDataStream& operator>>( QDataStream& in, CalypStandardResolutionVector& array );

/**
 *
 */
class ConfigureFormatDialog : public QDialog
{
  Q_OBJECT

public:
  ConfigureFormatDialog( QWidget* parent = 0 );
  int runConfigureFormatDialog( const QString& Filename, unsigned int& rWidth, unsigned int& rHeight, int& rInputFormat, unsigned int& rBits,
                                int& rEndianess, unsigned int& rFrameRate );

  void readSettings();
  void writeSettings();

private Q_SLOTS:
  void slotStandardResolutionSelected( int );
  void slotResolutionChange();
  void slotColorSpaceChange( int );
  void slotBitsChange( int );

private:
  QStringList standardResolutionNames;
  QVector<QSize> standardResolutionSizes;
  CalypStandardResolutionVector aRCustomFileFormats;

  QLabel* m_labelFilename;
  QComboBox* m_comboBoxStandardResolution;
  QSpinBox* m_spinBoxWidth;
  QSpinBox* m_spinBoxheight;
  QComboBox* m_comboBoxColorSpace;
  QComboBox* m_comboBoxPixelFormat;
  QSpinBox* m_spinBoxBits;
  QWidget* m_widgetEndianness;
  QComboBox* m_comboBoxEndianness;
  QHBoxLayout* framerateFormatLayout;
  QSpinBox* m_spinBoxFrameRate;
};

Q_DECLARE_METATYPE( CalypStandardResolution );
Q_DECLARE_METATYPE( CalypStandardResolutionVector );

#endif  // __CONFIGUREFORMATDIALOG_H__
