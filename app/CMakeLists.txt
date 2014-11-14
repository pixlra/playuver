###
### CMakeLists for playuver app component
###

# This is hard way - please fix
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/modules ${CMAKE_BINARY_DIR}/modules )

set(plaYUver_App_SRCS
# Main
    main.cpp
    plaYUVerApp.cpp
    SubWindowHandle.cpp
    ViewArea.cpp
    ModulesHandle.cpp
    PlaYUVerAppModuleIf.cpp
    PlaYUVerSettings.cpp
    GridManager.cpp
# Dialogs
    AboutDialog.cpp
    ConfigureFormatDialog.cpp
    DialogSubWindowSelector.cpp
# Docks / Sidebars
    ModuleHandleDock.cpp
    PropertiesSidebar.cpp
    PlaYUVerFrameStatistics.cpp
    QualityMeasurementSidebar.cpp
# Widgets
    HistogramWidget.cpp
    WidgetFrameNumber.cpp
    WidgetFrameNumber.cpp
)

set(plaYUver_App_UI "" )

set(plaYUver_App_RCC
  resources/playuver.qrc
)


IF( USE_FERVOR )
  IF(EXISTS ${FERVOR_DIR} )
    SET( FERVOR_APP_NAME ${PROJECT_NAME} )
    SET( FERVOR_APP_VERSION ${PLAYUVER_VERSION} )
    ADD_SUBDIRECTORY( ${FERVOR_DIR} )
    INCLUDE_DIRECTORIES( ${FERVOR_DIR} )
    LIST(APPEND APP_LINKER_LIBS "Fervor" )

    FIND_PACKAGE( Qt5 REQUIRED Network WebKit WebKitWidgets )

    LIST(APPEND QT_LINKER_LIBS Qt5::Network Qt5::Webkit Qt5::WebKitWidgets )
    LIST(APPEND QT_MODULES Network WebKit WebKitWidgets )

    LIST(APPEND LINKER_LIBS ${FERVOR_DEPENDENCIES} )
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
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/app/resources
    )
  ELSE()
    add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
      COMMAND windres.exe playuver.rc ${WINDOWS_RES_FILE}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/app/resources
    )
  ENDIF()
ENDIF()


ADD_EXECUTABLE( ${PROJECT_NAME} ${plaYUver_App_SRCS} ${HEADERS_MOC_FILES} ${plaYUver_App_UI_Wrap} ${plaYUver_App_RCC} ${WINDOWS_RES_FILE} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PlaYUVerLib PlaYUVerModules ${QT_LINKER_LIBS} ${APP_LINKER_LIBS} )

IF( NOT USE_QT4 )
  QT5_USE_MODULES( ${PROJECT_NAME} ${QT_MODULES} )
ENDIF()

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/icons )
INSTALL(FILES resources/x-raw.xml DESTINATION share/mime/video )