/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     VideoSubWindow.h
 * \brief    Video Sub windows handling
 */

#ifndef __VIDEOSUBWINDOW_H__
#define __VIDEOSUBWINDOW_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "QFuture"
#include "lib/PlaYUVerStream.h"
#include "SubWindowHandle.h"
#include "ViewArea.h"

namespace plaYUVer
{

class VideoSubWindow;
class PlaYUVerAppModuleIf;

typedef struct
{
  QString m_cFilename;
  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPelFormat;
  UInt m_uiBitsPelPixel;
  UInt m_uiFrameRate;
  UInt64 m_uiFileSize;
} PlaYUVerStreamInfo;
typedef QVector<PlaYUVerStreamInfo> PlaYUVerStreamInfoVector;

QDataStream& operator<<( QDataStream& out, const PlaYUVerStreamInfoVector& d );
QDataStream& operator>>( QDataStream& in, PlaYUVerStreamInfoVector& d );
Int findPlaYUVerStreamInfo( PlaYUVerStreamInfoVector array, QString filename );

class VideoSubWindow: public SubWindowHandle
{
Q_OBJECT

private:

  ViewArea* m_cViewArea;

  QString m_cFilename;
  QString m_cStreamInformation;
  PlaYUVerStreamInfo m_sStreamInfo;
  PlaYUVerStream* m_pCurrStream;

  PlaYUVerFrame* m_pcCurrFrame;
  QRect m_cSelectedArea;

  PlaYUVerAppModuleIf* m_pcCurrentDisplayModule;
  QList<PlaYUVerAppModuleIf*> m_apcCurrentModule;

  VideoSubWindow* m_pcReferenceSubWindow;

  QString m_cWindowShortName;
  QString m_cCurrFileName;

  Bool m_bIsPlaying;
  Bool m_bIsModule;

  /**
   * Threads variables
   * QtConcurrent
   */
  QFuture<Void> m_cRefreshResult;
  QFuture<Void> m_cReadResult;

  Void refreshFrame( Bool bThreaded = false );
  Void refreshFrameOperation();

  static Bool guessFormat( QString filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );

  /**
   * Private zoom function to handle
   * zoom to fit
   */
  Void scaleView( const QSize & size, QPoint center = QPoint() );

  Void updateVideoWindowInfo();

public:
  enum VideoSubWindowCategories
  {
    VIDEO_STREAM_SUBWINDOW = 2,
    MODULE_SUBWINDOW = 4,
  };
  VideoSubWindow( enum VideoSubWindowCategories category, QWidget * parent = 0 );
  ~VideoSubWindow();

  Bool loadFile( QString cFilename, Bool bForceDialog = false );
  Bool loadFile( PlaYUVerStreamInfo* streamInfo );
  Void loadAll();
  Bool save( QString filename );

  Void refreshSubWindow();

  Bool play();
  Void pause();
  Bool playEvent();
  Void stop();

  Bool isPlaying()
  {
    return m_bIsPlaying;
  }

  Void seekAbsoluteEvent( UInt new_frame_num );
  Void seekRelativeEvent( Bool bIsFoward );

  Void setCurrFrame( PlaYUVerFrame* pcCurrFrame );

  PlaYUVerStreamInfo getStreamInfo()
  {
    return m_sStreamInfo;
  }
  QString getStreamInformation()
  {
    return m_cStreamInformation;
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

  Void setRefSubWindow( VideoSubWindow* subWindow )
  {
    m_pcReferenceSubWindow = NULL;
    if( subWindow )
      if( m_pcCurrFrame->haveSameFmt( subWindow->getCurrFrame() ) )
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
  Void enableModule( PlaYUVerAppModuleIf* pcModule );
  Void disableModule( PlaYUVerAppModuleIf* pcModule = NULL );
  Void associateModule( PlaYUVerAppModuleIf* pcModule );

  PlaYUVerAppModuleIf* getDisplayModule()
  {
    return m_pcCurrentDisplayModule;
  }

  QList<PlaYUVerAppModuleIf*> getModuleArray()
  {
    return m_apcCurrentModule;
  }

  Void setTool( UInt uiTool )
  {
    m_cViewArea->setTool( ( ViewArea::eTool )uiTool );
  }

  /**
   * Virtual functions from SubWindowHandle
   */
  Void normalSize();
  Void zoomToFit();
  Void scaleView( Double scale, QPoint center = QPoint() );
  Void zoomToFactor( Double factor, QPoint center = QPoint() );

  Double getScaleFactor()
  {
    return m_cViewArea->getZoomFactor();
  }

  /**
   * Size related functions
   */
  QSize sizeHint() const;
  QSize sizeHint( const QSize & ) const;

  Void setWindowShortName( const QString& name )
  {
    m_cWindowShortName = name;
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
  void updatePixelValueStatusBar( const QPoint& pos );
};

}  // NAMESPACE

Q_DECLARE_METATYPE( plaYUVer::PlaYUVerStreamInfo );
Q_DECLARE_METATYPE( plaYUVer::PlaYUVerStreamInfoVector );

#endif // __VIDEOSUBWINDOW_H__

