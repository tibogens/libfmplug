SET(ALL_TESTS
	basic
)

FOREACH (mytest ${ALL_TESTS})
    ADD_SUBDIRECTORY(${mytest})
ENDFOREACH (mytest)
