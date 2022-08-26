cmake_minimum_required(VERSION 3.12.4)

project(example-project)

# Download automatically, you can also just copy the conan.cmake file
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(REQUIRES ZipUtils/0.1@zhekehz/stable
        BASIC_SETUP NO_OUTPUT_DIRS CMAKE_TARGETS)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CONAN_INCLUDE_DIRS})

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
