###
### CMakeLists for calyp lib component
###

INCLUDE(GNUInstallDirs)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

set(Calyp_Lib_SRCS
    LibMemory.h
    CalypLib.h
    CalypDefs.h
    # Frame
    CalypFrame.h
    CalypFrame.cpp
    CalypPixel.cpp
    PixelFormats.h
    PixelFormats.cpp
    # Stream
    CalypStream.h
    CalypStream.cpp
    CalypStreamHandlerIf.h
    StreamHandlerRaw.h
    StreamHandlerRaw.cpp
    StreamHandlerPortableMap.h
    StreamHandlerPortableMap.cpp
    # Options Parser
    CalypOptions.h
    CalypOptions.cpp
    # Modules
    CalypModuleIf.h
)

LIST(APPEND CMAKE_CFG_LINKER_LIBSS ${PROJECT_LIBRARY} )
LIST(APPEND CMAKE_CFG_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR} )


IF( USE_FFMPEG )
  LIST( APPEND Calyp_Lib_SRCS StreamHandlerLibav.h )
  LIST( APPEND Calyp_Lib_SRCS StreamHandlerLibav.cpp )

  LIST(APPEND CMAKE_CFG_LINKER_LIBSS ${FFMPEG_LIBRARIES} )
  LIST(APPEND CMAKE_CFG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS} )
ENDIF()

IF( USE_OPENCV )
  LIST( APPEND Calyp_Lib_SRCS StreamHandlerOpenCV.h )
  LIST( APPEND Calyp_Lib_SRCS StreamHandlerOpenCV.cpp )

  LIST(APPEND CMAKE_CFG_LINKER_LIBSS ${OpenCV_LIBRARIES} )
  LIST(APPEND CMAKE_CFG_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} )
ENDIF()

set(Calyp_Lib_HEADERS
    CalypDefs.h
    CalypFrame.h
    CalypStream.h
    CalypOptions.h
    CalypModuleIf.h
)

CONFIGURE_FILE( CalypConfig.cmake.in CalypConfig.cmake @ONLY)

INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )

IF( USE_STATIC )
  ADD_LIBRARY( ${PROJECT_LIBRARY} STATIC ${Calyp_Lib_SRCS} )
ELSE()
  ADD_LIBRARY( ${PROJECT_LIBRARY} SHARED ${Calyp_Lib_SRCS} )
ENDIF()


TARGET_LINK_LIBRARIES( ${PROJECT_LIBRARY}
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBRARIES}
)

SET_TARGET_PROPERTIES( ${PROJECT_LIBRARY}
   PROPERTIES VERSION ${CALYP_VERSION}
   SOVERSION ${CALYP_VERSION_MAJOR}
)

IF( NOT USE_STATIC )
  INSTALL( TARGETS ${PROJECT_LIBRARY}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  )
  INSTALL(FILES ${Calyp_Lib_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CalypConfig.cmake DESTINATION share/calyp )
ENDIF()
