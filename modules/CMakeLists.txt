###
### CMakeLists for playuver modules component
###

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} )

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
INCLUDE( ModulesList.cmake )


#
# Finish header
#
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListMacro.h    "\n#endif // __MODULESLISTMACRO_H__\n" )
file( APPEND ${CMAKE_CURRENT_BINARY_DIR}/ModulesListHeader.h   "#endif // __MODULESLISTHEADER_H__\n" )

ADD_LIBRARY( PlaYUVerModules ${plaYUver_Mod_SRCS} )
