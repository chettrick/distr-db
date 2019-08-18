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
## add_library(julia_set include/ra/julia_set.hpp)

# create test_queue executable
set_target_properties(queue PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(thread_pool PROPERTIES LINKER_LANGUAGE CXX)
## set_target_properties(julia_set PROPERTIES LINKER_LANGUAGE CXX)
include_directories("include" ${Boost_INCLUE_DIRS})
## add_executable(test_queue app/test_queue.cpp)
## add_executable(test_thread_pool app/test_thread_pool.cpp)
## add_executable(test_julia_set app/test_julia_set.cpp)

# create executable
add_executable(wl_generator app/wl_generator.cpp)

# link libraries
target_link_libraries(wl_generator pthread queue thread_pool)
## target_link_libraries(test_thread_pool queue thread_pool Catch2::Catch2)
## target_link_libraries(test_julia_set thread_pool Catch2::Catch2)



# link with pthread lib
# target_link_libraries(wl_generator pthread)

