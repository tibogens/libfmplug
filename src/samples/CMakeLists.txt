SET(ALL_SAMPLES
	basic
	popen
)

FOREACH (mysample ${ALL_SAMPLES})
    ADD_SUBDIRECTORY(${mysample})
ENDFOREACH (mysample)
