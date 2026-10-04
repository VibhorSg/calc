# Use an installed GoogleTest when there is one (e.g. libgtest-dev), otherwise
# download a pinned release.
find_package(GTest 1.14 CONFIG QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    URL_HASH SHA256=b3e03aaaff81d68730d67392a135ac3fdfdf022880c66632ab188d2fe084cda3
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

file(GLOB CALC_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)
add_executable(calc_tests ${CALC_TEST_SOURCES})
target_link_libraries(calc_tests PRIVATE calc_core calc_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(calc_tests)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cli/CMakeLists.txt)
  add_subdirectory(cli)
endif()
