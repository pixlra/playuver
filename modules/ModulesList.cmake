###
### Modules List
###

ADD_MODULE( FilterComponentLuma "FilterComponent" )
ADD_MODULE( FilterComponentChromaU "FilterComponent" )
ADD_MODULE( FilterComponentChromaV "FilterComponent" )
ADD_MODULE( FrameDifference "FrameDifference" )
ADD_MODULE( AbsoluteFrameDifference "AbsoluteFrameDifference" )
#ADD_MODULE( MeasurePSNR "MeasurePSNR" )
IF( USE_OPENCV )
  ADD_MODULE( DisparityStereoVar "DisparityStereoVar" )
ENDIF()
