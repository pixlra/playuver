#####################################################################################
# Pixfc-sse library cmake file
######################################################################################
# https://github.com/pixlra/fervor
# git clone https://github.com/pixlra/fervor.git source/lib

SET( FERVOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/lib/fervor )

IF( USE_FERVOR )
  IF(EXISTS ${FERVOR_DIR} )
    SET( FERVOR_APP_NAME ${PROJECT_NAME} ) 
    SET( FERVOR_APP_VERSION ${PLAYUVER_VERSION} )
    ADD_SUBDIRECTORY( ${FERVOR_DIR} )
    INCLUDE_DIRECTORIES( ${FERVOR_DIR} )
    LIST(APPEND LINKER_LIBS "Fervor" )
    
    FIND_PACKAGE( Qt5Network REQUIRED )
    FIND_PACKAGE( Qt5WebKit REQUIRED )
    FIND_PACKAGE( Qt5WebKitWidgets REQUIRED )
    LIST(APPEND LINKER_LIBS ${FERVOR_DEPENDENCIES} )
    INCLUDE_DIRECTORIES( ${FERVOR_INCLUDE_DIRS} )
    
    SET(QT_USE_QTNETWORK true)
    SET(QT_USE_QTWEBKIT true)
    
    IF( WIN32 ) 
      INSTALL(FILES ${QT_DIR}/bin/Qt5Network${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5WebKit${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5WebKitWidgets${QT_DLL_POSTFIX}.dll DESTINATION bin )
    ENDIF() 
    
  ELSE()
    MESSAGE( "Fervor lib source code is not find in ${FERVOR_DIR}... Disabling it!" )
    SET( USE_FERVOR False )
  ENDIF()
ENDIF()