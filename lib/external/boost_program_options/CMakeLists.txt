###
### CMakeLists for playuver external component
###


#
# Boost Library
#

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

set(BoostProgramOptions_SRCS
    src/cmdline.cpp
    src/config_file.cpp
    src/options_description.cpp
    src/parsers.cpp
    src/variables_map.cpp
    src/value_semantic.cpp
    src/positional_options.cpp
    src/utf8_codecvt_facet.cpp
    src/convert.cpp
    src/winmain.cpp
    src/split.cpp
)

ADD_LIBRARY( BoostProgramOptions STATIC ${BoostProgramOptions_SRCS} )

SET( Boost_FOUND TRUE PARENT_SCOPE)
SET( Boost_LIBRARIES BoostProgramOptions PARENT_SCOPE)
