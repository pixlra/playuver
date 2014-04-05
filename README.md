plaYUVer
========

plaYUVer is an open-source video player with the following features:
- support for libavformat and libavcodec;
- support for wide range of raw video formats;
- advanced frame zoom with easy to use pan function
- usefull information in status bar, *e.g.*, pixel information
- API for frame processing algorithms


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

3. Run the following commands: (this will create an executable named playuver) 
```
mkdir build
cd build
cmake -DUSE_FFMPEG=ON ..  (for FFmpeg support)
make
make install 
```
