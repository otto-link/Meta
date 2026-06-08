if(META_ENABLE_QT_UI)
	file(GLOB TESTS LIST_DIRECTORIES true "*")

	foreach(item ${TESTS})
		if(IS_DIRECTORY ${item})
			add_subdirectory(${item})
		endif()
	endforeach()
endif()