###
### CMakeLists for playuver app component
###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/external )

OPTION( USE_QTDBUS    "Build with QtDBus"         ON  )
OPTION( USE_FERVOR    "Add Fervor support"        OFF )

ADD_SUBDIRECTORY( external )

FIND_PACKAGE( Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent )
LIST(APPEND QT_LINKER_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent )
LIST(APPEND QT_MODULES Core Gui Widgets Concurrent )

IF( USE_QTDBUS )
  FIND_PACKAGE( Qt5DBus )
  SET_PACKAGE_PROPERTIES(Qt5DBus PROPERTIES DESCRIPTION "Support for Qt5 DBus messages between instances" TYPE OPTIONAL )
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
    PlaYUVerApp.cpp
    PlaYUVerSubWindowHandle.cpp
    PlaYUVerMdiSubWindow.cpp
    SubWindowAbstract.cpp
# Module Video
    VideoSubWindow.cpp
    ViewArea.cpp
    VideoHandle.cpp
    FramePropertiesDock.cpp
# Module Quality
    QualityHandle.cpp
    QualityMeasurementSidebar.cpp
    PlotSubWindow.cpp
    external/qcustomplot/qcustomplot.cpp
# Module Plugins
    ModulesHandle.cpp
    ModulesHandleOptDialog.cpp
    ModuleHandleDock.cpp
    PlaYUVerAppModuleIf.cpp
    GridManager.cpp
# Dialogs
    AboutDialog.cpp
    ConfigureFormatDialog.cpp
    DialogSubWindowSelector.cpp
    ProgressBar.cpp
    SeekStreamDialog.cpp
# Widgets
    HistogramWidget.cpp
    WidgetFrameNumber.cpp
    WidgetFrameNumber.cpp
)

IF( USE_QTDBUS )
  LIST( APPEND plaYUVer_App_SRCS  PlaYUVerAppAdaptor.cpp )
ENDIF()


set(PlaYUVer_App_UI "" )

set(PlaYUVer_App_RCC
  resources/playuver.qrc
)


IF( USE_FERVOR )

  SET( FERVOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fervor )

  IF(EXISTS ${FERVOR_DIR} )

    SET( FERVOR_APP_NAME ${PROJECT_NAME} )
    SET( FERVOR_APP_VERSION ${PLAYUVER_VERSION_STRING} )

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

  ELSE()
    MESSAGE( "Fervor lib source code is not find in ${FERVOR_DIR}... Disabling it!" )
    SET( USE_FERVOR False )
  ENDIF()
ENDIF()

SET_PACKAGE_PROPERTIES(Fervor PROPERTIES URL "https://github.com/pypt/fervor" DESCRIPTION "Fervor lib includes check for updates feature" TYPE OPTIONAL )
ADD_FEATURE_INFO(Fervor USE_FERVOR "Fervor lib includes check for updates feature.")

QT5_WRAP_UI( PlaYUVer_App_UI_Wrap ${PlaYUVer_App_UI} )
QT5_ADD_RESOURCES( PlaYUVer_App_RCC ${PlaYUVer_App_RCC} )

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


ADD_EXECUTABLE( ${PROJECT_NAME} ${plaYUVer_App_SRCS} ${HEADERS_MOC_FILES} ${PlaYUVer_App_UI_Wrap} ${PlaYUVer_App_RCC} ${WINDOWS_RES_FILE} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PlaYUVerLib PlaYUVerModules QCustomPlotLib ${APP_LINKER_LIBS} ${QT_LINKER_LIBS} )

# QT5_USE_MODULES( ${PROJECT_NAME} ${QT_MODULES} )

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/pixmaps )
INSTALL(FILES resources/playuver.xml DESTINATION share/mime/packages )



