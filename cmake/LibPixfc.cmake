#####################################################################################
# Qt library cmake file
######################################################################################

IF( USE_PIXFC )
    LIST(APPEND LINKER_LIBS pixfc-sse)
    LIST(APPEND LINKER_LIBS rt)
ENDIF()