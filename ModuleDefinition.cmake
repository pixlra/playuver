######################################################################################
# Default Modules
######################################################################################

ADD_MODULE( FilterComponentLuma "FilterComponentLuma" )
ADD_MODULE( FilterComponentChromaU "FilterComponentLuma" )
ADD_MODULE( FrameDifference "FrameDifference" )
ADD_MODULE( MeasurePSNR "MeasurePSNR" )

IF( USE_OPENCV )
  ADD_MODULE( DisparityStereoVar "DisparityStereoVar" )
ENDIF()

