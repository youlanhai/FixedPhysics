
cmake_minimum_required(VERSION 3.2)

project(FixedPhysics)

option(ENABLE_TEST 		"enable test" ON)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w44251 /wd4102 /utf-8")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w44251 /wd4102 /utf-8")
endif()

if(ENABLE_TEST)
	message(STATUS "ENABLE_TEST is ON")
	add_definitions(-DENABLE_TEST)
endif()

set(THIRD_PARTY_PATH "${PROJECT_SOURCE_DIR}/thirdparty")
set(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
set(OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(src)

if(ENABLE_TEST)
	add_subdirectory(thirdparty/glad)
	add_subdirectory(test/test-gui)
endif()
