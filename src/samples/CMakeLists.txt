SET(ALL_SAMPLES
	basic
)

FOREACH (mysample ${ALL_SAMPLES})
    ADD_SUBDIRECTORY(${mysample})
ENDFOREACH (mysample)
