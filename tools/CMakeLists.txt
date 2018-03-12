###
### CMakeLists for calyp tools component
###

# This is hard way - please fix
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}/../
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../ )

set(Calyp_Tools_SRCS
  main.cpp
  CalypTools.cpp
  CalypToolsCmdParser.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME}Tools ${Calyp_Tools_SRCS} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}Tools ${PROJECT_LIBRARY} CalypModules )

INSTALL(TARGETS ${PROJECT_NAME}Tools DESTINATION bin )


