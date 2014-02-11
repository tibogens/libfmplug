SET(ALL_LIBS
	libfmplug
)

IF (FMPLUG_USE_WXWIDGETS)
	LIST(APPEND ALL_LIBS 
		""
    )
ENDIF (FMPLUG_USE_WXWIDGETS)

FOREACH (mylibfolder ${ALL_LIBS})

    ADD_SUBDIRECTORY(${mylibfolder})
	
	# On Windows, the import libraries (*.lib) are written to a subfolder
	# of lib/, named after the configuration (e.g. lib/Debug/ or lib/Release/)
	# Since we use suffixes to distinguish between them (e.g. <name>d.lib for Debug)
	# there is no need to put them in separate folders. Using IMPORT_PREFIX="../"
	# serves this purpose.
	
	# This loop, going through all targets (except for those whose directory name
	# is not the target name, see the comment below), is the ideal place to also
	# perform generic actions.
	# TODO: instead of duct taping like that, I should create my own ADD_LIBRARY
	# and ADD_EXECUTABLE macros that would do precisely this job - this would further
	# make the lower level CMakeLists much, much simpler.
	
	IF (WIN32)
		# The problem is that we can't just set the IMPORT_PREFIX for all
		# libraries, using the folder names, because in some cases the target
		# name is different from the directory name (e.g. for SWIG wrappers, the
		# target for directory wrapper/ will be called '_wrapper'). I guess 
		# we could wait until all targets are generated and apply the IMPORT_PREFIX
		# to all of them, but I'm currently doing it on a per-directory basis.
		
		# So, we first grab the directory name and check if it is a valid target.
		IF (TARGET ${mylibfolder})
			# OK, just apply the IMPORT_PREFIX
			SET_TARGET_PROPERTIES(${mylibfolder} PROPERTIES IMPORT_PREFIX "../" PREFIX "../")
			# ... no other actions for the moment :)
		ENDIF (TARGET ${mylibfolder})
		# If we don't have a regular target (e.g. SWIG target) the prefixes must 
		# be set within the CMakeLists.txt of the said target.
		
	ENDIF (WIN32)
	
ENDFOREACH (mylibfolder)

IF (FMPLUG_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF (FMPLUG_BUILD_TESTS)
IF (FMPLUG_BUILD_SAMPLES)
	ADD_SUBDIRECTORY(samples)
ENDIF (FMPLUG_BUILD_SAMPLES)
