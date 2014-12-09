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
 * \file     VideoSubWindow.h
 * \brief    Video Sub windows handling
 */

#ifndef __VIDEOSUBWINDOW_H__
#define __VIDEOSUBWINDOW_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "lib/PlaYUVerStream.h"
#include "ViewArea.h"
#include "ModulesHandle.h"
#include "SubWindowHandle.h"

namespace plaYUVer
{

class VideoSubWindow;

typedef struct
{
  QString m_cFilename;
  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPelFormat;
  UInt m_uiFrameRate;
} PlaYUVerStreamInfo;
typedef QVector<PlaYUVerStreamInfo> PlaYUVerStreamInfoVector;

QDataStream& operator<<( QDataStream& out, const PlaYUVerStreamInfoVector& d );
QDataStream& operator>>( QDataStream& in, PlaYUVerStreamInfoVector& d );
Int findPlaYUVerStreamInfo( PlaYUVerStreamInfoVector array, QString filename );

class VideoSubWindow: public SubWindowHandle
{
Q_OBJECT

private:

  QScrollArea* m_cScrollArea;
  ViewArea* m_cViewArea;

  PlaYUVerStreamInfo m_sStreamInfo;

  PlaYUVerStream* m_pCurrStream;
  PlaYUVerFrame* m_pcCurrFrame;
  QRect m_cSelectedArea;

  QString m_cFilename;
  PlaYUVerAppModuleIf* m_pcCurrentModule;

  VideoSubWindow* m_pcReferenceSubWindow;

  QString m_cWindowName;

  QString m_cWindowShortName;
  QString m_cCurrFileName;

  Bool m_bIsPlaying;
  Bool m_bIsModule;

  QPoint m_cLastScroll;

public:
  VideoSubWindow( QWidget * parent = 0, Bool isModule = false );
  ~VideoSubWindow();

  Bool loadFile( QString cFilename, Bool bForceDialog = false );
  Bool loadFile( PlaYUVerStreamInfo* streamInfo );
  Void reloadFile();
  Bool save( QString filename );

  Bool play();
  Void pause();
  Int playEvent();
  Void stop();

  Bool isPlaying()
  {
    return m_bIsPlaying;
  }

  Void seekAbsoluteEvent( UInt new_frame_num );
  Void seekRelativeEvent( Bool bIsFoward );

  QImage* FrameToQImage( PlaYUVerFrame* curr_frame );

  Void refreshFrame();
  Void setCurrFrame( PlaYUVerFrame* pcCurrFrame );

  PlaYUVerStreamInfo getStreamInfo()
  {
    return m_sStreamInfo;
  }

  PlaYUVerStream* getInputStream()
  {
    return m_pCurrStream;
  }

  PlaYUVerFrame* getCurrFrame()
  {
    return m_pcCurrFrame;
  }

  ViewArea* getViewArea()
  {
    return m_cViewArea;
  }

  Double getZoomFactor()
  {
    return m_cViewArea->getZoomFactor();
  }

  Void setModule( PlaYUVerAppModuleIf* pcCurrentModule )
  {
    m_pcCurrentModule = pcCurrentModule;
  }
  PlaYUVerAppModuleIf* getModule()
  {
    return m_pcCurrentModule;
  }

  Void setRefSubWindow( VideoSubWindow* subWindow )
  {
    m_pcReferenceSubWindow = subWindow;
  }
  VideoSubWindow* getRefSubWindow()
  {
    return m_pcReferenceSubWindow;
  }

  /**
   * Functions to enable a module in the
   * current SubWindow
   */
  Void enableModule( PlaYUVerAppModuleIf* select_module );
  Void disableModule();
  Void swapModuleFrames();
  Void applyModuleAllFrames();

  /**
   * Reimplemented function from SubWindowHandle
   */
  Void normalSize();
  Void zoomToFit();
  Void scaleView( Double scale );
  Void scaleView( Int width, Int height );
  Void scaleView( const QSize & size );

  Void scaleViewByRatio( Double ratio );
  Double getScaleFactor()
  {
    return m_cViewArea->getZoomFactor();
  }

  QSize sizeHint() const;

  Void setWindowName( QString name )
  {
    m_cWindowName = name;
    setWindowTitle( m_cWindowName );
  }
  QString getWindowName()
  {
    return m_cWindowName;
  }
  QString getWindowShortName()
  {
    return m_cWindowShortName;
  }
  QString getCurrentFileName()
  {
    return m_cFilename;
  }

  Bool getIsModule()
  {
    return m_bIsModule;
  }

protected:
  void closeEvent( QCloseEvent *event );

public Q_SLOTS:
  void updateSelectedArea( QRect area );
  void adjustScrollBarByZoom( double factor, QPoint center );
  void adjustScrollBarByOffset( QPoint Offset );
  void updateLastScrollValue();
};

}  // NAMESPACE

Q_DECLARE_METATYPE( plaYUVer::PlaYUVerStreamInfo );
Q_DECLARE_METATYPE( plaYUVer::PlaYUVerStreamInfoVector );

#endif // __VIDEOSUBWINDOW_H__
