#
# QGC Windows-only configuration directives
#

win32-msvc2010:message(Building for Windows Visual Studio 2010 (32bit))
win32-msvc2012:message(Building for Windows Visual Studio 2012 (32bit))
win32-g++:message(Building for Windows GCC (32bit))

# The EIGEN library needs this define
# to make the internal min/max functions work
DEFINES += NOMINMAX

win32-msvc2010|win32-msvc2012|win32-msvc2013 {
	# QWebkit is not needed on MS-Windows compilation environment
	CONFIG -= webkit webkitwidgets

	# Specify the inclusion of (U)INT*_(MAX/MIN) macros within Visual Studio
	DEFINES += __STDC_LIMIT_MACROS

	INCLUDEPATH += $$BASEDIR/libs/lib/sdl/msvc/include \
		  #$$BASEDIR/libs/lib/opal/include

	LIBS += -L$$BASEDIR/libs/lib/sdl/msvc/lib

	# Specify multi-process compilation within Visual Studio.
	# (drastically improves compilation times for multi-core computers)
	QMAKE_CXXFLAGS_DEBUG += /MP
	QMAKE_CXXFLAGS_RELEASE += /MP

	QMAKE_CXXFLAGS_WARN_ON += /W3 \
		  /wd4996 \   # silence warnings about deprecated strcpy and whatnot
		  /wd4005 \   # silence warnings about macro redefinition
		  /wd4290     # ignore exception specifications

	# QAxContainer support is needed for the Internet Control
	# element showing the Google Earth window
	greaterThan(QT_MAJOR_VERSION, 4) {
		QT += axcontainer
	} else {
		CONFIG += qaxcontainer
	}
} # end win32-msvc-*

win32-g++ {  # MinGW
	INCLUDEPATH += $$BASEDIR/libs/lib/sdl/include \
	LIBS += -L$$BASEDIR/libs/lib/sdl/win32 \
}


LIBS += -lSDLmain -lSDL -lsetupapi
