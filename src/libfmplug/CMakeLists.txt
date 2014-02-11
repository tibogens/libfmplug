SET(LIB_NAME libfmplug)

FIND_PACKAGE(FMProSDK REQUIRED)

SET(HEADER_PATH ${fmplug_SOURCE_DIR}/include/fmplug)

INCLUDE_DIRECTORIES(${FMPROSDK_INCLUDE_DIRS})

SET(LIB_PUBLIC_HEADERS
	${HEADER_PATH}/Error.h
	${HEADER_PATH}/Export.h
	${HEADER_PATH}/Function.h
	${HEADER_PATH}/Plugin.h
	${HEADER_PATH}/Setup.h
	${HEADER_PATH}/TypeMaps.h
	${HEADER_PATH}/UtfConv.h
	${HEADER_PATH}/Utils.h
)

SET(LIB_SRC
	Error.cpp
	Function.cpp
	Plugin.cpp
	UtfConv.cpp
	Utils.cpp
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

ADD_LIBRARY(${LIB_NAME}
	SHARED
    ${LIB_PUBLIC_HEADERS}
	${LIB_SRC}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
	${FMPROSDK_LIBRARIES}
)

set_property(TARGET ${LIB_NAME} PROPERTY INTERFACE_LINK_LIBRARIES "")

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PROJECT_LABEL "core - libfmplug")
