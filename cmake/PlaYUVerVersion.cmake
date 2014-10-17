
include(cmake/GetGitRevisionDescription.cmake)

git_describe(__version --tags)

string(REPLACE "-" ";" __version_list ${__version})
list(GET __version_list 0 __version)
list(GET __version_list 1 __patch)
string(REPLACE "." ";" __version_list ${__version})
list(GET __version_list 0 __major)
list(GET __version_list 1 __minor)


SET( PLAYUVER_VERSION_MAJOR ${__major})
SET( PLAYUVER_VERSION_MINOR ${__minor})
SET( PLAYUVER_VERSION_PATCH ${__patch})
SET( PLAYUVER_VERSION "${PLAYUVER_VERSION_MAJOR}.${PLAYUVER_VERSION_MINOR}.${PLAYUVER_VERSION_PATCH}")


IF( NOT WIN32 )
  
  execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET( __version_name_deb ${GIT_BRANCH} )
  STRING(REPLACE "master" "stable" __version_name_deb ${__version_name_deb}  )
  STRING(REPLACE "devel" "latest" __version_name_deb ${__version_name_deb} )

  SET( PACK_NAME ${__version_name_deb} )
ELSE()
  SET( PACK_NAME "devel" )
ENDIF()
unset(__version_name_deb) 
unset(__version_list) 
unset(__version)
unset(__patch)
unset(__major)
unset(__minor)

