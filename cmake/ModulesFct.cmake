

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


MACRO(ADD_MODULE_OPENCV name file version )
  IF( ${OpenCV_FOUND} AND ( NOT ( ${OpenCV_VERSION} VERSION_LESS ${version} ) ) )
    ADD_MODULE( ${name} ${file} )
  ENDIF()
ENDMACRO()


#
# Create header
#

MACRO(CREATE_MODULE_MACROS )

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

ENDMACRO()
