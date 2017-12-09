###
### CMakeLists for qcustomplot
###

FIND_PACKAGE( Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport )

set(qcustomplot_SRCS
  qcustomplot.cpp
  qcustomplot.h
)

ADD_LIBRARY( QCustomPlotLib STATIC ${qcustomplot_SRCS} )
TARGET_LINK_LIBRARIES( QCustomPlotLib
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::PrintSupport
)

TARGET_INCLUDE_DIRECTORIES(QCustomPlotLib PUBLIC . )
