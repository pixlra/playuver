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
 * \file     SubWindowHandle.h
 * \brief    Sub windows handling
 */

#ifndef __SUBWINDOWHANDLE_H__
#define __SUBWINDOWHANDLE_H__

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

namespace plaYUVer
{

class SubWindowHandle;

class SubWindowHandle: public QMdiSubWindow
{
Q_OBJECT

private:

  QScrollArea* m_cScrollArea;
  ViewArea* m_cViewArea;

  PlaYUVerStream* m_pCurrStream;
  PlaYUVerFrame* m_pcCurrFrame;
  QRect m_cSelectedArea;

  QString m_cFilename;
  PlaYUVerAppModuleIf* m_pcCurrentModule;

  SubWindowHandle* m_pcReferenceSubWindow;

  QString m_cWindowName;

  QString m_cWindowShortName;
  QString m_cCurrFileName;

  Bool m_bIsPlaying;
  Bool m_bIsModule;

  QPoint m_cLastScroll;

public:
  SubWindowHandle( QWidget * parent = 0, Bool isModule = false );
  ~SubWindowHandle();

  Bool mayClose();
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
    PlaYUVerStreamInfo info;
    if( m_pCurrStream )
      info = m_pCurrStream->getStreamInfo();
    return info;
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

  Void setRefSubWindow( SubWindowHandle* subWindow )
  {
    m_pcReferenceSubWindow = subWindow;
  }
  SubWindowHandle* getRefSubWindow()
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
   * Show the image at its original size
   */
  Void normalSize();
  /**
   * Scale the image (zoomed in or out) to fit on the window.
   */
  Void zoomToFit();
  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  Void scaleView( Double scale );

  /**
   * The current image size is scaled to a rectangle as large as possible
   * inside (@p width, @p height ) preserving the aspect ratio.
   */
  Void scaleView( Int width, Int height );

  /**
   * This is an overloaded member function, provided for convenience.
   * Scales the image to a rectangle with the given size, preserving the
   * aspect ratio.
   */
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

#endif // __SUBWINDOWHANDLE_H__

