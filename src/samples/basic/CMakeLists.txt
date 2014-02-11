SET(SAMPLE_NAME basic)
SET(PLUGIN_NAME sample_${SAMPLE_NAME})

FIND_PACKAGE(FMProSDK REQUIRED)
INCLUDE_DIRECTORIES(${FMPROSDK_INCLUDE_DIRS})

SET(PLUGIN_SRC
	main.cpp
)

if (WIN32)
	set_source_files_properties(
		pch.cpp
		PROPERTIES
		COMPILE_FLAGS "/Ycpch.h"
		)
    foreach( src_file ${PLUGIN_SRC} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yupch.h"
            )
    endforeach( src_file ${PLUGIN_SRC} )
    list(APPEND PLUGIN_SRC pch.cpp)
	list(APPEND PLUGIN_SRC pch.h)
endif (WIN32)

ADD_DEFINITIONS(-Dlibfmplug_STATIC)

ADD_LIBRARY (${PLUGIN_NAME}
	SHARED
	${PLUGIN_SRC}
)

SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

TARGET_LINK_LIBRARIES (${PLUGIN_NAME} libfmplug
) 

SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES PROJECT_LABEL "sample - ${SAMPLE_NAME}")
