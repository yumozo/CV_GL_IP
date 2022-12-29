add_library(imgui imgui/imgui_impl_glfw_gl3.cpp imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp)
target_link_libraries(imgui PUBLIC glfw glad)
target_include_directories(imgui PUBLIC imgui)