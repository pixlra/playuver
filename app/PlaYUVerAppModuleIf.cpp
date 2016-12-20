/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     PlaYUVerAppModuleIf.cpp
 * \brief    PlaYUVer modules interface
 */

#include "PlaYUVerAppModuleIf.h"
#include "VideoSubWindow.h"
#include "VideoSubWindow.h"

PlaYUVerAppModuleIf::PlaYUVerAppModuleIf( QObject* parent, QAction* action, PlaYUVerModuleIf* module ) :
        m_bIsRunning( false ),
        m_pcModuleAction( action ),
        m_pcModule( module ),
        m_pcDisplaySubWindow( NULL ),
        m_pcDockWidget( NULL ),
        m_pcModuleDock( NULL ),
        m_pcModuleStream( NULL ),
        m_pcProcessedFrame( NULL ),
        m_dMeasurementResult( 0 )
{
  setParent( parent );
  for( Int i = 0; i < MODULE_REQUIRES_MAX_NUM_FRAMES; i++ )
  {
    m_pcSubWindow[i] = NULL;
  }
}

Void PlaYUVerAppModuleIf::update()
{
  if( m_pcDisplaySubWindow )
  {
    m_pcDisplaySubWindow->refreshFrame();
  }
  else
  {
    apply();
  }
}

Bool PlaYUVerAppModuleIf::apply( Bool isPlaying, Bool disableThreads )
{
  Bool bRet = false;

  QApplication::setOverrideCursor( Qt::WaitCursor );
  if( m_pcModule->m_iModuleType == FRAME_PROCESSING_MODULE )
  {
    if( m_pcDisplaySubWindow )
    {
      m_pcDisplaySubWindow->setFillWindow( true );
    }
    else
    {
      m_pcSubWindow[0]->setFillWindow( true );
    }
  }
  if( !( isPlaying && ( m_pcModule->m_uiModuleRequirements & MODULE_REQUIRES_SKIP_WHILE_PLAY ) ) )
  {
#ifdef PLAYUVER_THREADED_MODULES
    if( !disableThreads )
    {
      wait();
      start();
    }
    else
#endif
    {
      run();
      show();
    }

    if( m_pcDisplaySubWindow || m_pcModule->m_iModuleType == FRAME_MEASUREMENT_MODULE )
    {
      bRet = true;
    }
  }
  else
  {
    bRet = true;
  }
  QApplication::restoreOverrideCursor();
  return bRet;
}

Void PlaYUVerAppModuleIf::run()
{
  m_bIsRunning = true;
  m_bSuccess = false;
  std::vector<PlaYUVerFrame*> apcFrameList;
  for( UInt i = 0; i < m_pcModule->m_uiNumberOfFrames; i++ )
  {
    apcFrameList.push_back( m_pcSubWindow[i]->getCurrFrame() );
  }

  if( m_pcModule->m_iModuleType == FRAME_PROCESSING_MODULE )
  {
    if( m_pcModule->m_iModuleAPI == MODULE_API_2 )
    {
      m_pcProcessedFrame = m_pcModule->process( apcFrameList );
    }
    else
    {
      m_pcProcessedFrame = m_pcModule->process( m_pcSubWindow[0]->getCurrFrame() );
    }
  }
  else if( m_pcModule->m_iModuleType == FRAME_MEASUREMENT_MODULE )
  {
    if( m_pcModule->m_iModuleAPI == MODULE_API_2 )
    {
      m_dMeasurementResult = m_pcModule->measure( apcFrameList );
    }
    else
    {
      m_dMeasurementResult = m_pcModule->measure( m_pcSubWindow[0]->getCurrFrame() );
    }
  }
  else
  {
    return;
  }
  m_bSuccess = true;
  m_bIsRunning = false;
#ifdef PLAYUVER_THREADED_MODULES
  EventData *eventData = new EventData( m_bSuccess, this );
  if( parent() )
  QCoreApplication::postEvent( parent(), eventData );
#endif
  return;
}

Void PlaYUVerAppModuleIf::show()
{
  switch( m_pcModule->m_iModuleType )
  {
  case FRAME_PROCESSING_MODULE:
    if( m_pcDisplaySubWindow )
    {
      m_pcDisplaySubWindow->setCurrFrame( m_pcProcessedFrame );
      m_pcDisplaySubWindow->setFillWindow( false );
      m_pcDisplaySubWindow->clearWindowBusy();
    }
    else
    {
      m_pcSubWindow[0]->setCurrFrame( m_pcProcessedFrame );
      m_pcSubWindow[0]->setFillWindow( false );
      m_pcSubWindow[0]->clearWindowBusy();
    }
    break;
  case FRAME_MEASUREMENT_MODULE:
    m_pcModuleDock->setModulueReturnValue( m_dMeasurementResult );
    break;
  }
}

Void PlaYUVerAppModuleIf::destroy()
{
  if( m_bIsRunning )
  {
    QApplication::setOverrideCursor( Qt::WaitCursor );
    while( m_bIsRunning )
    {
    }
    QApplication::restoreOverrideCursor();
  }
  if( m_pcModuleDock )
  {
    m_pcModuleDock->close();
    m_pcModuleDock = NULL;
  }
  if( m_pcDockWidget )
  {
    m_pcDockWidget->close();
    m_pcDockWidget = NULL;
  }
  if( m_pcDisplaySubWindow )
    m_pcDisplaySubWindow->closeSubWindow();
  m_pcDisplaySubWindow = NULL;

  for( Int i = 0; i < MODULE_REQUIRES_MAX_NUM_FRAMES; i++ )
  {
    if( m_pcSubWindow[i] )
    {
      m_pcSubWindow[i]->disableModule( this );
      m_pcSubWindow[i] = NULL;
    }
  }
  if( m_pcModuleStream )
  {
    m_pcModuleStream->close();
    delete m_pcModuleStream;
    m_pcModuleStream = NULL;
  }
  if( m_pcModule )
  {
    m_pcModule->destroy();
    m_pcModule->Delete();
    m_pcModule = NULL;
  }

}

