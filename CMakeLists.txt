include(FetchContent)
cmake_minimum_required(VERSION 3.25.1)

project(ImageProcessingAttempt)

find_package(OpenCV REQUIRED)

set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.3.8)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 0.9.9.8)

FetchContent_MakeAvailable(glfw glm)
add_subdirectory(glad)
add_subdirectory(imgui)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(main src/main.cpp)

target_link_libraries(main ${OpenCV_LIBS} imgui glm)