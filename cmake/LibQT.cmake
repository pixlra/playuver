#####################################################################################
# Qt library cmake file
######################################################################################

IF( WIN32 )
  SET(QT_DIR "QT-Dir" CACHE PATH "Where to find QT Lib on Windows")
  SET(QT_DLL_POSTFIX "" CACHE STRING "Postfix to dll's name for debug" )
	SET(CMAKE_PREFIX_PATH ${QT_DIR} )
ENDIF()

IF( USE_QT4 )

  FIND_PACKAGE( Qt4 REQUIRED QtCore QtGui QtNetwork QtWebKit )
  IF( NOT QT4_FOUND)
      SET(USE_QT4 FALSE)
  ENDIF()
  INCLUDE( ${QT_USE_FILE} )
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )
  
  QT4_WRAP_UI( FORMS_HEADERS_FILES ${FORMS_FILES} )
  QT4_ADD_RESOURCES( RESOURCES_RCC_FILES ${RESOURCES_FILES} )
  LIST(APPEND LINKER_LIBS ${QT_LIBRARIES} )
  
ENDIF()

IF( NOT USE_QT4 )

  FIND_PACKAGE( Qt5Core REQUIRED )
  FIND_PACKAGE( Qt5Gui REQUIRED )
  FIND_PACKAGE( Qt5Widgets REQUIRED )

  INCLUDE_DIRECTORIES( ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} )
  # QT5_WRAP_CPP( HEADERS_MOC_FILES ${HEADERS_FILES} )
  QT5_WRAP_UI( FORMS_HEADERS_FILES ${FORMS_FILES} )
  QT5_ADD_RESOURCES( RESOURCES_RCC_FILES ${RESOURCES_FILES} )
  
  # Add compiler flags for building executables (-fPIE)
  LIST(APPEND CMAKE_CXX_FLAGS ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} )
  
  LIST(APPEND LINKER_LIBS ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} )
  
  IF( WIN32 ) 
    INSTALL(FILES ${QT_DIR}/bin/Qt5Core${QT_DLL_POSTFIX}.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/Qt5Gui${QT_DLL_POSTFIX}.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/Qt5Widgets${QT_DLL_POSTFIX}.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/libGLESv2${QT_DLL_POSTFIX}.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/libEGL${QT_DLL_POSTFIX}.dll DESTINATION bin )    
    INSTALL(FILES ${QT_DIR}/bin/icudt52.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/icuin52.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/icuuc52.dll DESTINATION bin )
    
    INSTALL(FILES ${QT_DIR}/plugins/platforms/qminimal.dll DESTINATION bin/platforms )
    INSTALL(FILES ${QT_DIR}/plugins/platforms/qoffscreen.dll DESTINATION bin/platforms )
    INSTALL(FILES ${QT_DIR}/plugins/platforms/qwindows.dll DESTINATION bin/platforms )
  ENDIF() 
  
ENDIF()
