######################################################################################
# Install
######################################################################################

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/icons )
INSTALL(FILES resources/x-raw.xml DESTINATION share/mime/video )

######################################################################################
# Create deb package
######################################################################################
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PLAYUVER_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PLAYUVER_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PLAYUVER_VERSION_PATH})
IF( USE_QT4 )
  SET(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS} libqtcore4,libqtgui4 ")
ENDIF()
IF( USE_FFMPEG )
  SET(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS} libavformat53,libcodec53,libavutil51")
ENDIF()
SET(CPACK_PACKAGE_DESCRIPTION "plaYUVer is an open-source QT based raw video player")
SET(CPACK_PACKAGE_CONTACT "Joao Carreira (jfmcarreira@gmail.com), Luis Lucas (luisfrlucas@gmail.com)")
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/Debian/postinst")
INCLUDE(CPack)