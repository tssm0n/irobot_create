bin_PROGRAMS = robotcontrol
robotcontrol_SOURCES = controlprogram.cpp customirobotserial.cpp \
	irobotcommandhandler.cpp irobotcontrol.cpp irobotserial.cpp mappingcontrolprogram.cpp \
	remotecontrol.cpp sensordata.cpp

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
robotcontrol_LDFLAGS = $(all_libraries) -lpthread
noinst_HEADERS = controlprogram.h customirobotserial.h irobotcommandhandler.h \
	mappingcontrolprogram.h remotecontrol.h
