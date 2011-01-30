cmake_minimum_required(VERSION 2.6)

project(t3-ftrace)

if (UNIX)
  if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")
  endif()
endif()

include_directories(includes)

file(
  GLOB_RECURSE
  source_files
  src/*.cpp
  )

add_executable(
  t3-ftrace
  ${source_files}
)

add_subdirectory(test)
