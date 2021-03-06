cmake_minimum_required(VERSION 3.0.2)
project(libmbmeteor)

option(BUILD_EXAMPLE "Enables the example renderer." OFF)
option(BUILD_NDS "Builds the NDS core (currently WIP)." ON)

if (WIN32)
	message(STATUS "Operating system is Windows.")
elseif(UNIX AND NOT APPLE)
	message(STATUS "Operating system is Linux.")
elseif(APPLE)
	message(STATUS "Operating system is Mac.")
else()
	message(SEND_ERROR "Operating system not supported.")
	return()
endif()

set(LIBMBMETEOR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libmbmeteor/include")

set(LIBMBMETEOR_API_HEADERS
	libmbmeteor/include/libmbmeteor/libmbmeteor_api.h
	libmbmeteor/include/libmbmeteor/gba/mmu.h
	libmbmeteor/include/libmbmeteor/gba/cpu.h
	libmbmeteor/include/libmbmeteor/gba/gpu.h
	libmbmeteor/include/libmbmeteor/gba/apu.h
	libmbmeteor/include/libmbmeteor/gba/timers.h
	libmbmeteor/include/libmbmeteor/gba/input.h
	libmbmeteor/include/libmbmeteor/gba/serial.h
	libmbmeteor/include/libmbmeteor/nds/mmu.h
	libmbmeteor/include/libmbmeteor/nds/cpu.h
	libmbmeteor/include/libmbmeteor/libmbmeteor.h)

set(LIBMBMETEOR
	libmbmeteor/src/core/gba/mmu.cpp
	libmbmeteor/src/core/gba/cpu.cpp
	libmbmeteor/src/core/gba/gpu.cpp
	libmbmeteor/src/core/gba/apu.cpp
	libmbmeteor/src/core/gba/timers.cpp
	libmbmeteor/src/core/gba/input.cpp
	libmbmeteor/src/core/gba/serial.cpp
	libmbmeteor/src/core/nds/mmu.cpp
	libmbmeteor/src/core/nds/cpu.cpp
	libmbmeteor/src/core/libmbmeteor.cpp)

add_subdirectory(external)
add_definitions(-DBEEARM_ENABLE_THUMB)

if (BUILD_EXAMPLE STREQUAL "ON")
	add_subdirectory(example/sdl2)
endif()

if (BUILD_NDS STREQUAL "ON")
	add_subdirectory(example/nds)
endif()

if (WIN32)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -Wno-format-security -DLIBMBMETEOR_STATIC=1")
		message(STATUS "Compiler is MinGW.")
	else()
		message(SEND_ERROR "Compiler not supported.")
		return()
	endif()
elseif (UNIX AND NOT APPLE)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -Wno-format-security")
		message(STATUS "Compiler is GCC.")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -Wno-format-security")
		message(STATUS "Compiler is Clang.")
	else()
		message(SEND_ERROR "Compiler not supported.")
		return()
	endif()
elseif (APPLE)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -Wno-format-security")
		message(STATUS "Compiler is AppleClang.")
	else()
		message(SEND_ERROR "Compiler not supported.")
		return()
	endif()
endif()

if (WIN32)
	add_library(mbmeteor SHARED ${LIBMBMETEOR} ${LIBMBMETEOR_API_HEADERS})
else()
	add_library(mbmeteor STATIC ${LIBMBMETEOR} ${LIBMBMETEOR_API_HEADERS})
endif()

target_include_directories(mbmeteor PUBLIC ${LIBMBMETEOR_INCLUDE_DIR})
target_link_libraries(mbmeteor BeeARM)
add_library(libmbmeteor ALIAS mbmeteor)
