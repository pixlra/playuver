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

ADD_MODULE_USE_OPENCV( MeasureOpticalFlowDualTVL1 "MeasureOpticalFlowDualTVL1" VERSION 2.4.13.0 MODULES video )

ADD_MODULE_USE_OPENCV( OpticalFlowDualTVL1 "OpticalFlow" VERSION  3.1.0 MODULES optflow )
ADD_MODULE_USE_OPENCV( OpticalFlowSparseToDense "OpticalFlow" VERSION  3.1.0 MODULES optflow )
ADD_MODULE_USE_OPENCV( OpticalFlowFarneback "OpticalFlow" VERSION 3.1.0 MODULES video optflow )

ADD_MODULE_USE_OPENCV( DisparityStereoBM "DisparityStereoBM" VERSION 2.4.13.0 MODULES calib3d )
ADD_MODULE_USE_OPENCV( DisparityStereoSGBM "DisparityStereoSGBM" VERSION 2.4.13.0 MODULES calib3d )



CREATE_MODULE_MACROS()
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
ADD_LIBRARY( PlaYUVerModules ${plaYUver_Mod_SRCS} )
FIND_PACKAGE( OpenCV COMPONENTS ${OPENCV_MODULES} )
TARGET_LINK_LIBRARIES( PlaYUVerModules PlaYUVerLib  ${OpenCV_LIBRARIES} -ldl )
