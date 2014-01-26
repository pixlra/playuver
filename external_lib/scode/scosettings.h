/***************************************************************************
 *   This file is a part of SCode project                                  *
 *   IT - Instituto de Telecomunicacoes                                    *
 *   http://www.it.pt                                                      *
 *                                                                         *
 *   Copyright (C) 2008 by Ricardo N. Rocha Sardo                          *
 *   ricardo.sardo@co.it.pt                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SCODESCOSETTINGS_H
#define SCODESCOSETTINGS_H

#include <QSettings>
#include <QString>
#include <QSize>
#include <QPoint>
#include <QColor>
#include <QString>

#include <QBitArray>
#include "gridmanager.h"

namespace SCode
{

/**
 @author Ricardo N. Rocha Sardo <ricardo.sardo@co.it.pt>
 */
class SCoSettings
{

public:
  SCoSettings();

  // - - - - - - - - - - Main Window Settings - - - - - - - - - - - - - 
  QPoint mainWindowPos();
  void setMainWindowPos( const QPoint &v );

  QSize mainWindowSize();
  void setMainWindowSize( const QSize &v );

  QString lastOpenPath();
  void setLastOpenPath( const QString &v );

  // - - - - - - - - - - - - Grid Settings - - - - - - - - - - - - - - - 
  GridManager::Style gridStyle();
  void setGridStyle( GridManager::Style v );

  QColor gridColor();
  void setGridColor( const QColor &v );

  int gridHSpacing();
  void setGridHSpacing( int v );

  int gridVSpacing();
  void setGridVSpacing( int v );

  // - - - - - - - - - - - Segmentation Settings - - - - - - - - - - - -
  QBitArray results2show();
  void setResults2show( QBitArray vectorRes );

  // - - - - - - - - - - - - MMP-compound Encoder settings - - - - - - - - - - - - 
  QString binaryPath();
  void setBinaryPath( QString binaryPath );

  QString oimagePath();
  void setOimagePath( QString binaryPath );

  int blockSize();
  void setBlockSize( int blockSize );

  int quantization();
  void setQuantization( int quantization );

  int predictionLevel();
  void setPredictionLevel( int predictionlevel );

  int defineBounds();
  void setDefineBounds( int definebounds );

  int lambda();
  void setLambda( int lambda );

  int dictionary();
  void setDictionary( int dictionary );

  int radiusDistortion();
  void setRadiusDistortion( int distortion );

  int losslessDistance();
  void setLosslessDistance( int distance );

  QBitArray mmpTenCheckBoxs();
  void setMmpTenCheckBoxs( QBitArray tenCheckBoxs );

  int mmpAutoSeg();
  void setMmpAutoSeg( int autoSeg );

  // - - - - - - - - - - - - MMP-compound Decoder settings - - - - - - - - - - - - 
  QString binaryPathDec();
  void setBinaryPathDec( QString binaryPath );

  QString oimagePathDec();
  void setOimagePathDec( QString binaryPath );

  bool optOutResDec();
  void setOptOutResDec( bool par );

  QString outResDec();
  void setOutResDec( QString str );

  bool optImgYDec();
  void setOptImgYDec( bool par );

  QString imgYDec();
  void setImgYDec( QString str );

  bool optImgUDec();
  void setOptImgUDec( bool par );

  QString imgUDec();
  void setImgUDec( QString str );

  bool optImgVDec();
  void setOptImgVDec( bool par );

  QString imgVDec();
  void setImgVDec( QString str );

private:
  QSettings m_settings;
};

}  // namespace SCode

#endif  // SCODESCOSETTINGS_H
