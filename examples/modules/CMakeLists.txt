###
### CMakeLists for modules_example
###


CMAKE_MINIMUM_REQUIRED( VERSION 2.8.9 )

PROJECT( "modules_example" )

FIND_PACKAGE( PlaYUVer )

INCLUDE_DIRECTORIES( ${PLAYUVER_INCLUDE_DIRS} )

ADD_LIBRARY(FrameBinarization SHARED FrameBinarization_APIv1.cpp)
TARGET_LINK_LIBRARIES( FrameBinarization ${PLAYUVER_LINKER_LIBS}  )

ADD_LIBRARY(AbsoluteFrameDifference SHARED AbsoluteFrameDifference_APIv2.cpp)
TARGET_LINK_LIBRARIES( AbsoluteFrameDifference ${PLAYUVER_LINKER_LIBS}  )

