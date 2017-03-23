#####################################################################################
# DLLs and Win32 specific code
######################################################################################

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "." )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "." )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "." )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

SET(MSVC_DLL_DIR "MSVC_DLL_DIR" CACHE PATH "Where to find MSVC dlls")
INSTALL(FILES ${MSVC_DLL_DIR}/msvcr120.dll DESTINATION bin )
INSTALL(FILES ${MSVC_DLL_DIR}/msvcp120.dll DESTINATION bin )

SET(QT_DIR "QT-Dir" CACHE PATH "Where to find QT Lib on Windows")
SET(QT_ICU_VERSION "" CACHE STRING "Qt ICU version of dll's" )
SET(QT_DLL_POSTFIX "" CACHE STRING "Postfix to dll's name for debug" )
SET(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} ${QT_DIR}" )

SET(WINDLLIB "Dl-location" CACHE PATH "Where to find DL Lib on Windows")


IF( NOT USE_QT4 )
  INSTALL(FILES ${QT_DIR}/bin/Qt5Core${QT_DLL_POSTFIX}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/Qt5Gui${QT_DLL_POSTFIX}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/Qt5Widgets${QT_DLL_POSTFIX}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/libGLESv2${QT_DLL_POSTFIX}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/libEGL${QT_DLL_POSTFIX}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/icudt${QT_ICU_VERSION}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/icuin${QT_ICU_VERSION}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/bin/icuuc${QT_ICU_VERSION}.dll DESTINATION bin )
  INSTALL(FILES ${QT_DIR}/plugins/platforms/qminimal.dll DESTINATION bin/platforms )
  INSTALL(FILES ${QT_DIR}/plugins/platforms/qoffscreen.dll DESTINATION bin/platforms )
  INSTALL(FILES ${QT_DIR}/plugins/platforms/qwindows.dll DESTINATION bin/platforms )
ENDIF()

IF( USE_OPENCV )
  SET(OpenCV_DLL_DIR "${OpenCV_DIR}/bin" CACHE PATH "Where to find OpenCV dlls")
  FOREACH(opencv_module IN LISTS OPENCV_MODULES )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_${opencv_module}${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
  ENDFOREACH()
ENDIF()

IF( USE_FFMPEG )
  SET(FFMPEG_DIR "FFmpeg-Dir" CACHE PATH "Where to find FFmpeg Lib on Windows")
  INSTALL(FILES ${FFMPEG_DIR}/bin/avcodec-${AVCODEC_VERSION}.dll DESTINATION bin )
  INSTALL(FILES ${FFMPEG_DIR}/bin/avformat-${AVFORMAT_VERSION}.dll DESTINATION bin )
  INSTALL(FILES ${FFMPEG_DIR}/bin/avutil-${AVUTIL_VERSION}.dll DESTINATION bin )
  INSTALL(FILES ${FFMPEG_DIR}/bin/swresample-1.dll DESTINATION bin )
ENDIF()
