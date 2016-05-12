###
### CMakeLists for playuver modules component
###

OPTION( MODULE_THREADED_MODULES "Use threads for modules"  OFF )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set(plaYUver_Mod_SRCS
  PlaYUVerModuleFactory.cpp
)

MACRO(ADD_MODULE name file )
  set(__modulename "Module${name}")
  OPTION(MODULE_${name} "Use module ${name}" ON )
  ADD_FEATURE_INFO(${name} MODULE_${name} "Module ${name}" )
  if( MODULE_${name} )
    list(APPEND plaYUver_Mod_SRCS ${file} )
    LIST(APPEND MODULES_LIST_NAME ${name} )
    LIST(APPEND MODULES_LIST_FILES ${file} )
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#include \"${file}.h\"\n")
  endif()
  unset(__modulename)
ENDMACRO()


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
IF( OpenCV_FOUND )
  IF( OPENCV_OPTFLOW_FOUND )
    ADD_MODULE( OpticalFlowDualTVL1 "OpticalFlow" )
    ADD_MODULE( OpticalFlowSparseToDense "OpticalFlow" )
    ADD_MODULE( OpticalFlowFarneback "OpticalFlow" )
  ELSE()
    ADD_MODULE( OpticalFlowDualTVL1 "OpticalFlowDualTVL1" )
  ENDIF()
  ADD_MODULE( DisparityStereoBM "DisparityStereoBM" )
  ADD_MODULE( DisparityStereoSGBM "DisparityStereoSGBM" )
  #ADD_MODULE( DisparityStereoVar "DisparityStereoVar" )
ENDIF()


#
# Create header
#
file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "// This files add the header files of each module\n"

                                                            "#ifndef __MODULESLISTHEADER_H__\n#define __MODULESLISTHEADER_H__\n" )
foreach(module ${MODULES_LIST_FILES})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#include \"${module}.h\"\n")
endforeach(module ${MODULES_LIST_FILES})
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#define REGISTER_ALL_MODULES \\\n" )
foreach(module ${MODULES_LIST_NAME})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "Register( \"${module}\", &(${module}::Create) ); \\\n")
endforeach(module ${MODULES_LIST_NAME})
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "\n#endif // __MODULESLISTHEADER_H__\n" )

INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
ADD_LIBRARY( PlaYUVerModules ${plaYUver_Mod_SRCS} )
TARGET_LINK_LIBRARIES( PlaYUVerModules PlaYUVerLib  ${OpenCV_LIBRARIES} -ldl )
