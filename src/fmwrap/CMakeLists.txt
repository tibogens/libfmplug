SET(LIB_NAME fmwrap)

FIND_PACKAGE(FMProSDK REQUIRED)

SET(HEADER_PATH ${FMPROSDK_INCLUDE_DIR}/FMWrapper)

INCLUDE_DIRECTORIES(${FMPROSDK_INCLUDE_DIRS})

SET(LIB_PUBLIC_HEADERS
	${HEADER_PATH}/FMXExtern.h
	${fmplug_SOURCE_DIR}/include/fmwrap/Mock.h
)

SET(LIB_SRC
	fmwrap.cpp
)

if (WIN32)
	set_source_files_properties(
		pch.cpp
		PROPERTIES
		COMPILE_FLAGS "/Ycpch.h"
		)
    foreach( src_file ${LIB_SRC} )
		if ( ${src_file} MATCHES "^.*\\.h$" )
			set_source_files_properties(${src_file} PROPERTIES HEADER_FILE_ONLY TRUE) 
		else ( ${src_file} MATCHES "^.*\\.h$" )
			if ( ${src_file} MATCHES "^.*\\.cpp$" )
				set_source_files_properties(
					${src_file}
					PROPERTIES
					COMPILE_FLAGS "/Yupch.h"
					)
			endif ( ${src_file} MATCHES "^.*\\.cpp$" )
		endif ( ${src_file} MATCHES "^.*\\.h$" )
    endforeach( src_file ${LIB_SRC} )
    list(APPEND LIB_SRC pch.cpp)
	list(APPEND LIB_SRC pch.h)
    #foreach( src_file ${LIB_SRC} )
		#MESSAGE(${src_file})
	#endforeach( src_file ${LIB_SRC} )
endif (WIN32)

ADD_DEFINITIONS(-Dlibfmplug_STATIC)

ADD_LIBRARY(${LIB_NAME}
	SHARED
    ${LIB_PUBLIC_HEADERS}
	${LIB_SRC}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
)

set_property(TARGET ${LIB_NAME} PROPERTY LINK_INTERFACE_LIBRARIES "")

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PROJECT_LABEL "core - mock fmwrapper")
