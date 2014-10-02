#####################################################################################
# Pixfc-sse library cmake file
######################################################################################
# https://github.com/pixlra/fervor
# git clone https://github.com/pixlra/fervor.git source/lib

SET( FERVOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/lib/fervor )

IF( USE_FERVOR )
  IF(EXISTS ${FERVOR_DIR} )
    SET( FERVOR_APP_NAME ${PROJECT_NAME} ) 
    SET( FERVOR_APP_VERSION ${PLAYUVER_VERSION} )
    ADD_SUBDIRECTORY( ${FERVOR_DIR} )
    INCLUDE_DIRECTORIES( ${FERVOR_DIR} )
    LIST(APPEND LINKER_LIBS "Fervor" )
  ELSE()
    MESSAGE( "Fervor lib source code is not find in ${FERVOR_DIR}... Disabling it!" )
    SET( USE_FERVOR False )
  ENDIF()
ENDIF()