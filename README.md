plaYUVer
========

plaYUVer is an open-source video player with the following features:
- support for libavformat and libavcodec;
- support for wide range of raw video formats;
- support for OpenCV imag/video processing library
- advanced frame zoom with easy to use pan function
- usefull information in status bar, *e.g.*, pixel information
- usefull sidebars with stream and frame information, *e.g.*, frame histogram
- Advanced API for frame/video processing algorithms


Developers
----------------------------------------------------------------

- João Carreira     (jfmcarreira@gmail.com)
- Luís Lucas        (luisfrlucas@gmail.com) 


Install using cmake
----------------------------------------------------------------

1. The following are required for compilation:
  - an ordinary C++ development environment (g++, make, C++ libraries and headers, ...)
  - cmake
  - a Qt development environment (libraries, headers, qmake, ...): only QtCore, QtGui and QtWidgets are required, at least in version 4

2. Dependencies:
  - QT
  - OpenCv (optional)
  - FFmpeg (optional)

3. Run the following commands: (this will create an executable named playuver) 
```
mkdir build
cd build
cmake -DUSE_FFMPEG=ON -DUSE_OPENCV=ON ..  (for FFmpeg and OpenCV support)
make
make install 
```
