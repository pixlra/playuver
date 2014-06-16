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
#include "PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "PlaYUVerStream.h"
#include "ViewArea.h"
#include "ModulesHandle.h"

namespace plaYUVer
{

class SubWindowHandle: public QMdiSubWindow
{
Q_OBJECT

private:

  QScrollArea* m_cScrollArea;
  ViewArea* m_cViewArea;

  PlaYUVerModuleIf* m_pcCurrentModule;

  InputStream* m_pCurrStream;
  PlaYUVerFrame* m_pcCurrFrame;

  QString m_cWindowName;
  QString m_cCurrFileName;

  Bool m_bIsPlaying;
  Bool m_bIsModule;

  QPoint m_cLastScroll;

public:
  SubWindowHandle( QWidget * parent = 0, Bool isModule = false );
  ~SubWindowHandle();

  Bool mayClose();
  Bool loadFile( const QString &rcFilename, Bool bForceDialog = false );
  Void reloadFile();
  Bool save();

  Void play();
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

  InputStream* getInputStream()
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
  Void setModule( PlaYUVerModuleIf* pcCurrentModule )
  {
    m_pcCurrentModule = pcCurrentModule;
  }

  /**
   * Functions to enable a module in the
   * current SubWindow
   */
  Void enableModule( PlaYUVerModuleIf* select_module );
  Void disableModule();
  Void swapModuleFrames();
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

  QString userFriendlyCurrentFile()
  {
    return m_cWindowName;
  }

  QString currentFile()
  {
    return m_cCurrFileName;
  }

  Bool getIsModule()
  {
    return m_bIsModule;
  }

protected:
  void closeEvent( QCloseEvent *event );

public Q_SLOTS:
  void adjustScrollBarByZoom( double factor, QPoint center );
  void adjustScrollBarByOffset( QPoint Offset );
  void updateLastScrollValue();
};

}  // NAMESPACE

#endif // __SUBWINDOWHANDLE_H__
