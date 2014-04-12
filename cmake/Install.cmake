######################################################################################
# Install
######################################################################################

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/icons )
INSTALL(FILES resources/x-raw.xml DESTINATION share/mime/video )

# # try to set up the menu system
# find_program(XDG-MIME_EXECUTABLE xdg-mime)
# # find_program(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)
# INSTALL(CODE "execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor resources/)" )

######################################################################################
# Create deb package
######################################################################################
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-core,libqt4-gui,libavformat53,libcodec53,libavutil51")
SET(CPACK_PACKAGE_DESCRIPTION "plaYUVer is an open-source QT based raw video player")
SET(CPACK_PACKAGE_CONTACT "Joao Carreira (jfmcarreira@gmail.com)")
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/Debian/postinst")

INCLUDE(CPack)