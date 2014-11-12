###
### CMakeLists for playuver lib component
###

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_OPTION(USE_OPENCV                   "Add OpenCV support"       OFF )
ADD_OPTION(USE_FFMPEG                   "Add FFMPEG support"       OFF )
ADD_OPTION(USE_PIXFC                    "Add PIXFC support"        OFF )

set(plaYUver_Lib_SRCS
    # Frame
    PlaYUVerFrame.cpp
    PlaYUVerFramePixelFormats.cpp
    # Stream
    PlaYUVerStream.cpp
)

set(plaYUver_Lib_HEADERS
    PlaYUVerFrame.h
    PlaYUVerStream.h
)

INCLUDE( LibPixfc.cmake )

CONFIGURE_FILE( PlaYUVerConfig.cmake.in ${CMAKE_BINARY_DIR}/PlaYUVerConfig.cmake @ONLY)

ADD_LIBRARY( PlaYUVerLib ${plaYUver_Lib_SRCS} )
TARGET_LINK_LIBRARIES( PlaYUVerLib ${LINKER_LIBS} )

INSTALL(FILES ${plaYUver_Lib_HEADERS} DESTINATION include )
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libPlaYUVerLib.a DESTINATION lib )
