###
### CMakeLists for playuver
###

######################################################################################
# Project Definition
######################################################################################

cmake_minimum_required( VERSION 2.8.9 )

PROJECT( playuver )

SET( PROJECT_NAME playuver )

SET( CMAKE_BUILD_TYPE Debug )

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


######################################################################################
# Dependencies
######################################################################################
#FIND_PACKAGE( OpenCV REQUIRED   )


######################################################################################
# Qt5 library
######################################################################################
FIND_PACKAGE( Qt5Core           )
FIND_PACKAGE( Qt5Gui            )
FIND_PACKAGE( Qt5Widgets        )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${INC_DIR}  ) 

FILE( GLOB SOURCES_FILES   ${SRC_DIR}/*.cpp ) 
FILE( GLOB HEADERS_FILES   ${INC_DIR}/*.h   ) 
FILE( GLOB FORMS_FILES     ${UI_DIR}/*.ui   )
FILE( GLOB RESOURCES_FILES ${RSC_DIR}/*.qrc )

INCLUDE_DIRECTORIES( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})

# QT5_WRAP_CPP( HEADERS_MOC_FILES ${HEADERS_FILES} )
QT5_WRAP_UI( FORMS_HEADERS_FILES ${FORMS_FILES} )
QT5_ADD_RESOURCES( RESOURCES_RCC_FILES ${RESOURCES_FILES} )

SET( QT_USE_QTOPENGL TRUE )

# Add compiler flags for building executables (-fPIE)
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}" )

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
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${LIST_EXTERNAL_LIBS} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} )
QT5_USE_MODULES( ${PROJECT_NAME} Core Gui Widgets )



