PlaYUVer - Enhanced raw video player
========


plaYUVer is an open-source video player with the following features:
- Support for libavformat and libavcodec;
- Support for wide range of raw video formats;
- Support for OpenCV imag/video processing library
- Advanced frame zoom with easy to use pan function
- Synchronized zoom across several videos (perfect for comparison operations)
- Usefull information in status bar, *e.g.*, pixel information
    Usefull sidebars with frame information, *e.g.*, frame histogram
- Constant growing libs for stream and frame processing (PlaYUVerStream PlaYUVerFrame)
- Advanced API for frame/video processing algorithms
- Frame level quality measurement API based on the PlaYUVerFrame class
- Command-line tool for quality and frame processing algorithms (uses the referred APIs)

Supported Quality Metrics
----------------------------------------------------------------
- PSNR
- SSIM
- MSE

Frame Processing Modules
----------------------------------------------------------------
- FilterComponentLuma
- FilterComponentChromaU
- FilterComponentChromaV
- FrameDifference
- AbsoluteFrameDifference
- FrameCrop
- FrameBinarization
- EightBitsSampling
- LumaAverage
- DisparityStereoVar
- DisparityStereoBM

Developers
----------------------------------------------------------------

- Joao Carreira     (jfmcarreira@gmail.com)
- Lui­s Lucas        (luisfrlucas@gmail.com)


Build
----------------------------------------------------------------
Download update build for windows and linux from our <a href="https://sourceforge.net/projects/playuver/">SourceForge project page</a>

Building yourself

1. Dependencies
  - an ordinary C++ development environment (g++, make, C++ libraries and headers, ...)
  - cmake
  - a Qt development environment (libraries, headers, qmake, ...): only QtCore, QtGui and QtWidgets are required, at least in version 4
  - QtDBus (optional)
  - OpenCv (optional)
  - FFmpeg (optional)
2. Example configuration command
```
mkdir build
cd build
cmake -DUSE_QT4=OFF -DUSE_FFMPEG=ON -DUSE_OPENCV=ON ..  (for FFmpeg and OpenCV support)
make
make install
```
