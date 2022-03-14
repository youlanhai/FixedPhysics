
set(TARGET_NAME physics)

set(SOURCE_FILES)
set(SUB_DIRS
	common
	debug
	math
	physics2d
)

foreach(sub_dir ${SUB_DIRS})
	file(GLOB SUB_FILES
		"${sub_dir}/*.hpp"
		"${sub_dir}/*.cpp"
	)

	string(REPLACE "/" "\\" group_name "${sub_dir}")
	source_group ("${group_name}\\" FILES ${SUB_FILES})

	list(APPEND SOURCE_FILES ${SUB_FILES})
endforeach()

add_library(${TARGET_NAME} STATIC  ${SOURCE_FILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(OUTPUT_PATH)
	set_target_properties(${TARGET_NAME}
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH}
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_PATH}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_PATH}
	)
endif()

install(TARGETS ${TARGET_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	BUNDLE DESTINATION bin
)
