SET(APP_NAME test_basic)

FIND_PACKAGE(FMProSDK REQUIRED)
INCLUDE_DIRECTORIES(${FMPROSDK_INCLUDE_DIRS})

#LINK_DIRECTORIES(${libfmplug_BINARY_DIR}/uflr)

SET(APP_SRC
	main.cpp
)

if (WIN32)
	set_source_files_properties(
		pch.cpp
		PROPERTIES
		COMPILE_FLAGS "/Ycpch.h"
		)
    foreach( src_file ${APP_SRC} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yupch.h"
            )
    endforeach( src_file ${APP_SRC} )
    list(APPEND APP_SRC pch.cpp)
	list(APPEND APP_SRC pch.h)
endif (WIN32)

ADD_EXECUTABLE (${APP_NAME}
	${APP_SRC}
)

SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

TARGET_LINK_LIBRARIES (${APP_NAME} libfmplug
) 

SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES PROJECT_LABEL "test - ${APP_NAME}")
