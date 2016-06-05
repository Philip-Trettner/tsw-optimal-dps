cmake_minimum_required(VERSION 3.0)
project(TswDps)

file(GLOB_RECURSE SRCS "*.cc" "*.hh" "*.json" "*.csv")

add_executable(TswDps ${SRCS})

if (MSVC)
  target_compile_options(TswDps PUBLIC
    /wd4100
  )
else()
  target_compile_options(TswDps PUBLIC
    -std=c++11
    -Wno-unused-parameter
  )
endif()

find_package(Qt5Core REQUIRED)
target_link_libraries(TswDps PRIVATE ${Qt5Core_LIBRARIES})
