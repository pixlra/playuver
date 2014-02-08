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
SET( SRC_DIR source )
SET( INC_DIR source )
SET( LIB_DIR ${SRC_DIR}/lib )
SET( RSC_DIR resources )
SET( UI_DIR  ui )
SET( EXTERNAL_SRC external_lib )

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )
# Instruct CMake to run moc automatically when needed.
SET( CMAKE_AUTOMOC ON )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${INC_DIR} ) 

FILE( GLOB SOURCES_FILES   ${SRC_DIR}/*.cpp ) 
FILE( GLOB HEADERS_FILES   ${INC_DIR}/*.h   ) 
FILE( GLOB FORMS_FILES     ${UI_DIR}/*.ui   )
FILE( GLOB RESOURCES_FILES ${RSC_DIR}/*.qrc )


######################################################################################
# Variables
######################################################################################
ADD_OPTION(CMAKE_BUILD_TYPE  "Build type"               Debug )
ADD_OPTION(USE_QT4           "Build with QT4"           OFF )
ADD_OPTION(USE_OPENCV        "Add OpenCV support"       OFF )
ADD_OPTION(USE_FFMPEG        "Add FFMPEG support"       OFF )


######################################################################################
# Dependencies
######################################################################################
INCLUDE( cmake/LibQT.cmake )

INCLUDE_DIRECTORIES( ${EXTERNAL_SRC}/scode )
ADD_LIBRARY( Scode ${EXTERNAL_SRC}/scode/viewarea.cpp ${EXTERNAL_SRC}/scode/gridmanager.cpp ${EXTERNAL_SRC}/scode/scosettings.cpp )
SET( LINKER_LIBS ${LINKER_LIBS} Scode )


IF( USE_OPENCV )
    FIND_PACKAGE( OpenCV )
    IF( NOT OpenCV_FOUND)
        SET(USE_OPENCV FALSE)
    ELSE()
        SET( LINKER_LIBS ${LINKER_LIBS} ${OpenCV_LIBRARIES} )
    ENDIF()
ENDIF()

INCLUDE( cmake/LibFFmpeg.cmake )

######################################################################################
# Create config.h
######################################################################################
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

######################################################################################
# Target App
######################################################################################
ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES_FILES} ${HEADERS_MOC_FILES} ${FORMS_HEADERS_FILES} ${RESOURCES_RCC_FILES} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${LINKER_LIBS} )

IF( NOT USE_QT4)
    QT5_USE_MODULES( ${PROJECT_NAME} ${CURR_QT_MODULES} )
ENDIF()


status("Using third-party libraries:")
status("    Use OpenCV:"      USE_OPENCV          THEN YES ELSE NO)
status("    FFMPEG:"          USE_FFMPEG          THEN YES ELSE NO)
