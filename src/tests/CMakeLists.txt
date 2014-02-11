SET(ALL_TESTS
	func_def
)

FOREACH (mytest ${ALL_TESTS})
    ADD_SUBDIRECTORY(${mytest})
ENDFOREACH (mytest)
