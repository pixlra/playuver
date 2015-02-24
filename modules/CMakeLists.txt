###
### CMakeLists for playuver modules component
###

OPTION( MODULE_THREADED_MODULES "Use threads for modules"  OFF )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set(plaYUver_Mod_SRCS
  PlaYUVerModuleFactory.cpp
)

MACRO(ADD_MODULE name file )
  set(__modulename "Module${name}")
  OPTION(MODULE_${name}        "Use module ${name}"       ON )
  ADD_FEATURE_INFO(${name} MODULE_${name} "Module ${name}" )
  if( MODULE_${name} )
    list(APPEND plaYUver_Mod_SRCS ${file} )
    LIST(APPEND MODULES_LIST ${name} )
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#include \"${file}.h\"\n")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListMacro.h "\\\n  REGISTER_MODULE( ${name} ); ")
  endif()
  unset(__modulename)
ENDMACRO()


file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "// This files add the header files of each module\n"

                                                            "#ifndef __MODULESLISTHEADER_H__\n#define __MODULESLISTHEADER_H__\n" )


#
# Modules List
#
ADD_MODULE( FilterComponentLuma "FilterComponent" )
ADD_MODULE( FilterComponentChromaU "FilterComponent" )
ADD_MODULE( FilterComponentChromaV "FilterComponent" )
ADD_MODULE( FrameDifference "FrameDifference" )
ADD_MODULE( AbsoluteFrameDifference "AbsoluteFrameDifference" )
ADD_MODULE( LumaAverage "LumaAverage" )
IF( USE_OPENCV )
  ADD_MODULE( DisparityStereoVar "DisparityStereoVar" )
ENDIF()


#
# Finish header
#
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#endif // __MODULESLISTHEADER_H__\n" )

ADD_LIBRARY( PlaYUVerModules ${plaYUver_Mod_SRCS} )
