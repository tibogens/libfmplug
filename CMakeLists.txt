CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#CMAKE_POLICY(VERSION 2.8.12)

PROJECT(fmplug)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/")

SET(CMAKE_DEBUG_POSTFIX "d")

IF(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	SET(FMPLUG_SUFFIX "d")
	IF(NOT WIN32)
	  ADD_DEFINITIONS(-D_DEBUG)
	ENDIF(NOT WIN32)
ELSE (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	SET(FMPLUG_SUFFIX "")
ENDIF (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")

IF (CMAKE_CL_64)
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${fmplug_SOURCE_DIR}/lib64)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${fmplug_SOURCE_DIR}/lib64)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${fmplug_SOURCE_DIR}/bin64)
ELSE (CMAKE_CL_64)
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${fmplug_SOURCE_DIR}/lib)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${fmplug_SOURCE_DIR}/lib)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${fmplug_SOURCE_DIR}/bin)
ENDIF (CMAKE_CL_64)

OPTION (FMPLUG_BUILD_TESTS "Build the tests" ON)
OPTION (FMPLUG_BUILD_SAMPLES "Build the sample plugins" ON)
OPTION (FMPLUG_USE_WXWIDGETS "Build the wxWidgets-based components" OFF)
OPTION (FMPLUG_MSVC_INSTRUMENT_DEBUG "(Microsoft compilers only) Instrument the code in Debug mode (runs slower)" OFF)

INCLUDE_DIRECTORIES (${fmplug_SOURCE_DIR}/include )

# Some options not only affect the configuration of the build, but also the build
# itself. This is a common problem in CMake (also described in the Mastering CMake
# book, and answered on the net) and I solved it my way (prior to noticing the
# book's way).
# I need to first collect the FMPLUG_xx options into a list, then add the preprocessor
# definition -DFMPLUG_xx=0 or 1, depending on the CMake option's value. I have created
# two lists: one that contains all the options, and one that contain only the options
# that are actually used by the C++ compiler.
# There is certainly a better way to collect the names of the options, although I am
# not aware of it.

# All options
SET (ALL_OPTIONS
	FMPLUG_USE_WXWIDGETS
	FMPLUG_MSVC_INSTRUMENT_DEBUG
)
SET(FMPLUG_BUILD_OPTIONS "")
FOREACH (mymacro ${ALL_OPTIONS})
	IF (${mymacro})
		LIST(APPEND FMPLUG_BUILD_OPTIONS "-D${mymacro}=1 ")
	ELSE (${mymacro})
		LIST(APPEND FMPLUG_BUILD_OPTIONS "-D${mymacro}=0 ")
	ENDIF (${mymacro})
ENDFOREACH (mymacro ${ALL_OPTIONS})

# Options used by the C++ compiler (to avoid polluting the compiler command line)
SET (OPTIONS_TO_CPP_PREPROCESSOR
	FMPLUG_USE_WXWIDGETS
)
FOREACH (mymacro ${OPTIONS_TO_CPP_PREPROCESSOR})
	IF (${mymacro})
		ADD_DEFINITIONS(-D${mymacro}=1)
	ELSE (${mymacro})
		ADD_DEFINITIONS(-D${mymacro}=0)
	ENDIF (${mymacro})
ENDFOREACH (mymacro ${OPTIONS_TO_CPP_PREPROCESSOR})

# Add the _CRTDBG_REPORT_FLAG to Win32 projects. This is very handy
# to track more efficiently memory leaks and/or errors. 
IF (WIN32 AND FMPLUG_MSVC_INSTRUMENT_DEBUG)
	SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ${CurrentDebugFlags};_CRTDBG_MAP_ALLOC)
ENDIF (WIN32 AND FMPLUG_MSVC_INSTRUMENT_DEBUG)

ADD_SUBDIRECTORY(src)
