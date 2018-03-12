###
### CMakeLists for calyp app component
###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/external
                     ${CMAKE_CURRENT_SOURCE_DIR}/dialog
                     ${CMAKE_CURRENT_SOURCE_DIR}/docks
                     ${CMAKE_CURRENT_SOURCE_DIR}/widgets )

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

set(Calyp_App_SRCS
# Main
    main.cpp
    MainWindow.cpp
# Module SubWindow
    SubWindowHandle.cpp
    MdiSubWindow.cpp
    SubWindowAbstract.cpp
# Module Video
    VideoHandle.cpp
    VideoSubWindow.cpp
    ViewArea.cpp
    GridManager.cpp
# Module Quality
    QualityHandle.cpp
    PlotSubWindow.cpp
    external/qcustomplot/qcustomplot.cpp
# Module Plugins
    ModulesHandle.cpp
    CalypAppModuleIf.cpp
# Dialogs
    dialog/AboutDialog.cpp
    dialog/ConfigureFormatDialog.cpp
    dialog/SubWindowSelectorDialog.cpp
    dialog/ProgressBar.cpp
    dialog/SeekStreamDialog.cpp
    dialog/ModulesHandleOptDialog.cpp
# Widgets
    widgets/HistogramWidget.cpp
    widgets/FrameNumberWidget.cpp
# Docks
    docks/FramePropertiesDock.cpp
    docks/ModuleHandleDock.cpp
    docks/QualityMeasurementSidebar.cpp
)

IF( USE_QTDBUS )
  LIST( APPEND Calyp_App_SRCS  DBusAppAdaptor.cpp )
ENDIF()


set(Calyp_App_UI "" )

set(Calyp_App_RCC
  resources/${PROJECT_NAME}.qrc
)


IF( USE_FERVOR )

  SET( FERVOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/fervor )

  IF(EXISTS ${FERVOR_DIR} )

    SET( FERVOR_APP_NAME ${PROJECT_NAME} )
    SET( FERVOR_APP_VERSION ${CALYP_VERSION_STRING} )

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

QT5_WRAP_UI( Calyp_App_UI_Wrap ${Calyp_App_UI} )
QT5_ADD_RESOURCES( Calyp_App_RCC ${Calyp_App_RCC} )

# Instruct CMake to run moc automatically when needed.
SET( CMAKE_AUTOMOC ON )

# Windows application icon
IF( WIN32 )
  set(WINDOWS_RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources.obj)
  IF( MSVC )
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND rc.exe /fo ${WINDOWS_RES_FILE} ${PROJECT_NAME}.rc
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources
    )
  ELSE()
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND windres.exe ${PROJECT_NAME}.rc ${WINDOWS_RES_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources
    )
  ENDIF()
ENDIF()


ADD_EXECUTABLE( ${PROJECT_NAME} ${Calyp_App_SRCS} ${HEADERS_MOC_FILES} ${Calyp_App_UI_Wrap} ${Calyp_App_RCC} ${WINDOWS_RES_FILE} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARY} CalypModules ${APP_LINKER_LIBS} ${QT_LINKER_LIBS} QCustomPlotLib )

# QT5_USE_MODULES( ${PROJECT_NAME} ${QT_MODULES} )

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/calyp.desktop DESTINATION share/applications )
INSTALL(FILES resources/calyp.png DESTINATION share/pixmaps )
INSTALL(FILES resources/calyp.xml DESTINATION share/mime/packages )



