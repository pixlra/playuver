###
### CMakeLists for playuver
###

cmake_minimum_required( VERSION 2.8.9 )

######################################################################################
# Project Definition
######################################################################################

IF(UNIX)
  EXECUTE_PROCESS(COMMAND "date" "+%Y%m%d" OUTPUT_VARIABLE DAY)
  string(STRIP ${DAY} DAY)
ENDIF(UNIX)

SET( PROJECT_NAME "playuver" )

PROJECT( ${PROJECT_NAME} )
INCLUDE( PlaYUVerVersion.cmake )

INCLUDE( cmake/CMakeMacros.cmake )

# Project directories 
SET( SRC_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/source )
SET( LIB_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/source/lib )
SET( RSC_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/resources )
SET( UI_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/ui )
SET( EXTERNAL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/external )

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )
# Instruct CMake to run moc automatically when needed.
SET( CMAKE_AUTOMOC ON )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_DIR}/app ${SRC_DIR}/lib ) 

FILE( GLOB SOURCES_FILES   ${SRC_DIR}/app/*.cpp )
FILE( GLOB FORMS_FILES     ${UI_DIR}/*.ui   )
FILE( GLOB RESOURCES_FILES ${RSC_DIR}/*.qrc ${RSC_DIR}/*.rc )

# must go before the project command
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
ENDIF()

SET( CMAKE_CXX_FLAGS "" )

ADD_DEFINITIONS( "-DPLAYUVER" )

######################################################################################
# Variables
######################################################################################
ADD_OPTION(PLAYUVER_LONGVERSIONNAME     "Use long version name in deb" OFF )
ADD_OPTION(PLAYUVER_WERROR              "Warnings as errors"       OFF )
ADD_OPTION(PLAYUVER_THREADED_MODULES    "Warnings as errors"       ON  )
ADD_OPTION(PLAYUVER_INSTALL_LIBS        "Install PlaYUVer Lib"     ON  )
ADD_OPTION(USE_QT4                      "Build with QT4"           OFF )
ADD_OPTION(USE_OPENCV                   "Add OpenCV support"       OFF )
ADD_OPTION(USE_FFMPEG                   "Add FFMPEG support"       OFF )
ADD_OPTION(USE_FERVOR                   "Add Fervor support"       OFF )
ADD_OPTION(USE_PIXFC                    "Add PIXFC support"        OFF )

######################################################################################
# PlaYUVer lib
######################################################################################
FILE( GLOB PLAYUVER_LIB_SOURCES_FILES ${SRC_DIR}/lib/*.cpp )
INCLUDE_DIRECTORIES( ${SRC_DIR}/lib )
ADD_LIBRARY( PlaYUVerLib ${PLAYUVER_LIB_SOURCES_FILES} )
  
######################################################################################
# Dependencies
######################################################################################
INCLUDE( cmake/LibFervor.cmake )
INCLUDE( cmake/LibQT.cmake )
INCLUDE( cmake/LibOpenCV.cmake )
INCLUDE( cmake/LibFFmpeg.cmake )
INCLUDE( cmake/LibPixfc.cmake )

######################################################################################
# PlaYUVer modules build
######################################################################################
SET( MODULES_DIR ${SRC_DIR}/modules )
INCLUDE_DIRECTORIES( ${MODULES_DIR} )
FILE( GLOB ModuleCMakeFiles cmake/Module*.cmake )

file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ModulesListMacro.h  "// This files defines the macro to \n"
                                                            "// define a list of modules\n"
                                                            "#ifndef __MODULESLISTMACRO_H__\n#define __MODULESLISTMACRO_H__\n"
                                                            "#define REGISTER_ALL_MODULES ") 

file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "// This files add the header files of each module\n"
                                                              "#ifndef __MODULESLISTHEADER_H__\n#define __MODULESLISTHEADER_H__\n" )
INCLUDE( ${ModuleCMakeFiles} )

file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListMacro.h    "\n#endif // __MODULESLISTMACRO_H__\n" )
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#endif // __MODULESLISTHEADER_H__\n" )

######################################################################################
# Configuration
######################################################################################
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlaYUVerConfig.cmake.in ${CMAKE_BINARY_DIR}/PlaYUVerConfig.cmake @ONLY)

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

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES_FILES} ${HEADERS_MOC_FILES} ${FORMS_HEADERS_FILES} ${RESOURCES_RCC_FILES} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PlaYUVerLib ${MODULES_LIBS}  ${LINKER_LIBS} )

IF( NOT USE_QT4 )
  QT5_USE_MODULES( ${PROJECT_NAME} ${QT_MODULES} )
ENDIF()

INCLUDE(cmake/Install.cmake)

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
status("Features:")
status("    Modules List:"        "${MODULES_LIST}" )
