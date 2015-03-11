###
### CMakeLists for playuver app component
###

# This is hard way - please fix
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/../
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../
                     ${CMAKE_CURRENT_SOURCE_DIR}/external
                     ${CMAKE_CURRENT_SOURCE_DIR}/../modules
                     ${CMAKE_CURRENT_BINARY_DIR}/../modules )

#####################################################################################
# Qt library
######################################################################################

IF( USE_QT4 )

  FIND_PACKAGE( Qt4 REQUIRED COMPONENTS QtCore QtGui QtDBus QtNetwork QtWebKit )
  SET( USE_QTDBUS TRUE )
  IF( NOT QT4_FOUND )
      SET(USE_QT4 FALSE)
  ELSE()
    INCLUDE( ${QT_USE_FILE} )
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    SET( QT_LINKER_LIBS ${QT_LIBRARIES} )
  ENDIF()
ENDIF()

IF( NOT USE_QT4 )
  FIND_PACKAGE( Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Concurrent )
  LIST(APPEND QT_LINKER_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::PrintSupport Qt5::Concurrent )
  LIST(APPEND QT_MODULES Core Gui Widgets PrintSupport Concurrent )

  FIND_PACKAGE( Qt5DBus )
  IF( ${Qt5DBus_FOUND} )
    SET( USE_QTDBUS ${Qt5DBus_FOUND} ) # TODO: fix this stupid variable
    SET( USE_QTDBUS ${Qt5DBus_FOUND} PARENT_SCOPE)
    LIST(APPEND QT_LINKER_LIBS Qt5::DBus )
    LIST(APPEND QT_MODULES DBus )
  ENDIF()

ENDIF()

set(plaYUVer_App_SRCS
# Main
    main.cpp
    plaYUVerApp.cpp
    PlaYUVerSubWindowHandle.cpp
    PlaYUVerMdiSubWindow.cpp
    SubWindowHandle.cpp
    Settings.cpp
# Module Video
    VideoSubWindow.cpp
    ViewArea.cpp
    VideoHandle.cpp
    FramePropertiesSidebar.cpp
# Module Quality
    QualityHandle.cpp
    QualityMeasurementSidebar.cpp
    PlotSubWindow.cpp
    external/qcustomplot.cpp
# Module Plugins
    ModulesHandle.cpp
    ModuleHandleDock.cpp
    PlaYUVerAppModuleIf.cpp
    GridManager.cpp
# Dialogs
    AboutDialog.cpp
    ConfigureFormatDialog.cpp
    DialogSubWindowSelector.cpp
# Widgets
    HistogramWidget.cpp
    WidgetFrameNumber.cpp
    WidgetFrameNumber.cpp
)

IF( USE_QTDBUS )
  LIST( APPEND plaYUVer_App_SRCS  PlaYUVerAppAdaptor.cpp )
ENDIF()


set(plaYUver_App_UI "" )

set(plaYUver_App_RCC
  resources/playuver.qrc
)

OPTION( USE_FERVOR "Add Fervor support" OFF )


IF( USE_FERVOR )

  SET( FERVOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fervor )

  IF(EXISTS ${FERVOR_DIR} )

    SET( FERVOR_APP_NAME ${PROJECT_NAME} )
    SET( FERVOR_APP_VERSION ${PLAYUVER_VERSION} )

    ADD_SUBDIRECTORY( ${FERVOR_DIR} )

    INCLUDE_DIRECTORIES( ${FERVOR_DIR} )

    LIST(APPEND APP_LINKER_LIBS Fervor )

    FIND_PACKAGE( Qt5 REQUIRED Network WebKit WebKitWidgets )

    LIST(APPEND QT_LINKER_LIBS Qt5::Network Qt5::WebKit Qt5::WebKitWidgets )
    LIST(APPEND QT_MODULES Network WebKit WebKitWidgets )

#     LIST(APPEND APP_LINKER_LIBS ${FERVOR_DEPENDENCIES} )
    INCLUDE_DIRECTORIES( ${FERVOR_INCLUDE_DIRS} )

    SET(QT_USE_QTNETWORK true)
    SET(QT_USE_QTWEBKIT true)

    IF( WIN32 )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Network${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5WebKit${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5WebKitWidgets${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Quick${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Qml${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Multimedia${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Sql${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Positioning${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5Sensors${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5MultimediaWidgets${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5OpenGL${QT_DLL_POSTFIX}.dll DESTINATION bin )
      INSTALL(FILES ${QT_DIR}/bin/Qt5PrintSupport${QT_DLL_POSTFIX}.dll DESTINATION bin )
    ENDIF()

  ELSE()
    MESSAGE( "Fervor lib source code is not find in ${FERVOR_DIR}... Disabling it!" )
    SET( USE_FERVOR False )
  ENDIF()
ENDIF()

SET_PACKAGE_PROPERTIES(Fervor PROPERTIES URL "http://ffmpeg.org/" DESCRIPTION "Fervor lib includes check for updates feature" TYPE OPTIONAL )
ADD_FEATURE_INFO(Fervor USE_FERVOR "Fervor lib includes check for updates feature.")

IF( USE_QT4 )
  QT4_WRAP_UI( plaYUver_App_UI_Wrap ${plaYUver_App_UI} )
  QT4_ADD_RESOURCES( plaYUver_App_RCC ${plaYUver_App_RCC} )
ELSE()
  QT5_WRAP_UI( plaYUver_App_UI_Wrap ${plaYUver_App_UI} )
  QT5_ADD_RESOURCES( plaYUver_App_RCC ${plaYUver_App_RCC} )
ENDIF()

# Instruct CMake to run moc automatically when needed.
SET( CMAKE_AUTOMOC ON )

# Windows application icon
IF( WIN32 )
  set(WINDOWS_RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources.obj)
  IF( MSVC )
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND rc.exe /fo ${WINDOWS_RES_FILE} playuver.rc
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources
    )
  ELSE()
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND windres.exe playuver.rc ${WINDOWS_RES_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources
    )
  ENDIF()
ENDIF()


ADD_EXECUTABLE( ${PROJECT_NAME} ${plaYUVer_App_SRCS} ${HEADERS_MOC_FILES} ${plaYUver_App_UI_Wrap} ${plaYUver_App_RCC} ${WINDOWS_RES_FILE} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PlaYUVerLib PlaYUVerModules ${APP_LINKER_LIBS} ${QT_LINKER_LIBS} )

IF( NOT USE_QT4 )
  QT5_USE_MODULES( ${PROJECT_NAME} ${QT_MODULES} )
ENDIF()

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/pixmaps )
INSTALL(FILES resources/playuver.xml DESTINATION share/mime/packages )



