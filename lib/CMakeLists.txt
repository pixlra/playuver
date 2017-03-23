###
### CMakeLists for playuver lib component
###

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

set(PlaYUVer_Lib_SRCS
    # Frame
    PlaYUVerPixel.cpp
    PlaYUVerFrame.cpp
    PlaYUVerFramePixelFormats.cpp
    # Stream
    PlaYUVerStream.cpp
    StreamHandlerRaw.cpp
    StreamHandlerPortableMap.cpp
    # Options Parser
    PlaYUVerOptions.cpp
)

IF( USE_FFMPEG )
  LIST( APPEND PlaYUVer_Lib_SRCS StreamHandlerLibav.cpp )
ENDIF()

IF( USE_OPENCV )
  LIST( APPEND PlaYUVer_Lib_SRCS StreamHandlerOpenCV.cpp )
ENDIF()


set(PlaYUVer_Lib_HEADERS
    PlaYUVerDefs.h
    PlaYUVerPixel.h
    PlaYUVerFrame.h
    PlaYUVerStream.h
    PlaYUVerOptions.h
    PlaYUVerModuleIf.h
)


CONFIGURE_FILE( PlaYUVerConfig.cmake.in PlaYUVerConfig.cmake @ONLY)

IF( USE_STATIC )
  ADD_LIBRARY( PlaYUVerLib STATIC ${PlaYUVer_Lib_SRCS} )
ELSE()
  ADD_LIBRARY( PlaYUVerLib SHARED ${PlaYUVer_Lib_SRCS} )
ENDIF()

INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( PlaYUVerLib ${FFMPEG_LIBRARIES} ${OpenCV_LIBRARIES}  )

IF( NOT USE_STATIC )
  INSTALL(FILES ${PlaYUVer_Lib_HEADERS} DESTINATION include )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/PlaYUVerConfig.cmake DESTINATION share/playuver )
  INSTALL(TARGETS PlaYUVerLib DESTINATION lib )
ENDIF()
