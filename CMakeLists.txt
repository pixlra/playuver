###
### CMakeLists for playuver
###

cmake_minimum_required( VERSION 2.8.9 )

######################################################################################
# Project Definition
######################################################################################

SET( PROJECT_NAME "playuver" )

PROJECT( ${PROJECT_NAME} )
INCLUDE( cmake/PlaYUVerVersion.cmake )

INCLUDE( cmake/CMakeMacros.cmake )

# must go before the project command
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
ENDIF()

######################################################################################
# Variables
######################################################################################
ADD_OPTION(PLAYUVER_LONGVERSIONNAME     "Use long version name in deb" OFF )
ADD_OPTION(PLAYUVER_WERROR              "Warnings as errors"       OFF )
ADD_OPTION(PLAYUVER_THREADED_MODULES    "Use threads for modules"  OFF )
ADD_OPTION(USE_QT4                      "Build with QT4"           OFF )
ADD_OPTION(USE_FERVOR                   "Add Fervor support"       OFF )




######################################################################################
# Dependencies
######################################################################################
INCLUDE( cmake/LibFervor.cmake )
INCLUDE( cmake/LibQT.cmake )
INCLUDE( cmake/LibFFmpeg.cmake )
INCLUDE( cmake/LibOpenCV.cmake )



######################################################################################
# Configuration
######################################################################################
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlaYUVerUpdate.xml.in ${CMAKE_BINARY_DIR}/PlaYUVerUpdate.xml @ONLY)

######################################################################################
# Target App
######################################################################################
IF (NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "No build type selected, default to Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3" )

IF( PLAYUVER_WERROR )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
ENDIF()

ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( modules )
ADD_SUBDIRECTORY( app )

IF( WIN32 )
  SET(MSVC_DLL_DIR "MSVC_DLL_DIR" CACHE PATH "Where to find MSVC dlls")
  INSTALL(FILES ${MSVC_DLL_DIR}/msvcr120.dll DESTINATION bin )
  INSTALL(FILES ${MSVC_DLL_DIR}/msvcp120.dll DESTINATION bin )
ENDIF()

######################################################################################
# Create deb package
######################################################################################

SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_PACKAGE_CONTACT "Joao Carreira (jfmcarreira@gmail.com), Luis Lucas (luisfrlucas@gmail.com)")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PLAYUVER_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PLAYUVER_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PLAYUVER_VERSION_PATH})
SET(CPACK_PACKAGE_VERSION ${PLAYUVER_VERSION})
SET(CPACK_PACKAGE_ARCHITECTURE "amd64")

IF( UNIX )
  SET(CPACK_GENERATOR "DEB;ZIP")
  SET(OS "Linux")
  SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "plaYUVer is an open-source QT based raw video player")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joao Carreira (jfmcarreira@gmail.com), Luis Lucas (luisfrlucas@gmail.com)")
  SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_PACKAGE_ARCHITECTURE} )
  STRING(REPLACE ";" "" CPACK_DEBIAN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS} )
ENDIF()
IF( WIN32 )
  SET(CPACK_GENERATOR "ZIP")
  SET(OS "Windows")
  SET(GIT_BRANCH "master") # temp work around
ENDIF()

IF( PLAYUVER_LONGVERSIONNAME )
  SET( CPACK_PACKAGE_FILE_NAME  "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}${APPEND_VERSION}-${OS}-${CPACK_PACKAGE_ARCHITECTURE}" )
  SET( PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME} )
ELSE()
  SET( CPACK_PACKAGE_FILE_NAME  "${CPACK_PACKAGE_NAME}-${PACK_NAME}${APPEND_VERSION}-${OS}-${CPACK_PACKAGE_ARCHITECTURE}" )
  SET( PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME} )
ENDIF()

INCLUDE(CPack)

######################################################################################
# Final information dump
######################################################################################
status("Version:"                 "${PLAYUVER_VERSION}")
status("Configuration:")
status("    Build type:"          "${CMAKE_BUILD_TYPE}")
status("    Build flags:"         "${CMAKE_CXX_FLAGS}")
status("    Warnings as errors:"  USE_WERROR          THEN YES ELSE NO)
status("    Package Name:"        "${PACKAGE_FILE_NAME}" )
status("Dependencies:")
status("    QT:"                  USE_QT4             THEN "QT4" ELSE "QT5")
status("    OpenCV:"              USE_OPENCV          THEN YES ELSE NO)
status("    FFMPEG:"              USE_FFMPEG          THEN YES ELSE NO)
status("    Fervor:"              USE_FERVOR          THEN YES ELSE NO)
status("    Pixfc-sse:"           USE_PIXFC           THEN YES ELSE NO)
