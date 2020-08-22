set(BEEARM_SOURCES
	arm/BeeARM/BeeARM/beearm.cpp)
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_library(BeeARM STATIC ${BEEARM_SOURCES})

target_include_directories(BeeARM PUBLIC
	arm/BeeARM)
