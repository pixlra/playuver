###
### CMakeLists for playuver modules component
###

OPTION( MODULE_THREADED_MODULES "Use threads for modules"  OFF )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set(plaYUver_Mod_SRCS
  PlaYUVerModuleFactory.cpp
)

#
# Modules List
#
ADD_MODULE( FilterComponentLuma "FilterComponent" )
ADD_MODULE( FilterComponentChromaU "FilterComponent" )
ADD_MODULE( FilterComponentChromaV "FilterComponent" )
ADD_MODULE( FrameDifference "FrameDifference" )
ADD_MODULE( FrameMask "FrameMask" )
ADD_MODULE( AbsoluteFrameDifference "AbsoluteFrameDifference" )
ADD_MODULE( FrameBinarization "FrameBinarization" )
ADD_MODULE( FrameShift "FrameShift" )
ADD_MODULE( SetChromaHalfScale "SetChromaHalfScale" )
ADD_MODULE( EightBitsSampling "EightBitsSampling" )
ADD_MODULE( FrameCrop "FrameCrop" )
ADD_MODULE( LumaAverage "LumaAverage" )
ADD_MODULE( WeightedPSNR "WeightedPSNR" )
IF( OpenCV_optflow_FOUND )
  ADD_MODULE_OPENCV( OpticalFlowDualTVL1 "OpticalFlow" 3.1.0 )
  ADD_MODULE_OPENCV( OpticalFlowSparseToDense "OpticalFlow" 3.1.0 )
  ADD_MODULE_OPENCV( OpticalFlowFarneback "OpticalFlow" 3.1.0 )
ELSE()
  ADD_MODULE_OPENCV( OpticalFlowDualTVL1 "OpticalFlowDualTVL1" 2.4.13.0 )
ENDIF()
ADD_MODULE_OPENCV( DisparityStereoBM "DisparityStereoBM" 2.4.8 )
ADD_MODULE_OPENCV( DisparityStereoSGBM "DisparityStereoSGBM" 2.4.8 )
# ADD_MODULE_OPENCV( DisparityStereoVar "DisparityStereoVar" )


CREATE_MODULE_MACROS()
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
ADD_LIBRARY( PlaYUVerModules ${plaYUver_Mod_SRCS} )
TARGET_LINK_LIBRARIES( PlaYUVerModules PlaYUVerLib  ${OpenCV_LIBRARIES} -ldl )
