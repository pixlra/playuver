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
SET( PLAYUVER_VERSION_PATCH "1" )
SET( PLAYUVER_VERSION "${PLAYUVER_VERSION_MAJOR}.${PLAYUVER_VERSION_MINOR}.${PLAYUVER_VERSION_PATCH}")

######################################################################################
# CMake Defin1itions
######################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
INCLUDE(FeatureSummary)

# must go before the project command
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
ENDIF()
SET( CMAKE_CXX_FLAGS "" )

cmake_policy(SET CMP0043 NEW)

######################################################################################
# Variables
######################################################################################

OPTION( PLAYUVER_BUILD_APP      "Build GUI Application"                ON )
OPTION( PLAYUVER_BUILD_TOOLS    "Build Command Line Application"       ON )

OPTION( USE_FERVOR                   "Add Fervor support"       OFF )
OPTION( USE_WERROR                   "Warnings as errors"       OFF )

######################################################################################
# Dependencies
######################################################################################

FIND_PACKAGE(Boost COMPONENTS program_options )
SET_PACKAGE_PROPERTIES(Boost PROPERTIES URL "http://www.boost.org" DESCRIPTION "Boost library support" TYPE REQUIRED)

OPTION( USE_OPENCV "Add OpenCV support" ON )
IF( USE_OPENCV )
  FIND_PACKAGE( OpenCV )
  SET(USE_OPENCV ${OpenCV_FOUND})
ENDIF()
SET_PACKAGE_PROPERTIES(OpenCV PROPERTIES URL "http://opencv.willowgarage.com" DESCRIPTION "OpenCV library support in PlaYUVerFrame" TYPE OPTIONAL)

OPTION( USE_FFMPEG "Add FFMPEG support" ON )
IF( USE_FFMPEG )
  FIND_PACKAGE( FFmpeg )
  SET(USE_FFMPEG ${FFMPEG_FOUND})
ENDIF()
SET_PACKAGE_PROPERTIES(FFmpeg PROPERTIES URL "http://ffmpeg.org/" DESCRIPTION "Libav library support in PlaYUVerStream" TYPE OPTIONAL)

ADD_FEATURE_INFO(Fervor USE_FERVOR "Fervor lib includes check for updates feature.")
ADD_FEATURE_INFO(WErrors PLAYUVER_WERROR "Warnings as errors"  )

IF( WIN32 )
  INCLUDE( cmake/Win32.cmake )
ENDIF()

######################################################################################
# Configuration
######################################################################################
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

######################################################################################
# Target App
######################################################################################
IF (NOT CMAKE_BUILD_TYPE )
    MESSAGE(STATUS "No build type selected, default to Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
#
# IF( PLAYUVER_WERROR )
#   SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
# ENDIF()

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2" )


ADD_FEATURE_INFO(PlaYUVerApp     PLAYUVER_BUILD_TOOLS    "Build Graphical Interface")
ADD_FEATURE_INFO(PlaYUVerTools   PLAYUVER_BUILD_APP      "Build Command line tool"  )

ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( modules )

IF( PLAYUVER_BUILD_TOOLS )
  ADD_SUBDIRECTORY( tools )
ENDIF()

IF( PLAYUVER_BUILD_APP )
  ADD_SUBDIRECTORY( app )
ENDIF()



######################################################################################
# Final information dump
######################################################################################
MESSAGE("")
MESSAGE( STATUS "${PROJECT_NAME} INFORMATION"                      )
MESSAGE( STATUS "Version: "                 "${PLAYUVER_VERSION}"  )
MESSAGE( STATUS "Configuration:"                                   )
MESSAGE( STATUS "    Build type: "          "${CMAKE_BUILD_TYPE}"   )
MESSAGE( STATUS "    Build flags:"          "${CMAKE_CXX_FLAGS}"    )


FEATURE_SUMMARY(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
