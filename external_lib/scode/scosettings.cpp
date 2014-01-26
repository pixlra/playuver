/***************************************************************************
 *   This file is a part of SCode project               		   *
 *   IT - Instituto de Telecomunicacoes   	  	                   *
 *   http://www.it.pt				                           *
 *									   *
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
#include <QDir>

#include "scosettings.h"

namespace SCode
{

SCoSettings::SCoSettings() :
    m_settings( "IT", "SCoDE" )
{
}
// - - - - - - - - - - - -  Main Window Settings - - - - - - - - - - - - - - - - 

QPoint SCoSettings::mainWindowPos()
{
  return m_settings.value( "MainWindow/pos", QPoint( 200, 200 ) ).toPoint();
}

void SCoSettings::setMainWindowPos( const QPoint & pos )
{
  m_settings.setValue( "MainWindow/pos", pos );
}

QSize SCoSettings::mainWindowSize()
{
  return m_settings.value( "MainWindow/size", QSize( 500, 400 ) ).toSize();
}

void SCoSettings::setMainWindowSize( const QSize & size )
{
  m_settings.setValue( "MainWindow/size", size );
}

QString SCoSettings::lastOpenPath()
{
  return m_settings.value( "MainWindow/lastOpenPath", QDir::homePath() ).toString();
}

void SCoSettings::setLastOpenPath( const QString & path )
{
  m_settings.setValue( "MainWindow/lastOpenPath", path );
}

// - - - - - - - - - - - - - - Grid Settings - - - - - - - - - - - - - - - - - - 

GridManager::Style SCoSettings::gridStyle()
{
  return ( GridManager::Style )m_settings.value( "Grid/style", 0 ).toInt();
}

void SCoSettings::setGridStyle( GridManager::Style v )
{
  m_settings.setValue( "Grid/style", v );
}

QColor SCoSettings::gridColor()
{
  return m_settings.value( "Grid/color", QColor( Qt::white ) ).value<QColor>();
}

void SCoSettings::setGridColor( const QColor & v )
{
  m_settings.setValue( "Grid/color", v );
}

int SCoSettings::gridHSpacing()
{
  return m_settings.value( "Grid/hSpacing", 16 ).toInt();
}

void SCoSettings::setGridHSpacing( int v )
{
  m_settings.setValue( "Grid/hSpacing", v );
}

int SCoSettings::gridVSpacing()
{
  return m_settings.value( "Grid/vSpacing", 16 ).toInt();
}

void SCoSettings::setGridVSpacing( int v )
{
  m_settings.setValue( "Grid/vSpacing", v );
}

// - - - - - - - - - - - - - Segmentation Settings - - - - - - - - - - - - - - 

QBitArray SCoSettings::results2show()
{
  QBitArray vectorRes( 20, false );
  vectorRes.setBit( 16, true );
  vectorRes.setBit( 17, true );
  return m_settings.value( "Segmentation/vectorResults", vectorRes ).toBitArray();
}

void SCoSettings::setResults2show( QBitArray vectorRes )
{
  m_settings.setValue( "Segmentation/vectorResults", vectorRes );
}

// - - - - - - - - - - - - - MMP-compound Encoder settings - - - - - - - - - - - - - - -

QString SCoSettings::binaryPath()
{
  return m_settings.value( "MMP/binaryPath", QDir::currentPath() ).toString();
}

void SCoSettings::setBinaryPath( QString binaryPath )
{
  m_settings.setValue( "MMP/binaryPath", binaryPath );
}

QString SCoSettings::oimagePath()
{
  return m_settings.value( "MMP/oimagePath", QDir::currentPath() ).toString();
}

void SCoSettings::setOimagePath( QString oimagePath )
{
  m_settings.setValue( "MMP/oimagePath", oimagePath );
}

int SCoSettings::blockSize()
{
  return m_settings.value( "MMP/blockSize", 8 ).toInt();
}

void SCoSettings::setBlockSize( int blockSize )
{
  m_settings.setValue( "MMP/blockSize", blockSize );
}

int SCoSettings::quantization()
{
  return m_settings.value( "MMP/quantization", 4 ).toInt();
}

void SCoSettings::setQuantization( int quantization )
{
  m_settings.setValue( "MMP/quantization", quantization );
}

int SCoSettings::predictionLevel()
{
  return m_settings.value( "MMP/predictionLevel", 0 ).toInt();
}

void SCoSettings::setPredictionLevel( int predictionlevel )
{
  m_settings.setValue( "MMP/predictionLevel", predictionlevel );
}

int SCoSettings::defineBounds()
{
  return m_settings.value( "MMP/defineBounds", 1 ).toInt();
}

void SCoSettings::setDefineBounds( int definebounds )
{
  m_settings.setValue( "MMP/defineBounds", definebounds );
}

int SCoSettings::lambda()
{
  return m_settings.value( "MMP/lambda", 1000 ).toInt();
}

void SCoSettings::setLambda( int lambda )
{
  m_settings.setValue( "MMP/lambda", lambda );
}

int SCoSettings::dictionary()
{
  return m_settings.value( "MMP/dictionary", 5000 ).toInt();
}

void SCoSettings::setDictionary( int dictionary )
{
  m_settings.setValue( "MMP/dictionary", dictionary );
}

int SCoSettings::radiusDistortion()
{
  return m_settings.value( "MMP/radiusDistortion", 100 ).toInt();
}

void SCoSettings::setRadiusDistortion( int distortion )
{
  m_settings.setValue( "MMP/radiusDistortion", distortion );
}

int SCoSettings::losslessDistance()
{
  return m_settings.value( "MMP/losslessDistance", 0 ).toInt();
}

void SCoSettings::setLosslessDistance( int distance )
{
  m_settings.setValue( "MMP/losslessDistance", distance );
}

int SCoSettings::mmpAutoSeg()
{
  return m_settings.value( "MMP/autoSeg", 1 ).toInt();
}

void SCoSettings::setMmpAutoSeg( int autoSeg )
{
  m_settings.setValue( "MMP/autoSeg", autoSeg );
}

/*
 bool SCoSettings::fastMode()
 {
 return m_settings.value("MMP/fastMode", 1).toBool();
 }

 void SCoSettings::setFastMode( bool fastmode )
 {
 m_settings.setValue("MMP/fastMode" , fastmode);
 }
 */

