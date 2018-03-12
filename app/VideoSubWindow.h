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
 * \file     VideoSubWindow.h
 * \brief    Video Sub windows handling
 */

#ifndef __VIDEOSUBWINDOW_H__
#define __VIDEOSUBWINDOW_H__

#include <QDataStream>
#include <QFuture>
#include <QRect>
#include <QString>
#include <QVector>
#include "CommonDefs.h"
#include "SubWindowAbstract.h"
#include "ViewArea.h"
#include "config.h"
#include "lib/CalypStream.h"

class QScrollArea;

class VideoInformation;
class VideoSubWindow;
class CalypAppModuleIf;

typedef struct
{
  QString m_cFilename;
  unsigned int m_uiWidth;
  unsigned int m_uiHeight;
  int m_iPelFormat;
  unsigned int m_uiBitsPelPixel;
  unsigned int m_iEndianness;
  unsigned int m_uiFrameRate;
  unsigned long long int m_uiFileSize;
} CalypFileInfo;
typedef QVector<CalypFileInfo> CalypFileInfoVector;

QDataStream& operator<<( QDataStream& out, const CalypFileInfoVector& d );
QDataStream& operator>>( QDataStream& in, CalypFileInfoVector& d );
int findCalypStreamInfo( CalypFileInfoVector array, QString filename );

class VideoSubWindow : public SubWindowAbstract
{
  Q_OBJECT

private:
  bool m_bWindowBusy;

  QScrollArea* m_pcScrollArea;
  QPoint m_cCurrScroll;
  double m_dHorScroll;
  double m_dVerScroll;

  ViewArea* m_cViewArea;

  VideoInformation* m_pcVideoInfo;

  QString m_cFilename;
  QString m_cStreamInformation;
  CalypFileInfo m_sStreamInfo;
  CalypStream* m_pCurrStream;

  CalypFrame* m_pcCurrFrame;
  QRect m_cSelectedArea;

  CalypAppModuleIf* m_pcCurrentDisplayModule;
  QList<CalypAppModuleIf*> m_apcCurrentModule;

  VideoSubWindow* m_pcReferenceSubWindow;

  // QString m_cWindowShortName;
  QString m_cCurrFileName;

  bool m_bIsPlaying;

  QTimer* m_pcUpdateTimer;
  /**
   * Threads variables
   * QtConcurrent
   */
  QFuture<void> m_cRefreshResult;
  QFuture<void> m_cReadResult;

public:
  enum VideoSubWindowCategories
  {
    VIDEO_STREAM_SUBWINDOW = SubWindowAbstract::VIDEO_STREAM_SUBWINDOW,
    MODULE_SUBWINDOW = SubWindowAbstract::MODULE_SUBWINDOW,
  };
  VideoSubWindow( enum VideoSubWindowCategories category, QWidget* parent = 0 );
  ~VideoSubWindow();

  bool loadFile( QString cFilename, bool bForceDialog = false );
  bool loadFile( CalypFileInfo* streamInfo );
  void loadAll();
  bool save( QString filename );
  bool saveStream( QString filename );

  void refreshSubWindow();
  void refreshFrame( bool bThreaded = false );

  bool play();
  void pause();
  bool playEvent();
  void stop();

  bool isPlaying() { return m_bIsPlaying; }
  void seekAbsoluteEvent( unsigned int new_frame_num );
  void seekRelativeEvent( bool bIsFoward );

  void setCurrFrame( CalypFrame* pcCurrFrame );

  QScrollArea* getScroll() { return m_pcScrollArea; }
  CalypFileInfo getStreamInfo() { return m_sStreamInfo; }
  QString getStreamInformation() { return m_cStreamInformation; }
  CalypStream* getInputStream() { return m_pCurrStream; }
  CalypStream* getCurrStream() { return m_pCurrStream; }
  CalypFrame* getCurrFrame() { return m_pcCurrFrame; }
  ViewArea* getViewArea() { return m_cViewArea; }
  void setRefSubWindow( VideoSubWindow* subWindow )
  {
    m_pcReferenceSubWindow = NULL;
    if( subWindow )
      if( m_pcCurrFrame->haveSameFmt( subWindow->getCurrFrame() ) )
        m_pcReferenceSubWindow = subWindow;
  }

  VideoSubWindow* getRefSubWindow() { return m_pcReferenceSubWindow; }
  /**
   * Functions to enable a module in the
   * current SubWindow
   */
  void enableModule( CalypAppModuleIf* pcModule );
  void disableModule( CalypAppModuleIf* pcModule = NULL );
  void associateModule( CalypAppModuleIf* pcModule );

  CalypAppModuleIf* getDisplayModule() { return m_pcCurrentDisplayModule; }
  QList<CalypAppModuleIf*> getModuleArray()
  {
    QList<CalypAppModuleIf*> apcModulesArray;
    if( m_pcCurrentDisplayModule )
      apcModulesArray.append( m_pcCurrentDisplayModule );
    apcModulesArray.append( m_apcCurrentModule );
    return apcModulesArray;
  }

  /**
   * Virtual functions from SubWindowAbstract
   */
  void normalSize();
  void zoomToFit();
  void scaleView( double scale, QPoint center = QPoint() );
  void zoomToFactor( double factor, QPoint center = QPoint() );

  double getScaleFactor() { return m_cViewArea->getZoomFactor(); }
  /**
   * Size related functions
   */
  QSize sizeHint() const;
  QSize sizeHint( const QSize& ) const;

  QString getCurrentFileName() { return m_cFilename; }
  bool getIsModule() { return getCategory() | SubWindowAbstract::MODULE_SUBWINDOW; }
  void clearWindowBusy() { m_bWindowBusy = false; }
  void setFillWindow( bool bFlag );

  void adjustScrollBarToRatio( const double& horRatio, const double& verRatio );

private:
  void refreshFrameOperation();
  bool goToNextFrame( bool bThreaded = false );

  static bool guessFormat( QString filename, unsigned int& rWidth, unsigned int& rHeight, int& rInputFormat, unsigned int& rBitsPerPixel,
                           int& rEndianness );

  bool hasRunningModule();

  /**
   * Private zoom function to handle
   * zoom to fit
   */
  void scaleView( const QSize& size, QPoint center = QPoint() );
  void updateVideoWindowInfo();

  QSize getScrollSize();

protected:
  void keyPressEvent( QKeyEvent* event );
  void resizeEvent( QResizeEvent* event );
  void closeEvent( QCloseEvent* event );

public Q_SLOTS:
  void updateWindowOnTimeout();
  void adjustScrollBarByScale( double scale, QPoint center );
  void adjustScrollBarByOffset( QPoint Offset );
  void updateScrollValues();
  void updateSelectedArea( QRect area );
  void updatePixelValueStatusBar( const QPoint& pos );
};

Q_DECLARE_METATYPE( CalypFileInfo );
Q_DECLARE_METATYPE( CalypFileInfoVector );

#endif  // __VIDEOSUBWINDOW_H__
