#####################################################################################
# Qt library cmake file
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
    
    LIST(APPEND LINKER_LIBS ${QT_LIBRARIES} )
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
    
    LIST(APPEND LINKER_LIBS ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} )
    SET( CURR_QT_MODULES Core Gui Widgets )
ENDIF()
