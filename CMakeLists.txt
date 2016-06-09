cmake_minimum_required(VERSION 3.0)
project(TswDps)

file(GLOB_RECURSE SRCS
    "TswDps/*.cc"
    "TswDps/*.hh"
    "TswDps/*.json"
    "TswDps/*.csv"
    )

add_executable(TswDps ${SRCS})

target_include_directories(TswDps PUBLIC "TswDps")

if (MSVC)
  target_compile_options(TswDps PUBLIC
    /wd4100
  )
  # Taken from gtest:
  # For MSVC, CMake sets certain flags to defaults we want to override.
  # This replacement code is taken from sample in the CMake Wiki at
  # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
  foreach (flag_var
           CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
           CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if (NOT BUILD_SHARED_LIBS AND NOT gtest_force_shared_crt)
      # When Google Test is built as a shared library, it should also use
      # shared runtime libraries.  Otherwise, it may end up with multiple
      # copies of runtime library data in different modules, resulting in
      # hard-to-find crashes. When it is built as a static library, it is
      # preferable to use CRT as static libraries, as we don't have to rely
      # on CRT DLLs being available. CMake always defaults to using shared
      # CRT libraries, so we override that default here.
      string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
    endif()

    # We prefer more strict warning checking for building Google Test.
    # Replaces /W3 with /W4 in defaults.
    string(REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
  endforeach()
else()
  target_compile_options(TswDps PUBLIC
    -std=c++11
    -Wno-unused-parameter
    -Wall
    -Werror
  )
endif()

add_subdirectory(extern/aion)
target_link_libraries(TswDps PUBLIC aion)

add_subdirectory(extern/googletest/googletest)
target_link_libraries(TswDps PUBLIC gtest)

find_package(Qt5Core REQUIRED)
target_link_libraries(TswDps PRIVATE ${Qt5Core_LIBRARIES})
