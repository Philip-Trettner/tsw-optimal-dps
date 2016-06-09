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
