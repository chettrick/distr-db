option(ENABLE_ASAN "address sanitizer" false)
option (ENABLE_UBSAN "undefined behaviour sanitizer" false)
option (ENABLE_LSAN "memory sanitizer" false)

if (ENABLE_ASAN)
	message("ASAN enabled")
	if(CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
		message("GNU or Clang detected")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	endif()
endif()

if (ENABLE_TSAN)
	message("ASAN enabled")
	if(CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
		message("GNU or Clang detected")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
	endif()
endif()

if (ENABLE_UBSAN)
	message("UBSAN enabled")
	if(CMAKE_CXX_COMPILER_ID  MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
		message("GNU or Clang detected")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	endif()
endif()

if (ENABLE_LSAN)
	message("LSAN enabled")
	if(CMAKE_CXX_COMPILER_ID  MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
		message("GNU or Clang detected")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
	endif()
endif()

