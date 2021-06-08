cmake_minimum_required(VERSION 3.12.4)

include(CTest)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS TARGETS)

add_executable(zip_utils_all_tests)

target_sources(zip_utils_all_tests PRIVATE tests.cpp)

target_include_directories(
        zip_utils_all_tests
        PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                $<BUILD_INTERFACE:${CONAN_INCLUDE_DIRS_CATCH2}/catch2>)


set_target_properties(
        zip_utils_all_tests PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

if(MSVC)
    target_compile_options(zip_utils_all_tests PRIVATE /WX)
else()
    target_compile_options(zip_utils_all_tests PRIVATE -Wall)
    target_compile_options(zip_utils_all_tests PRIVATE -Werror -Wno-error=uninitialized)
    target_compile_options(zip_utils_all_tests PRIVATE -Wpedantic)
endif()

add_test(NAME zip_utils_all_tests COMMAND zip_utils_all_tests)