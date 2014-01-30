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

# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )
# Instruct CMake to run moc automatically when needed.
SET( CMAKE_AUTOMOC ON )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${INC_DIR}  ) 

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

######################################################################################
# Variables
######################################################################################
ADD_OPTION(CMAKE_BUILD_TYPE  "Build type"               Debug )
ADD_OPTION(USE_QT4           "Build with QT4"           OFF )
ADD_OPTION(USE_OPENCV        "Add OpenCV support"       OFF )

######################################################################################
# Dependencies
######################################################################################
IF(HAVE_OpenCV)
  FIND_PACKAGE( OpenCV REQUIRED   )
  #CMAKEDEFINE HAVE_MALLOC_H 1
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
    SET( CURR_QT_LINKER_LIBS ${QT_LIBRARIES} )
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
    
    SET( CURR_QT_LINKER_LIBS ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} )
    
    SET( CURR_QT_MODULES Core Gui Widgets )
ENDIF()


######################################################################################
# External Libs
######################################################################################

INCLUDE_DIRECTORIES( ${EXTERNAL_SRC}/scode )
ADD_LIBRARY( Scode ${EXTERNAL_SRC}/scode/viewarea.cpp ${EXTERNAL_SRC}/scode/gridmanager.cpp ${EXTERNAL_SRC}/scode/scosettings.cpp )


SET( LIST_EXTERNAL_LIBS Scode )

######################################################################################
# Target App
######################################################################################
ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES_FILES} ${HEADERS_MOC_FILES} ${FORMS_HEADERS_FILES} ${RESOURCES_RCC_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${LIST_EXTERNAL_LIBS} ${CURR_QT_LINKER_LIBS} )

IF( NOT USE_QT4)
    QT5_USE_MODULES( ${PROJECT_NAME} ${CURR_QT_MODULES} )
ENDIF()


