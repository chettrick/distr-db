cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(wl_generator LANGUAGES CXX)


# outputs compile_commands.json, used by YCM autocompleter for Vim
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 17)

# Used to enable debugging (gdb)
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
		add_compile_options("-g")
endif()

# Enables undefined behaviour and address sanitizers
include(Sanitizers.cmake)

add_library(queue include/ra/queue.hpp)
add_library(thread_pool lib/thread_pool.cpp)

# create test_queue executable
set_target_properties(queue PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(thread_pool PROPERTIES LINKER_LANGUAGE CXX)
include_directories("include" ${Boost_INCLUE_DIRS})

# create executable
add_executable(wl_generator app/wl_generator.cpp)

# find_package(SFML 2 REQUIRED network audio graphics window system)
# if(SFML_FOUND)
# 	include_directories(${SFML_INCLUDE_DIR})
# 	target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
# endif()

# link libraries
target_link_libraries(wl_generator pthread queue thread_pool curl)

