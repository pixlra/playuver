###
### CMakeLists for playuver
###

cmake_minimum_required( VERSION 2.8.9 )

######################################################################################
# Project Definition
######################################################################################

set( PROJECT_NAME "playuver" CACHE INTERNAL "" FORCE )

PROJECT( ${PROJECT_NAME} )
SET( PLAYUVER_VERSION_MAJOR "0" )
SET( PLAYUVER_VERSION_MINOR "10" )
SET( PLAYUVER_VERSION_PATCH "0" )
set( PLAYUVER_VERSION "${PLAYUVER_VERSION_MAJOR}.${PLAYUVER_VERSION_MINOR}.${PLAYUVER_VERSION_PATCH}" CACHE INTERNAL "" FORCE )

######################################################################################
# Variables
######################################################################################

OPTION( PLAYUVER_BUILD_APP      "Build GUI Application"                 ON  )
OPTION( PLAYUVER_BUILD_TOOLS    "Build Command Line Application"        ON  )
OPTION( PLAYUVER_BUILD_EXAMPLES "Build Examples"                        OFF )
OPTION( PLAYUVER_BUILD_DOC      "Build Documentation"                   OFF )
OPTION( USE_SSE                 "Build with SSE support"                OFF )
OPTION( USE_FERVOR              "Add Fervor support"                    OFF )
OPTION( USE_WERROR              "Warnings as errors"                    OFF )
OPTION( USE_STATIC              "Use static libs"                       OFF )
OPTION( USE_QT4                 "Build with Qt4"                        OFF )
OPTION( USE_QTDBUS              "Build with QtDBus"                     ON  )
OPTION( USE_DYNLOAD             "Use dynamic load of modules"           ON  )
# To be switched on when releasing.
OPTION(RELEASE_BUILD "Remove Git revision from program version (use for stable releases)" ON)



# Get current version.
set(PLAYUVER_VERSION_STRING "${PLAYUVER_VERSION}")
if(NOT RELEASE_BUILD AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  # Probably a Git workspace; determine the revision.
  find_package(Git QUIET)
  if(GIT_FOUND)
    exec_program(${GIT_EXECUTABLE}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ARGS "describe --tags"
      OUTPUT_VARIABLE PLAYUVER_GIT_REVISION
      RETURN_VALUE TAG_RESULT
    )
    # git describe --tags failed; maybe the repository was checked with depth=1.
    if(NOT ${TAG_RESULT} EQUAL 0)
      exec_program(${GIT_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "describe --always"
        OUTPUT_VARIABLE PLAYUVER_GIT_REVISION
      )
    endif()
    message(STATUS "PlaYUVer Git revision: ${PLAYUVER_GIT_REVISION}")
    set(PLAYUVER_VERSION_STRING "${PLAYUVER_VERSION} (rev. ${PLAYUVER_GIT_REVISION})")
  else()
    message(STATUS "PlaYUVer Git revision could not be determined")
  endif()
endif()

SET(UPDATE_CHANNEL "stable" CACHE STRING "Update channel" )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/PlaYUVerUpdate.xml.in ${CMAKE_CURRENT_BINARY_DIR}/PlaYUVerUpdate-${UPDATE_CHANNEL}.xml )


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

######################################################################################
# Dependencies
######################################################################################

# find_package(Doxygen)
# IF(DOXYGEN_FOUND)
#   SET( PLAYUVER_BUILD_DOC ON )
# ENDIF(DOXYGEN_FOUND)
# SET_PACKAGE_PROPERTIES(Doxygen PROPERTIES URL "http://www.doxygen.org/" DESCRIPTION "Doxygen" TYPE OPTIONAL)

OPTION( USE_OPENCV "Add OpenCV support" ON )
SET( OPENCV_OPTFLOW_FOUND OFF )
IF( USE_OPENCV )
  FIND_PACKAGE( OpenCV )
  SET(USE_OPENCV ${OpenCV_FOUND})
  ## Check
  LIST(FIND OpenCV_LIB_COMPONENTS opencv_optflow OpenCV_optflow_FOUND)
ENDIF()
SET_PACKAGE_PROPERTIES(OpenCV PROPERTIES URL "http://opencv.willowgarage.com" DESCRIPTION "OpenCV library support in PlaYUVerFrame" TYPE OPTIONAL)

OPTION( USE_FFMPEG "Add FFMPEG support" ON )
IF( USE_FFMPEG )
  FIND_PACKAGE( FFmpeg )
  SET(USE_FFMPEG ${FFMPEG_FOUND})
ENDIF()
SET_PACKAGE_PROPERTIES(FFmpeg PROPERTIES URL "http://ffmpeg.org/" DESCRIPTION "Libav library support in PlaYUVerStream" TYPE OPTIONAL)

ADD_FEATURE_INFO(SSE USE_SSE "SSE instructions support"  )
ADD_FEATURE_INFO(WErrors USE_WERROR "Warnings as errors"  )


IF( WIN32 )
  SET( USE_STATIC ON )
  INCLUDE( cmake/Win32.cmake )
ENDIF()

######################################################################################
# Configure targets
######################################################################################

if(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=gnu++0x)
endif()

IF (NOT CMAKE_BUILD_TYPE )
    MESSAGE(STATUS "No build type selected, default to Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )

IF( USE_WERROR )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
ENDIF()

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2" )


ADD_FEATURE_INFO(PlaYUVerApp     PLAYUVER_BUILD_APP    "Build Graphical Interface")
ADD_FEATURE_INFO(PlaYUVerTools   PLAYUVER_BUILD_TOOLS  "Build Command line tool"  )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( modules )

IF( ${PLAYUVER_BUILD_TOOLS} )
  ADD_SUBDIRECTORY( tools )
ENDIF()

IF( ${PLAYUVER_BUILD_APP} )
  ADD_SUBDIRECTORY( app )
ENDIF()

# add a target to generate API documentation with Doxygen
# IF(PLAYUVER_BUILD_DOC)
#   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#   add_custom_target(doc   ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating API documentation with Doxygen" VERBATIM )
# ENDIF(PLAYUVER_BUILD_DOC)


######################################################################################
# Final information dump
######################################################################################
MESSAGE("")
MESSAGE( STATUS "${PROJECT_NAME} INFORMATION"                     )
MESSAGE( STATUS "Version: "          "${PLAYUVER_VERSION_STRING}" )
MESSAGE( STATUS "Configuration:"                                  )
MESSAGE( STATUS "    Static libs: "         "${USE_STATIC}" )
MESSAGE( STATUS "    Build type: "          "${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "    Build flags: "         "${CMAKE_CXX_FLAGS}"  )

FEATURE_SUMMARY(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
