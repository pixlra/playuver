###
### CMakeLists for playuver
###

cmake_minimum_required( VERSION 2.8.9 )

######################################################################################
# Project Definition
######################################################################################

SET( PROJECT_NAME "playuver" )

PROJECT( ${PROJECT_NAME} )
SET( PLAYUVER_VERSION_MAJOR "0" )
SET( PLAYUVER_VERSION_MINOR "5" )
SET( PLAYUVER_VERSION_PATCH "0" )
SET( PLAYUVER_VERSION "${PLAYUVER_VERSION_MAJOR}.${PLAYUVER_VERSION_MINOR}.${PLAYUVER_VERSION_PATCH}")

######################################################################################
# CMake Definitions
######################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )
INCLUDE(FeatureSummary)

# must go before the project command
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
ENDIF()
SET( CMAKE_CXX_FLAGS "" )


######################################################################################
# Variables
######################################################################################

OPTION( USE_FERVOR                   "Add Fervor support"       OFF )
OPTION( PLAYUVER_LONGVERSIONNAME     "Use long version name in deb" OFF )
OPTION( PLAYUVER_WERROR              "Warnings as errors"       OFF )
OPTION( PLAYUVER_THREADED_MODULES    "Use threads for modules"  OFF )
OPTION( USE_QT4                      "Build with QT4"           OFF )

ADD_FEATURE_INFO(Fervor USE_FERVOR "Fervor lib includes check for updates feature.")
ADD_FEATURE_INFO(WErrors PLAYUVER_WERROR "Warnings as errors"  )

#####################################################################################
# Qt library
######################################################################################

IF( USE_QT4 )
  SET( DEPENDENCIES_STRING "${DEPENDENCIES_STRING}QT4" )
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

  SET( DEPENDENCIES_STRING "${DEPENDENCIES_STRING}QT5" )

  FIND_PACKAGE( Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport )
  FIND_PACKAGE( Qt5DBus )


  LIST(APPEND QT_LINKER_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::PrintSupport )
  LIST(APPEND QT_MODULES Core Gui Widgets )
  IF( ${Qt5DBus_FOUND} )
    SET( USE_QTDBUS ${Qt5DBus_FOUND} )
    LIST(APPEND QT_LINKER_LIBS Qt5::DBus )
    LIST(APPEND QT_MODULES DBus )
  ENDIF()

  INCLUDE_DIRECTORIES( ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}" )

ENDIF()

######################################################################################
# Dependencies
######################################################################################

OPTION( USE_OPENCV "Add OpenCV support" OFF )
IF( USE_OPENCV )
  FIND_PACKAGE( OpenCV )
  SET(USE_OPENCV ${OpenCV_FOUND})
ENDIF()
SET_PACKAGE_PROPERTIES(OpenCV PROPERTIES URL "http://opencv.willowgarage.com" DESCRIPTION "OpenCV library support in PlaYUVerFrame" TYPE OPTIONAL)

OPTION( USE_FFMPEG "Add FFMPEG support" OFF )
IF( USE_FFMPEG )
  FIND_PACKAGE( FFmpeg )
  SET(USE_FFMPEG ${FFMPEG_FOUND})
ENDIF()
SET_PACKAGE_PROPERTIES(FFmpeg PROPERTIES URL "http://ffmpeg.org/" DESCRIPTION "Libav library support in PlaYUVerStream" TYPE OPTIONAL)

IF( WIN32 )
  INCLUDE( cmake/Win32.cmake )
ENDIF()

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

IF( PLAYUVER_WERROR )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
ENDIF()

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2" )

ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( modules )
ADD_SUBDIRECTORY( app )




######################################################################################
# Final information dump
######################################################################################
MESSAGE("")
MESSAGE( STATUS "${PROJECT_NAME} INFORMATION"                      )
MESSAGE( STATUS "Version: "                 "${PLAYUVER_VERSION}"  )
MESSAGE( STATUS "Configuration:"                                   )
MESSAGE( STATUS "    Build type:"          "${CMAKE_BUILD_TYPE}"   )
MESSAGE( STATUS "    Build flags:"         "${CMAKE_CXX_FLAGS}"    )


FEATURE_SUMMARY(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
