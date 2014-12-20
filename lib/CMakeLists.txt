###
### CMakeLists for playuver lib component
###

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

set(plaYUver_Lib_SRCS
    # Frame
    PlaYUVerFrame.cpp
    PlaYUVerFramePixelFormats.cpp
    PlaYUVerFrameStats.cpp
    # Stream
    PlaYUVerStream.cpp
)

IF( USE_FFMPEG )
  LIST( APPEND plaYUver_Lib_SRCS LibAvContextHandle.cpp )
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
TARGET_LINK_LIBRARIES( PlaYUVerLib ${FFMPEG_LIBRARIES} ${OpenCV_LIBRARIES} )

INSTALL(FILES ${plaYUver_Lib_HEADERS} DESTINATION include )
# INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libPlaYUVerLib${CMAKE_STATIC_LIBRARY_SUFFIX} DESTINATION lib )
INSTALL(TARGETS PlaYUVerLib DESTINATION lib )
