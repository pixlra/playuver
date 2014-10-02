######################################################################################
# Default Modules
######################################################################################

ADD_MODULE( FilterFrame "FilterFrame.cpp" )
ADD_MODULE( FrameDifference "FrameDifference.cpp" )
#ADD_MODULE( MeasurePSNR "MeasurePSNR.cpp" )

IF( USE_OPENCV )
  ADD_MODULE( DisparityStereoVar "DisparityStereoVar.cpp" )
ENDIF()

