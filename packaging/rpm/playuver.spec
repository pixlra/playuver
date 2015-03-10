#
# Spec file for package plaYUVer
#
# Copyright © 2015 Markus S. <kamikazow@web.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

Name:       playuver
Summary:    plaYUVer - Enhanced Raw video player
Version:    0.5.1
Release:    0%{?dist}
License:    GPL-2.0+
URL:        https://github.com/pixlra/playuver

# For this spec file to work, the plaYUVer sources must be located
# in a directory named playuver-0.5 (with "0.5" being the version
# number defined above).
# If the sources are compressed in another format than .tar.xz, change the
# file extension accordingly.
Source0:    %{name}-%{version}.tar.xz

# Package names verified with, CentOS 7, Fedora 21, and openSUSE 13.2.
# Should the packages in your distro be named differently,
# see http://en.opensuse.org/openSUSE:Build_Service_cross_distribution_howto

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  opencv-devel
BuildRequires:  pkgconfig(QtCore)
BuildRequires:  pkgconfig(QtGui)
# QtWebKit is required for building even if FERVOR auto updater is OFF.
# This causes problems with CentOS and RHEL which don't include QtWebKit
# in their main repos.
BuildRequires:  pkgconfig(QtWebKit)

%description
plaYUVer is an open source video player with the following features:
- Support for libavformat and libavcodec;
- Support for wide range of raw video formats;
- Support for OpenCV imag/video processing library
- Advanced frame zoom with easy to use pan function
- Usefull information in status bar, *e.g.*, pixel information
- Usefull sidebars with stream and frame information, *e.g.*, frame histogram
- Constant growing libs for stream and frame processing (PlaYUVerStream PlaYUVerFrame)
- Advanced API for frame/video processing algorithms
- Frame level quality measurement API based on the PlaYUVerFrame class

# ------------------------------------------------------

%package devel
Summary:        Development files for plaYUVer

%description devel
Development files for plaYUVer

%files devel
%{_includedir}/PlaYUVer*
# _libdir may become lib64 on 64bit systems.
%{_libdir}/../lib/libPlaYUVerLib.a

# ------------------------------------------------------

%prep
%setup -q

%build
export CCFLAGS='%{optflags}'
mkdir build
pushd build
# Frevor should be off for builds in a repo.
# FFMPEG is not included in many distros.
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DUSE_QT4=ON -DUSE_FERVOR=OFF -DUSE_OPENCV=ON -DUSE_FFMPEG=OFF
make %{?_smp_mflags}

%install
export CCFLAGS='%{optflags}'
pushd build
make %{?_smp_mflags} install DESTDIR="%{?buildroot}"

%files
%defattr(-,root,root,-)
%doc LICENSE README.md
%{_bindir}/%{name}
%{_bindir}/playuverTools
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/mime/packages/%{name}.xml

%clean
rm -rf %{buildroot}

%changelog
