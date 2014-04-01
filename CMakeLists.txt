###
### CMakeLists for playuver
###

######################################################################################
# Project Definition
######################################################################################

cmake_minimum_required( VERSION 2.8.9 )

INCLUDE( cmake/CMakeMacros.cmake )

PROJECT( playuver )

SET( PROJECT_NAME playuver )

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

FILE( GLOB SOURCES_FILES   ${SRC_DIR}/app/*.cpp ${SRC_DIR}/lib/*.cpp )
FILE( GLOB FORMS_FILES     ${UI_DIR}/*.ui   )
FILE( GLOB RESOURCES_FILES ${RSC_DIR}/*.qrc )


######################################################################################
# Variables
######################################################################################
ADD_OPTION(CMAKE_BUILD_TYPE  "Build type"               "Debug" )
ADD_OPTION(USE_WERROR        "Warnings as errors"       OFF )
ADD_OPTION(USE_QT4           "Build with QT4"           OFF )
ADD_OPTION(USE_OPENCV        "Add OpenCV support"       OFF )
ADD_OPTION(USE_PIXFC         "Add PIXFC support"        OFF )
ADD_OPTION(USE_FFMPEG        "Add FFMPEG support"       OFF )

######################################################################################
# Dependencies
######################################################################################
INCLUDE( cmake/LibQT.cmake )

IF( USE_OPENCV )
    FIND_PACKAGE( OpenCV )
    IF( NOT OpenCV_FOUND)
        SET(USE_OPENCV FALSE)
    ELSE()
        LIST(APPEND LINKER_LIBS ${OpenCV_LIBRARIES} )
    ENDIF()
ENDIF()
INCLUDE( cmake/LibPixfc.cmake )
INCLUDE( cmake/LibFFmpeg.cmake )

######################################################################################
# PlaYUVer modules build
######################################################################################
SET( MODULES_DIR ${SRC_DIR}/modules )
INCLUDE_DIRECTORIES( ${MODULES_DIR} )
FILE( GLOB ModuleCMakeFiles cmake/Module*.cmake )
INCLUDE( ${ModuleCMakeFiles} )

######################################################################################
# Main config.h
######################################################################################
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

######################################################################################
# Target App
######################################################################################
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3" )
IF( USE_WERROR )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
ENDIF()


ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES_FILES} ${HEADERS_MOC_FILES} ${FORMS_HEADERS_FILES} ${RESOURCES_RCC_FILES} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PLAYUVER_LIBS}  ${MODULES_LIBS}  ${LINKER_LIBS} )

IF( NOT USE_QT4)
    QT5_USE_MODULES( ${PROJECT_NAME} ${CURR_QT_MODULES} )
ENDIF()

status("Configuration:")
status("    Build type:"           ${CMAKE_BUILD_TYPE} )
status("    Warnings as errors:" USE_WERROR       THEN YES ELSE NO)
status("Dependencies:")
status("    QT:"              USE_QT4             THEN "QT4" ELSE "QT5" )
status("    OpenCV:"          USE_OPENCV          THEN YES ELSE NO)
status("    FFMPEG:"          USE_FFMPEG          THEN YES ELSE NO)
status("    Pixfc-sse:"       USE_PIXFC           THEN YES ELSE NO)
