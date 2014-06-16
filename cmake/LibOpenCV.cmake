#####################################################################################
# OpenCV library cmake file
######################################################################################

IF( USE_OPENCV )
  FIND_PACKAGE( OpenCV )
  IF( NOT OpenCV_FOUND)
      SET(USE_OPENCV FALSE)
  ELSE()
      LIST(APPEND LINKER_LIBS ${OpenCV_LIBRARIES} )
  ENDIF()
ENDIF()