QBitArray SCoSettings::mmpTenCheckBoxs()
{
  QBitArray bitArray( 10, false );
  return m_settings.value( "MMP/tenCheckBoxs", bitArray ).toBitArray();
}

void SCoSettings::setMmpTenCheckBoxs( QBitArray tenCheckBoxs )
{
  m_settings.setValue( "MMP/tenCheckBoxs", tenCheckBoxs );
}

// - - - - - - - - - - - - - MMP-compound Decoder settings - - - - - - - - - - - - - - -

QString SCoSettings::binaryPathDec()
{
  return m_settings.value( "MMPdec/binaryPath", QDir::currentPath() ).toString();
}

void SCoSettings::setBinaryPathDec( QString binaryPath )
{
  m_settings.setValue( "MMPdec/binaryPath", binaryPath );
}

QString SCoSettings::oimagePathDec()
{
  return m_settings.value( "MMPdec/oimagePath", QDir::currentPath() ).toString();
}

void SCoSettings::setOimagePathDec( QString oimagePath )
{
  m_settings.setValue( "MMPdec/oimagePath", oimagePath );
}

bool SCoSettings::optOutResDec()
{
  return m_settings.value( "MMPdec/optOutRes", 0 ).toBool();
}

void SCoSettings::setOptOutResDec( bool par )
{
  m_settings.setValue( "MMPdec/optOutRes", par );
}

QString SCoSettings::outResDec()
{
  return m_settings.value( "MMPdec/outRes", "" ).toString();
}

void SCoSettings::setOutResDec( QString str )
{
  m_settings.setValue( "MMPdec/outRes", str );
}

bool SCoSettings::optImgYDec()
{
  return m_settings.value( "MMPdec/optImgY", 0 ).toBool();
}

void SCoSettings::setOptImgYDec( bool par )
{
  m_settings.setValue( "MMPdec/optImgY", par );
}

QString SCoSettings::imgYDec()
{
  return m_settings.value( "MMPdec/imgY", "" ).toString();
}

void SCoSettings::setImgYDec( QString str )
{
  m_settings.setValue( "MMPdec/imgY", str );
}

bool SCoSettings::optImgUDec()
{
  return m_settings.value( "MMPdec/optImgU", 0 ).toBool();
}

void SCoSettings::setOptImgUDec( bool par )
{
  m_settings.setValue( "MMPdec/optImgU", par );
}

QString SCoSettings::imgUDec()
{
  return m_settings.value( "MMPdec/imgU", "" ).toString();
}

void SCoSettings::setImgUDec( QString str )
{
  m_settings.setValue( "MMPdec/imgU", str );
}

bool SCoSettings::optImgVDec()
{
  return m_settings.value( "MMPdec/optImgV", 0 ).toBool();
}

void SCoSettings::setOptImgVDec( bool par )
{
  m_settings.setValue( "MMPdec/optImgV", par );
}

QString SCoSettings::imgVDec()
{
  return m_settings.value( "MMPdec/imgV", "" ).toString();
}

void SCoSettings::setImgVDec( QString str )
{
  m_settings.setValue( "MMPdec/imgV", str );
}

} // namespace SCode
