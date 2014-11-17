
include(cmake/GetGitRevisionDescription.cmake)

git_describe(__version --tags)

string(REPLACE "-" ";" __version_list ${__version})
list(GET __version_list 0 __version)
list(LENGTH ${__version_list} __version_list_size )
IF( ${__version_list_size} GREATER 1 )
  list(GET __version_list 1 PLAYUVER_VERSION_PATCH)
ELSE()
  SET( PLAYUVER_VERSION_PATCH 0 )
ENDIF()

string(REPLACE "." ";" __version_list ${__version})
list(GET __version_list 0 PLAYUVER_VERSION_MAJOR)
list(GET __version_list 1 PLAYUVER_VERSION_MINOR)


SET( PLAYUVER_VERSION "${PLAYUVER_VERSION_MAJOR}.${PLAYUVER_VERSION_MINOR}.${PLAYUVER_VERSION_PATCH}")

IF( ${PLAYUVER_VERSION_PATCH} EQUAL 0 )
  SET( PACK_NAME "${PLAYUVER_VERSION_MAJOR}.${PLAYUVER_VERSION_MINOR}" )
ELSE()
  IF( NOT WIN32 )
    execute_process(
      COMMAND git rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    SET( __version_name_deb ${GIT_BRANCH} )
    STRING(REPLACE "master" "latest" __version_name_deb ${__version_name_deb}  )
    STRING(REPLACE "devel" "experimental" __version_name_deb ${__version_name_deb} )

    SET( PACK_NAME ${__version_name_deb} )
  ELSE()
    SET( PACK_NAME "latest" )
  ENDIF()
ENDIF()

unset(__version_name_deb) 
unset(__version_list) 
unset(__version_list_size)
unset(__version)


