#####################################################################################
# Pixfc-sse library cmake file
######################################################################################
# https://code.google.com/p/pixfc-sse/
# svn checkout http://pixfc-sse.googlecode.com/svn/trunk/ pixfc-sse
# Add to folder external

IF( USE_PIXFC )
  IF(EXISTS "${EXTERNAL_SRC}/pixfc-sse/build/src/libpixfc-sse.a")
    INCLUDE_DIRECTORIES( ${EXTERNAL_SRC}/pixfc-sse/include )
    LIST(APPEND LINKER_LIBS ${EXTERNAL_SRC}/pixfc-sse/build/src/libpixfc-sse.a )
    LIST(APPEND LINKER_LIBS rt)
  ELSE()
    SET( USE_PIXFC OFF )
  ENDIF()
ENDIF()