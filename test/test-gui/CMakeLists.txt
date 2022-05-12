
set(TARGET_NAME TestPhysics)

file(GLOB SOURCE_FILES *.hpp *.cpp)

set(LIBRARIES physics glad glfw3)

include_directories(
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/thirdparty/glad/include"
	"${PROJECT_SOURCE_DIR}/thirdparty/glfw/include"
)

if(DARWIN)
	set(LIBRARY_PATH
		"${PROJECT_SOURCE_DIR}/thirdparty/glfw/lib-macos"
	)
	list(APPEND LIBRARIES
		"-framework Cocoa"
		"-framework IOKit"
		"-framework OpenGL"
	)
elseif(MSVC)
	set(GLFW_LIB 2019)
	if (MSVC_TOOLSET_VERSION EQUAL 110)
		set(GLFW_LIB 2012)
	elseif (MSVC_TOOLSET_VERSION EQUAL 120)
		set(GLFW_LIB 2013)
	elseif (MSVC_TOOLSET_VERSION EQUAL 140)
		set(GLFW_LIB 2015)
	elseif (MSVC_TOOLSET_VERSION EQUAL 141)
		set(GLFW_LIB 2017)
	endif()

	set(LIBRARY_PATH
		"${PROJECT_SOURCE_DIR}/thirdparty/glfw/lib-vc${GLFW_LIB}"
	)
endif()

link_directories(${LIBRARY_PATH})
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_link_libraries(${TARGET_NAME} ${LIBRARIES})

# set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")

if(OUTPUT_PATH)
	set_target_properties(${TARGET_NAME}
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH}
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_PATH}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_PATH}
	)
	if(MSVC)
		install(FILES
			"${PROJECT_SOURCE_DIR}/thirdparty/glfw/lib-vc${MSVC_VERSION}/glfw3.dll"
			DESTINATION ${OUTPUT_PATH})
	endif()
endif()

install(TARGETS ${TARGET_NAME}
	RUNTIME DESTINATION bin
	BUNDLE DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

set(CMAKE_DEFAULT_STARTUP_PROJECT ${TARGET_NAME})
