###
### CMakeLists for playuver
###

######################################################################################
# Project Definition
######################################################################################

cmake_minimum_required( VERSION 2.8.9 )

PROJECT( playuver )

SET( PROJECT_NAME playuver )

# Project directories 
SET( SRC_DIR source )
SET( INC_DIR source )
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
# Macros
######################################################################################
macro(ADD_OPTION variable description value)
  set(__value ${value})
  set(__condition "")
  set(__varname "__value")
  foreach(arg ${ARGN})
    if(arg STREQUAL "IF" OR arg STREQUAL "if")
      set(__varname "__condition")
    else()
      list(APPEND ${__varname} ${arg})
    endif()
  endforeach()
  unset(__varname)
  if("${__condition}" STREQUAL "")
    set(__condition 2 GREATER 1)
  endif()

  if(${__condition})
    if("${__value}" MATCHES ";")
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    elseif(DEFINED ${__value})
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    else()
      option(${variable} "${description}" ${__value})
    endif()
  else()
    unset(${variable} CACHE)
  endif()
  unset(__condition)
  unset(__value)
endmacro()

# Status report function.
# Automatically align right column and selects text based on condition.
# Usage:
#   status(<text>)
#   status(<heading> <value1> [<value2> ...])
#   status(<heading> <condition> THEN <text for TRUE> ELSE <text for FALSE> )
function(status text)
  set(status_cond)
  set(status_then)
  set(status_else)

  set(status_current_name "cond")
  foreach(arg ${ARGN})
    if(arg STREQUAL "THEN")
      set(status_current_name "then")
    elseif(arg STREQUAL "ELSE")
      set(status_current_name "else")
    else()
      list(APPEND status_${status_current_name} ${arg})
    endif()
  endforeach()

  if(DEFINED status_cond)
    set(status_placeholder_length 32)
    string(RANDOM LENGTH ${status_placeholder_length} ALPHABET " " status_placeholder)
    string(LENGTH "${text}" status_text_length)
    if(status_text_length LESS status_placeholder_length)
      string(SUBSTRING "${text}${status_placeholder}" 0 ${status_placeholder_length} status_text)
    elseif(DEFINED status_then OR DEFINED status_else)
      message(STATUS ${text})
      set(status_text "${status_placeholder}")
    else()
      set(status_text "${text}")
    endif()

    if(DEFINED status_then OR DEFINED status_else)
      if(${status_cond})
        string(REPLACE ";" " " status_then "${status_then}")
        string(REGEX REPLACE "^[ \t]+" "" status_then "${status_then}")
        message(STATUS "${status_text} ${status_then}")
      else()
        string(REPLACE ";" " " status_else "${status_else}")
        string(REGEX REPLACE "^[ \t]+" "" status_else "${status_else}")
        message(STATUS "${status_text} ${status_else}")
      endif()
    else()
      string(REPLACE ";" " " status_cond "${status_cond}")
      string(REGEX REPLACE "^[ \t]+" "" status_cond "${status_cond}")
      message(STATUS "${status_text} ${status_cond}")
    endif()
  else()
    message(STATUS "${text}")
  endif()
endfunction()


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
IF( USE_OPENCV )
    FIND_PACKAGE( OpenCV )
    IF( NOT OpenCV_FOUND)
        SET(USE_OPENCV FALSE)
    ELSE()
        SET( LINKER_LIBS ${LINKER_LIBS} ${OpenCV_LIBRARIES} )
    ENDIF()
ENDIF()

IF( USE_FFMPEG )
    FIND_PACKAGE( FFmpeg )
    IF( NOT FFMPEG_FOUND )
        SET( USE_FFMPEG FALSE )
    ELSE()
        SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS )
    ENDIF()
ENDIF()

######################################################################################
# Qt library
######################################################################################
IF( USE_QT4 )
    FIND_PACKAGE( Qt4 REQUIRED QtCore QtGui )
    IF( NOT QT4_FOUND)
        SET(USE_QT4 FALSE)
    ENDIF()
    INCLUDE( ${QT_USE_FILE} )
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    
    QT4_WRAP_UI( FORMS_HEADERS_FILES ${FORMS_FILES} )
    QT4_ADD_RESOURCES( RESOURCES_RCC_FILES ${RESOURCES_FILES} )
    
    SET( LINKER_LIBS ${LINKER_LIBS} ${QT_LIBRARIES} )
ENDIF()

IF( NOT USE_QT4 )
    FIND_PACKAGE( Qt5Core REQUIRED )
    FIND_PACKAGE( Qt5Gui REQUIRED )
    FIND_PACKAGE( Qt5Widgets REQUIRED )
    
    INCLUDE_DIRECTORIES( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} )
    # QT5_WRAP_CPP( HEADERS_MOC_FILES ${HEADERS_FILES} )
    QT5_WRAP_UI( FORMS_HEADERS_FILES ${FORMS_FILES} )
    QT5_ADD_RESOURCES( RESOURCES_RCC_FILES ${RESOURCES_FILES} )
    
    # Add compiler flags for building executables (-fPIE)
    SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} )
    
    SET( LINKER_LIBS ${LINKER_LIBS} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} )
    
    SET( CURR_QT_MODULES Core Gui Widgets )
ENDIF()

######################################################################################
# External Libs
######################################################################################

INCLUDE_DIRECTORIES( ${EXTERNAL_SRC}/scode )
ADD_LIBRARY( Scode ${EXTERNAL_SRC}/scode/viewarea.cpp ${EXTERNAL_SRC}/scode/gridmanager.cpp ${EXTERNAL_SRC}/scode/scosettings.cpp )

SET( LINKER_LIBS ${LINKER_LIBS} Scode )
######################################################################################
# Create config.h
######################################################################################
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )


######################################################################################
# Target App
######################################################################################

ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES_FILES} ${HEADERS_MOC_FILES} ${FORMS_HEADERS_FILES} ${RESOURCES_RCC_FILES} )

IF( USE_FFMPEG )
    INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS})
    SET( LINKER_LIBS ${LINKER_LIBS} ${FFMPEG_LIBRARIES} )
ENDIF()
message( STATUS ${LINKER_LIBS} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${LINKER_LIBS} )

IF( NOT USE_QT4)
    QT5_USE_MODULES( ${PROJECT_NAME} ${CURR_QT_MODULES} )
ENDIF()


status("Using third-party libraries:")
status("    Use OpenCV:"      USE_OPENCV          THEN YES ELSE NO)
status("    FFMPEG:"          USE_FFMPEG          THEN YES ELSE NO)
