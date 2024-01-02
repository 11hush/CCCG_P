cmake_minimum_required(VERSION 3.0.0)
project(test_opengl)
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)

set(HEADER_DIR ${PROJECT_SOURCE_DIR})
include_directories(${HEADER_DIR})
include_directories("/usr/local/include/")
include_directories(${HEADER_DIR}/include/)

find_package(OpenGL REQUIRED)

find_package(glfw3 3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})

find_package(GLEW 2 REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-g -std=c++11 -Wformat")
link_directories(/usr/local/lib)
add_executable(test_opengl ${SRC_DIR}/main.cpp ${SRC_DIR}/glad.c)

message(STATUS "OpenGL library status:")
message(STATUS "    version: ${OPENGL_VERSION}")
message(STATUS "    libraries: ${OPENGL_LIBRARIES}")
message(STATUS "    include path: ${OPENGL_INCLUDE_DIRS}")

target_link_libraries(test_opengl 
    glfw
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLM_LIBRARIES}
)