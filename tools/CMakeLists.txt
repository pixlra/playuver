###
### CMakeLists for playuver app component
###

# This is hard way - please fix
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}/../
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../ )

set(plaYUver_Tools_SRCS
  main.cpp
  PlaYUVerTools.cpp
  CommandLineHandle.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME}Tools ${plaYUver_Tools_SRCS} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}Tools PlaYUVerLib PlaYUVerModules )

INSTALL(TARGETS ${PROJECT_NAME}Tools DESTINATION bin )


