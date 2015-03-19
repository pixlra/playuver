###
### CMakeLists for playuver lib component
###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../ )

#
# Add boost support
#
IF( NOT ${Boost_FOUND} )
  MESSAGE("Using internal boost program options lib")
  ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/external/boost_program_options )
ENDIF()
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

set(plaYUver_Lib_SRCS
    # Frame
    PlaYUVerFrame.cpp
    PlaYUVerFramePixelFormats.cpp
    PlaYUVerFrameStats.cpp
    # Stream
    PlaYUVerStream.cpp
    # Command Line Parser
    PlaYUVerCmdParser.cpp
)

IF( USE_FFMPEG )
  LIST( APPEND plaYUver_Lib_SRCS LibAvContextHandle.cpp )
ENDIF()

IF( USE_OPENCV )
  LIST( APPEND plaYUver_Lib_SRCS LibOpenCVHandler.cpp )
ENDIF()


set(plaYUver_Lib_HEADERS
    PlaYUVerFrame.h
    PlaYUVerStream.h
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

CONFIGURE_FILE( PlaYUVerConfig.cmake.in ${CMAKE_BINARY_DIR}/PlaYUVerConfig.cmake @ONLY)

ADD_LIBRARY( PlaYUVerLib STATIC ${plaYUver_Lib_SRCS} )

INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( PlaYUVerLib ${FFMPEG_LIBRARIES} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} )

INSTALL(FILES ${plaYUver_Lib_HEADERS} DESTINATION include )
# INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libPlaYUVerLib${CMAKE_STATIC_LIBRARY_SUFFIX} DESTINATION lib )
INSTALL(TARGETS PlaYUVerLib DESTINATION lib )
