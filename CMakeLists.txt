cmake_minimum_required(VERSION 3.16)

project(calc
  VERSION 0.1.0
  DESCRIPTION "A command-line calculator and expression library in C++17"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(CALC_BUILD_TESTS "Build the unit and CLI tests" ON)
option(CALC_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

add_library(calc_warnings INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(calc_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
  if(CALC_WARNINGS_AS_ERRORS)
    target_compile_options(calc_warnings INTERFACE -Werror)
  endif()
elseif(MSVC)
  target_compile_options(calc_warnings INTERFACE /W4)
  if(CALC_WARNINGS_AS_ERRORS)
    target_compile_options(calc_warnings INTERFACE /WX)
  endif()
endif()

configure_file(cmake/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/calc/version.h @ONLY)

file(GLOB CALC_CORE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(calc_core STATIC ${CALC_CORE_SOURCES})
add_library(calc::core ALIAS calc_core)
target_include_directories(calc_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(calc_core PRIVATE calc_warnings)

file(GLOB CALC_CLI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/apps/cli/*.cpp)
add_executable(calc ${CALC_CLI_SOURCES})
target_include_directories(calc PRIVATE apps/cli)
target_link_libraries(calc PRIVATE calc_core calc_warnings)

if(CALC_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
  enable_testing()
  add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(TARGETS calc calc_core
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/calc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/calc/version.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/calc)

set(CPACK_PACKAGE_NAME calc)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR TGZ)
include(CPack)
