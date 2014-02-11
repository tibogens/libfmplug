# - Try to find FMProSDK (FileMaker Pro (TM) SDK)
# Once done this will define
#  FMPROSDK_FOUND - System has FMProSDK
#  FMPROSDK_INCLUDE_DIRS - The FMProSDK include directories
#  FMPROSDK_LIBRARIES - The libraries needed to use FMProSDK

set(_FMPROSDK_POSSIBLE_ROOTS "${CMAKE_SOURCE_DIR}/fmpro_sdk/current" "${CMAKE_SOURCE_DIR}/fmpro_sdk/2014-02-11"
)

find_path(FMPROSDK_INCLUDE_DIR FMWrapper/FMXExtern.h
          PATHS ${_FMPROSDK_POSSIBLE_ROOTS}
		  PATH_SUFFIXES Headers
        )


find_library(FMPROSDK_LIBRARY NAMES FMWrapper
          PATHS ${_FMPROSDK_POSSIBLE_ROOTS}
		  PATH_SUFFIXES Libraries/Win/win32
        )

set(FMPROSDK_LIBRARIES ${FMPROSDK_LIBRARY} )
set(FMPROSDK_INCLUDE_DIRS ${FMPROSDK_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FMPROSDK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FMProSDK  DEFAULT_MSG
                                  FMPROSDK_LIBRARY FMPROSDK_INCLUDE_DIR)

mark_as_advanced(FMPROSDK_INCLUDE_DIR FMPROSDK_LIBRARY )