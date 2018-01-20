###
### CMakeLists for playuver lib component
###

INCLUDE(GNUInstallDirs)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

set(PlaYUVer_Lib_SRCS
    LibMemory.h
    PlaYUVerLib.h
    # Frame
    PlaYUVerPixel.cpp
    PlaYUVerFrame.cpp
    PlaYUVerFramePixelFormats.cpp
    # Stream
    PlaYUVerStreamHandlerIf.h
    PlaYUVerStream.cpp
    StreamHandlerRaw.cpp
    StreamHandlerPortableMap.cpp
    # Options Parser
    PlaYUVerOptions.cpp
    # Modules
    PlaYUVerModuleIf.h
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

INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )

IF( USE_STATIC )
  ADD_LIBRARY( PlaYUVerLib STATIC ${PlaYUVer_Lib_SRCS} )
ELSE()
  ADD_LIBRARY( PlaYUVerLib SHARED ${PlaYUVer_Lib_SRCS} )
ENDIF()


TARGET_LINK_LIBRARIES( PlaYUVerLib
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBRARIES}
)

SET_TARGET_PROPERTIES( PlaYUVerLib
   PROPERTIES VERSION ${PLAYUVER_VERSION}
   SOVERSION ${PLAYUVER_VERSION_MAJOR}
)

IF( NOT USE_STATIC )
  INSTALL( TARGETS PlaYUVerLib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  )
  INSTALL(FILES ${PlaYUVer_Lib_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/PlaYUVerConfig.cmake DESTINATION share/playuver )
ENDIF()
