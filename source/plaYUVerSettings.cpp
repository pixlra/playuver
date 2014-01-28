/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
 * \file     playuversettings.h
 * \brief    plaYUVer app settings
 */

#include <QDir>

#include "plaYUVerSettings.h"

namespace plaYUVer
{

PlaYUVerSettings::PlaYUVerSettings() :
    m_settings( "IT", "YUV Player" )
{
}
// - - - - - - - - - - - -  Main Window Settings - - - - - - - - - - - - - - - - 

QPoint PlaYUVerSettings::mainWindowPos()
{
  return m_settings.value( "MainWindow/pos", QPoint( 200, 200 ) ).toPoint();
}

void PlaYUVerSettings::setMainWindowPos( const QPoint & pos )
{
  m_settings.setValue( "MainWindow/pos", pos );
}

QSize PlaYUVerSettings::mainWindowSize()
{
  return m_settings.value( "MainWindow/size", QSize( 500, 400 ) ).toSize();
}

void PlaYUVerSettings::setMainWindowSize( const QSize & size )
{
  m_settings.setValue( "MainWindow/size", size );
}

QString PlaYUVerSettings::lastOpenPath()
{
  return m_settings.value( "MainWindow/lastOpenPath", QDir::homePath() ).toString();
}

void PlaYUVerSettings::setLastOpenPath( const QString & path )
{
  m_settings.setValue( "MainWindow/lastOpenPath", path );
}

// - - - - - - - - - - - - - Segmentation Settings - - - - - - - - - - - - - - 

QBitArray PlaYUVerSettings::results2show()
{
  QBitArray vectorRes( 20, false );
  vectorRes.setBit( 16, true );
  vectorRes.setBit( 17, true );
  return m_settings.value( "Segmentation/vectorResults", vectorRes ).toBitArray();
}

void PlaYUVerSettings::setResults2show( QBitArray vectorRes )
{
  m_settings.setValue( "Segmentation/vectorResults", vectorRes );
}

// - - - - - - - - - - - - - MMP-compound Encoder settings - - - - - - - - - - - - - - -

QString PlaYUVerSettings::binaryPath()
{
  return m_settings.value( "MMP/binaryPath", QDir::currentPath() ).toString();
}

void PlaYUVerSettings::setBinaryPath( QString binaryPath )
{
  m_settings.setValue( "MMP/binaryPath", binaryPath );
}

QString PlaYUVerSettings::oimagePath()
{
  return m_settings.value( "MMP/oimagePath", QDir::currentPath() ).toString();
}

void PlaYUVerSettings::setOimagePath( QString oimagePath )
{
  m_settings.setValue( "MMP/oimagePath", oimagePath );
}

int PlaYUVerSettings::blockSize()
{
  return m_settings.value( "MMP/blockSize", 8 ).toInt();
}

void PlaYUVerSettings::setBlockSize( int blockSize )
{
  m_settings.setValue( "MMP/blockSize", blockSize );
}

int PlaYUVerSettings::quantization()
{
  return m_settings.value( "MMP/quantization", 4 ).toInt();
}

void PlaYUVerSettings::setQuantization( int quantization )
{
  m_settings.setValue( "MMP/quantization", quantization );
}

int PlaYUVerSettings::predictionLevel()
{
  return m_settings.value( "MMP/predictionLevel", 0 ).toInt();
}

void PlaYUVerSettings::setPredictionLevel( int predictionlevel )
{
  m_settings.setValue( "MMP/predictionLevel", predictionlevel );
}

int PlaYUVerSettings::defineBounds()
{
  return m_settings.value( "MMP/defineBounds", 1 ).toInt();
}

void PlaYUVerSettings::setDefineBounds( int definebounds )
{
  m_settings.setValue( "MMP/defineBounds", definebounds );
}

int PlaYUVerSettings::lambda()
{
  return m_settings.value( "MMP/lambda", 1000 ).toInt();
}

void PlaYUVerSettings::setLambda( int lambda )
{
  m_settings.setValue( "MMP/lambda", lambda );
}

int PlaYUVerSettings::dictionary()
{
  return m_settings.value( "MMP/dictionary", 5000 ).toInt();
}

void PlaYUVerSettings::setDictionary( int dictionary )
{
  m_settings.setValue( "MMP/dictionary", dictionary );
}

int PlaYUVerSettings::radiusDistortion()
{
  return m_settings.value( "MMP/radiusDistortion", 100 ).toInt();
}

void PlaYUVerSettings::setRadiusDistortion( int distortion )
{
  m_settings.setValue( "MMP/radiusDistortion", distortion );
}

int PlaYUVerSettings::losslessDistance()
{
  return m_settings.value( "MMP/losslessDistance", 0 ).toInt();
}

void PlaYUVerSettings::setLosslessDistance( int distance )
{
  m_settings.setValue( "MMP/losslessDistance", distance );
}

int PlaYUVerSettings::mmpAutoSeg()
{
  return m_settings.value( "MMP/autoSeg", 1 ).toInt();
}

void PlaYUVerSettings::setMmpAutoSeg( int autoSeg )
{
  m_settings.setValue( "MMP/autoSeg", autoSeg );
}

/*
 bool PlaYUVerSettings::fastMode()
 {
 return m_settings.value("MMP/fastMode", 1).toBool();
 }

 void PlaYUVerSettings::setFastMode( bool fastmode )
 {
 m_settings.setValue("MMP/fastMode" , fastmode);
 }
 */

QBitArray PlaYUVerSettings::mmpTenCheckBoxs()
{
  QBitArray bitArray( 10, false );
  return m_settings.value( "MMP/tenCheckBoxs", bitArray ).toBitArray();
}

void PlaYUVerSettings::setMmpTenCheckBoxs( QBitArray tenCheckBoxs )
{
  m_settings.setValue( "MMP/tenCheckBoxs", tenCheckBoxs );
}

// - - - - - - - - - - - - - MMP-compound Decoder settings - - - - - - - - - - - - - - -

QString PlaYUVerSettings::binaryPathDec()
{
  return m_settings.value( "MMPdec/binaryPath", QDir::currentPath() ).toString();
}

void PlaYUVerSettings::setBinaryPathDec( QString binaryPath )
{
  m_settings.setValue( "MMPdec/binaryPath", binaryPath );
}

QString PlaYUVerSettings::oimagePathDec()
{
  return m_settings.value( "MMPdec/oimagePath", QDir::currentPath() ).toString();
}

void PlaYUVerSettings::setOimagePathDec( QString oimagePath )
{
  m_settings.setValue( "MMPdec/oimagePath", oimagePath );
}

bool PlaYUVerSettings::optOutResDec()
{
  return m_settings.value( "MMPdec/optOutRes", 0 ).toBool();
}

void PlaYUVerSettings::setOptOutResDec( bool par )
{
  m_settings.setValue( "MMPdec/optOutRes", par );
}

QString PlaYUVerSettings::outResDec()
{
  return m_settings.value( "MMPdec/outRes", "" ).toString();
}

void PlaYUVerSettings::setOutResDec( QString str )
{
  m_settings.setValue( "MMPdec/outRes", str );
}

bool PlaYUVerSettings::optImgYDec()
{
  return m_settings.value( "MMPdec/optImgY", 0 ).toBool();
}

void PlaYUVerSettings::setOptImgYDec( bool par )
{
  m_settings.setValue( "MMPdec/optImgY", par );
}

QString PlaYUVerSettings::imgYDec()
{
  return m_settings.value( "MMPdec/imgY", "" ).toString();
}

void PlaYUVerSettings::setImgYDec( QString str )
{
  m_settings.setValue( "MMPdec/imgY", str );
}

bool PlaYUVerSettings::optImgUDec()
{
  return m_settings.value( "MMPdec/optImgU", 0 ).toBool();
}

void PlaYUVerSettings::setOptImgUDec( bool par )
{
  m_settings.setValue( "MMPdec/optImgU", par );
}

QString PlaYUVerSettings::imgUDec()
{
  return m_settings.value( "MMPdec/imgU", "" ).toString();
}

void PlaYUVerSettings::setImgUDec( QString str )
{
  m_settings.setValue( "MMPdec/imgU", str );
}

bool PlaYUVerSettings::optImgVDec()
{
  return m_settings.value( "MMPdec/optImgV", 0 ).toBool();
}

void PlaYUVerSettings::setOptImgVDec( bool par )
{
  m_settings.setValue( "MMPdec/optImgV", par );
}

QString PlaYUVerSettings::imgVDec()
{
  return m_settings.value( "MMPdec/imgV", "" ).toString();
}

void PlaYUVerSettings::setImgVDec( QString str )
{
  m_settings.setValue( "MMPdec/imgV", str );
}

} // NAMESPACE
