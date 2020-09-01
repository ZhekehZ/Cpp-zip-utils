cmake_minimum_required(VERSION 3.12.4)

include(CTest)

if (UNIX)
    set(CMAKE_CXX_COMPILER g++-10)
endif()

add_executable(zip_utils_all_tests)

target_sources(zip_utils_all_tests PRIVATE tests.cpp)

target_include_directories(
        zip_utils_all_tests
        PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

target_compile_features(zip_utils_all_tests PRIVATE cxx_std_20)

target_compile_options(
        zip_utils_all_tests
        PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4>
                $<$<CXX_COMPILER_ID:MSVC>:/WX>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Werror>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wpedantic>)

set(CMAKE_CXX_STANDARD 20)

add_test(NAME zip_utils_all_tests COMMAND zip_utils_all_tests)