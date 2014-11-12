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

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PlaYUVerLib PlaYUVerModules ${LINKER_LIBS} )

IF( NOT USE_QT4 )
  QT5_USE_MODULES( ${PROJECT_NAME} ${QT_MODULES} )
ENDIF()

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/icons )
INSTALL(FILES resources/x-raw.xml DESTINATION share/mime/video )