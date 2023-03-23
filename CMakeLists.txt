cmake_minimum_required(VERSION 3.7...3.26)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Pullseq VERSION 1.0
		DESCRIPTION "Extract & Manipulate Sequence Files"
		LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#include(cmake/FindPCRE2.cmake)

add_subdirectory(src)

find_package(PCRE2 REQUIRED)
find_package(ZLIB REQUIRED)

if(PCRE2_FOUND)
	target_include_directories(pullseq PUBLIC ${PCRE2_INCLUDE_DIRS})
	target_link_libraries(pullseq ${PCRE2_LIBRARIES})
endif(PCRE2_FOUND)

target_link_libraries(pullseq z)
target_link_libraries(seqdiff z)

