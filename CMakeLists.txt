include(FetchContent)
cmake_minimum_required(VERSION 3.25.1)
project(main)

# set(CMAKE_CXX_STANDARD 17)

# Vars
set(INC_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

# stb does not have a CMake buil, but is header-only
# find_path(STB_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include/stb_image")

# OpenGL Flags
# set(OpenGL_GL_PREFERENCE GLVND)

# GLFW
# option(GLFW_BUILD_DOCS OFF)
# option(GLFW_BUILD_EXAMPLES OFF)
# option(GLFW_BUILD_TESTS OFF)
# option(GLFW_INSTALL OFF)
# find_package(glfw3 REQUIRED)
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
# add_subdirectory( ${INC_DIR}/glfw )

# OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
include_directories( ${OpenCV_INCLUDE_DIRS} )

# glad
# add_library(glad
#   STATIC
#   ${INC_DIR}/glad/src/glad.c
# )

# target_include_directories(glad
#   PUBLIC
#   ${INC_DIR}/glad/include  
# )
# external includes
include_directories( SYSTEM ${INC_DIR}/glad/include )
include_directories( SYSTEM ${INC_DIR}/glm )
include_directories( SYSTEM ${INC_DIR}/imgui )
include_directories( SYSTEM ${INC_DIR}/stb_image )
include_directories( SYSTEM ${SRC_DIR} )
include_directories( SYSTEM ${SRC_DIR}/tests )

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.3.7)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 0.9.9.8)
FetchContent_MakeAvailable(glfw glm)

# Project
add_executable(main
  ${SRC_DIR}/main.cpp
  ${INC_DIR}/glad/src/glad.c

  ${SRC_DIR}/renderer.cpp
  ${SRC_DIR}/index_buffer.cpp
  ${SRC_DIR}/shader.cpp
  ${SRC_DIR}/texture.cpp
  ${SRC_DIR}/vertex_array.cpp
  ${SRC_DIR}/vertex_buffer.cpp
  ${SRC_DIR}/cv_image.cpp
  ${SRC_DIR}/filtering.cpp
  ${SRC_DIR}/custom_filtering.cpp

  ${INC_DIR}/stb_image/stb_image.cpp

  ${INC_DIR}/imgui/imgui.cpp
  ${INC_DIR}/imgui/imgui_draw.cpp
  ${INC_DIR}/imgui/imgui_impl_glfw_gl3.cpp

  ${SRC_DIR}/tests/test.cpp
  ${SRC_DIR}/tests/test_clear_color.cpp
  ${SRC_DIR}/tests/test_texture_2D.cpp
)

# target_link_libraries(main ${OpenCV_LIBS} imgui glm)
# target_include_directories(main
#   PRIVATE
#   ${INC_DIR}/glad/include
#   ${INC_DIR}/stb_image
# )

# target_sources(main
#   PRIVATE
#   ${SRC_DIR}/main.cpp  
# )

target_link_libraries(main
  # PRIVATE
    ${OpenCV_LIBS}
    glfw # fetching
    # glad
    # stb_image
    # imgui
    glm # fetching
    ${CMAKE_DL_LIBS}
)