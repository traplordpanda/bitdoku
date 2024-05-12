cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

# Bootstrap vcpkg
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg.cmake")
  message(STATUS "Bootstrapping vcpkg...")
  if(WIN32)
    execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat" "-disableMetrics" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg" RESULT_VARIABLE VCPKG_BOOTSTRAP_RESULT) 
  else()
    execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh" "-disableMetrics" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg" RESULT_VARIABLE VCPKG_BOOTSTRAP_RESULT)
  if(NOT VCPKG_BOOTSTRAP_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to bootstrap vcpkg: ${VCPKG_BOOTSTRAP_RESULT}")
  endif()
endif()

# Set the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(
    bitdoku
    VERSION 0.1.0
    DESCRIPTION "Soduko Bitboard with brute force backtracking solver"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    bitdoku_lib OBJECT
    source/bitdoku.cpp
)

target_include_directories(
    bitdoku_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(bitdoku_lib PUBLIC cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(bitdoku_lib PRIVATE fmt::fmt)

# ---- Declare executable ----

find_package(fmt CONFIG REQUIRED)

add_executable(bitdoku_exe source/main.cpp)
add_executable(bitdoku::exe ALIAS bitdoku_exe)

set_property(TARGET bitdoku_exe PROPERTY OUTPUT_NAME bitdoku)

target_compile_features(bitdoku_exe PRIVATE cxx_std_20)

target_link_libraries(bitdoku_exe PRIVATE bitdoku_lib fmt::fmt)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT bitdoku_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of bitdoku"
  )
endif()

include(cmake/dev-mode.cmake)
