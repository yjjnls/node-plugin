#
#
# example for plugin 
#
#
cmake_minimum_required(VERSION 2.8)
project(plugin)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
include_directories(converter)
add_library( converter SHARED converter/main.c)
set_target_properties(converter PROPERTIES VERSION 1.1 SOVERSION 1)

set(instd ${CMAKE_HOME_DIRECTORY}/../../bin)
if( MSVC )
	set(instd ${instd}/win )
else()
	set(instd ${instd}/linux )
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64 )
    set(instd "${instd}/x64")
else()
	set(instd "${instd}/x86")
endif()

install(TARGETS converter RUNTIME DESTINATION ${instd})
