######################################################################################
# Install
######################################################################################

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin )
INSTALL(FILES resources/playuver.desktop DESTINATION share/applications )
INSTALL(FILES resources/playuver.png DESTINATION share/icons )
INSTALL(FILES resources/x-raw.xml DESTINATION share/mime/video )

IF( WIN32 )
  IF( NOT USE_QT4 )
    INSTALL(FILES ${QT_DIR}/bin/QT5Core.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/Qt5Gui.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/Qt5Widgets.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/libGLESv2.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/libEGL.dll DESTINATION bin )    
    INSTALL(FILES ${QT_DIR}/bin/icudt52.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/icuin52.dll DESTINATION bin )
    INSTALL(FILES ${QT_DIR}/bin/icuuc52.dll DESTINATION bin )
    
    INSTALL(FILES ${QT_DIR}/plugins/platforms/qminimal.dll DESTINATION bin/platforms )
    INSTALL(FILES ${QT_DIR}/plugins/platforms/qoffscreen.dll DESTINATION bin/platforms )
    INSTALL(FILES ${QT_DIR}/plugins/platforms/qwindows.dll DESTINATION bin/platforms )
  ENDIF()
  IF( USE_OPENCV )
    SET(OpenCV_DLL_DIR "${OpenCV_DIR}/bin" CACHE PATH "Where to find OpenCV dlls")
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_core${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_contrib${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_highgui${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_contrib${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_imgproc${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_calib3d${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_features2d${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_flann${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_ml${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_video${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_videostab${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_superres${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_stitching${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_photo${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_objdetect${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_nonfree${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_legacy${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
    INSTALL(FILES ${OpenCV_DLL_DIR}/opencv_gpu${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll DESTINATION bin )
  ENDIF()
ENDIF()


######################################################################################
# Create deb package
######################################################################################
SET(CPACK_GENERATOR "DEB")

SET(CPACK_PACKAGE_NAME "plaYUVer")
SET(CPACK_PACKAGE_CONTACT "Joao Carreira (jfmcarreira@gmail.com), Luis Lucas (luisfrlucas@gmail.com)")

SET(CPACK_PACKAGE_VERSION_MAJOR ${PLAYUVER_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PLAYUVER_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PLAYUVER_VERSION_PATH})

SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "plaYUVer is an open-source QT based raw video player")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joao Carreira (jfmcarreira@gmail.com), Luis Lucas (luisfrlucas@gmail.com)")


SET(CPACK_DEBIAN_PACKAGE_DEPENDS "")
IF( USE_QT4 )
  LIST(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "libqtcore4 (>= 4:4.8.5~), libqtgui4 (>= 4:4.8.5~)" )
ELSE()
  LIST(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5gui5, libqt5widgets5")
ENDIF()

IF( USE_FFMPEG )
  LIST(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS ",")
  LIST(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "libavformat53 (>= 6:0.8.10~), libcodec53 (>= 6:0.8.3-1~), libavutil51 (>= 6:0.8.3-1~)")
ENDIF()
<<<<<<< HEAD
IF( USE_OPENCV )
  SET(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}")
ENDIF()

SET(CPACK_PACKAGE_DESCRIPTION "plaYUVer is an open-source QT based raw video player")
SET(CPACK_PACKAGE_CONTACT "Joao Carreira (jfmcarreira@gmail.com), Luis Lucas (luisfrlucas@gmail.com)")
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/Debian/postinst")
=======


>>>>>>> master
INCLUDE(CPack)
