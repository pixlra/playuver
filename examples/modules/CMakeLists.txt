###
### CMakeLists for modules_example
###


CMAKE_MINIMUM_REQUIRED( VERSION 2.8.9 )

PROJECT( "modules_example" )

# FIND_PACKAGE( Calyp )

INCLUDE_DIRECTORIES( ${CALYP_INCLUDE_DIRS} )

ADD_LIBRARY(FrameBinarization SHARED FrameBinarization_APIv1.cpp)
TARGET_LINK_LIBRARIES( FrameBinarization ${CALYP_LINKER_LIBS}  )

ADD_LIBRARY(AbsoluteFrameDifferenceExample SHARED AbsoluteFrameDifferenceExample.cpp)
TARGET_LINK_LIBRARIES( AbsoluteFrameDifferenceExample ${CALYP_LINKER_LIBS}  )

