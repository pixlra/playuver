---
title: PlaYUVer
author:
    username: jfmcarreira
markdown:
    extra: true
---

# [PlaYUVer](https://github.com/pixlra/playuver)
## Enhanced YUV video player based on Qt focusing on image/video analysis


## Features
- Support for libavformat and libavcodec;
- Support for wide range of raw video formats;
- Support up to 16 bits per pixel
- Support for OpenCV image/video processing library
- Advanced frame zoom with easy to use pan function
- Synchronized zoom across several videos (perfect for comparison operations)
- Useful information in status bar, *e.g.*, pixel information
    Useful sidebars with frame information, *e.g.*, frame histogram
- Constant growing libs for stream and frame processing (PlaYUVerStream and PlaYUVerFrame)
- Advanced API for frame/video processing algorithms
- Frame level quality measurement API based on the PlaYUVerFrame class
- Powefull command-line tool for quality and frame processing algorithms (uses the referred APIs)

## Modules
- PlaYUVerLib: Low level library
- PlaYUVerModules: Abstract interface for frame processing modules
- PlaYUVerApp: High level graphical interface
- PlaYUVerTools: High level command line interface

##  Supported Quality Metrics
- PSNR
- SSIM
- MSE

## Frame Processing Modules
- Component filtering
- Frame difference, crop, shift, binarization
- Sub-sampling operations
- Measuring modules
- Modules based on opencv:
    - Disparity estimation
    - Motion estimation

## Supported Formats
- Supports for the following pixel formats:
    * YUV: 420, 44, 422, 400
    * RGB
    * Gray
- Support containers:
    * Raw video (yuv,rgb,gray)
    * Portable Network Graphics (png)
    * Joint Photographic Experts Group (jpeg)
    * Windows Bitmap (bmp)
    * Tagged Image File Format (tiff)
    * Portable Map (ppm,pgm,pbm)

## Developers
- **Joao Carreira**     (jfmcarreira@gmail.com)
- **Lui­s Lucas**        (luisfrlucas@gmail.com)


### Binaries and Build
* **Daily builds** available for Windows and Linux in our [SourceForge page](https://sourceforge.net/projects/playuver/)
* **Dependencies**
  1. an ordinary C++ development environment (g++, make, C++ libraries and headers, ...)
  2. cmake
  3. a Qt development environment: QtCore, QtGui, QtWidgets, QtPrintSupport and QtConcurrent (version 4 or higher)
  4. Optional: QtDBus, OpenCV, FFmpeg
* **List of required commands**
```
mkdir build
cd build
cmake -DUSE_QT4=OFF -DUSE_FFMPEG=ON -DUSE_OPENCV=ON ..  (for FFmpeg and OpenCV support)
make
make install
```
