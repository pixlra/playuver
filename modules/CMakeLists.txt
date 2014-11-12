###
### CMakeLists for playuver modules component
###

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

# set(plaYUver_Mod_SRCS
#     PlaYUVerModuleIf.cpp
# )


#
# Init header
#
file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ModulesListMacro.h  "// This files defines the macro to \n"
                                                            "// define a list of modules\n"
                                                            "#ifndef __MODULESLISTMACRO_H__\n#define __MODULESLISTMACRO_H__\n"
                                                            "#define REGISTER_ALL_MODULES ")

file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "// This files add the header files of each module\n"

                                                            "#ifndef __MODULESLISTHEADER_H__\n#define __MODULESLISTHEADER_H__\n" )


#
# Modules List
#
ADD_MODULE( FilterComponentLuma "FilterComponent" )
ADD_MODULE( FilterComponentChromaU "FilterComponent" )
ADD_MODULE( FilterComponentChromaV "FilterComponent" )
ADD_MODULE( FrameDifference "FrameDifference" )
ADD_MODULE( MeasurePSNR "MeasurePSNR" )
# IF( USE_OPENCV )
#   ADD_MODULE( DisparityStereoVar "DisparityStereoVar" )
# ENDIF()


#
# Finish header
#
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListMacro.h    "\n#endif // __MODULESLISTMACRO_H__\n" )
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#endif // __MODULESLISTHEADER_H__\n" )

ADD_LIBRARY( PlaYUVerModules ${plaYUver_Mod_SRCS} )


status("Modules List:"        "${MODULES_LIST}" )