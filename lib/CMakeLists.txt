###
### CMakeLists for playuver lib component
###

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../ )

set(plaYUver_Lib_SRCS
    # Frame
    PlaYUVerFrame.cpp
    PlaYUVerFramePixelFormats.cpp
    #    PlaYUVerFrameStats.cpp
    # Stream
    PlaYUVerStream.cpp
    # Command Parser
    PlaYUVerCmdParser.cpp
    PlaYUVerProgramOptions.cpp
)

IF( USE_FFMPEG )
  LIST( APPEND plaYUver_Lib_SRCS LibAvContextHandle.cpp )
ENDIF()

IF( USE_OPENCV )
  LIST( APPEND plaYUver_Lib_SRCS LibOpenCVHandler.cpp )
ENDIF()


set(plaYUver_Lib_HEADERS
    PlaYUVerDefs.h
    PlaYUVerProgramOptions.h
    #   PlaYUVerFrameStats.h
    PlaYUVerFrame.h
    PlaYUVerStream.h
    PlaYUVerModuleIf.h
)

# OPTION( USE_PIXFC  "Add PIXFC support"  OFF )
# IF( USE_PIXFC )
#   IF(EXISTS "${EXTERNAL_SRC}/pixfc-sse/build/src/libpixfc-sse.a")
#     INCLUDE_DIRECTORIES( ${EXTERNAL_SRC}/pixfc-sse/include )
#     LIST(APPEND LINKER_LIBS ${EXTERNAL_SRC}/pixfc-sse/build/src/libpixfc-sse.a )
#     LIST(APPEND LINKER_LIBS rt)
#   ELSE()
#     SET( USE_PIXFC OFF )
#   ENDIF()
# ENDIF()

CONFIGURE_FILE( PlaYUVerConfig.cmake.in PlaYUVerConfig.cmake @ONLY)

IF( USE_STATIC )
  ADD_LIBRARY( PlaYUVerLib STATIC ${plaYUver_Lib_SRCS} )
ELSE()
  ADD_LIBRARY( PlaYUVerLib SHARED ${plaYUver_Lib_SRCS} )
ENDIF()

INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( PlaYUVerLib ${FFMPEG_LIBRARIES} ${OpenCV_LIBRARIES}  )

IF( NOT USE_STATIC )
  INSTALL(FILES ${plaYUver_Lib_HEADERS} DESTINATION include )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/PlaYUVerConfig.cmake DESTINATION share/PlaYUVer )
  INSTALL(TARGETS PlaYUVerLib DESTINATION lib )
ENDIF()
