CC = g++
FLAGS = -std=c++11 # -I.
# source
SOURCES = src/main.cpp src/index_buffer.cpp src/renderer.cpp \
src/vertex_buffer.cpp src/vertex_array.cpp src/shader.cpp src/texture.cpp\
src/tests/test_clear_color.cpp src/tests/test.cpp
# glad
SOURCES += src/glad.c 
# vendors
 # stb_image
SOURCES += src/vendor/stb_image/stb_image.cpp 
 # imgui
SOURCES += src/vendor/imgui/imgui_impl_glfw_gl3.cpp
SOURCES += src/vendor/imgui/imgui.cpp src/vendor/imgui/imgui_demo.cpp\
src/vendor/imgui/imgui_draw.cpp
# includes/libs
SOURCES += lib/libglfw3.a -Iinclude -lgdi32 -lopengl32

main: 
	$(CC) $(FLAGS) $(SOURCES) -o $@
	
#	g++ src/main.cpp src/index_buffer.cpp src/renderer.cpp src/vertex_buffer.cpp src/vertex_array.cpp src/glad.c lib/libglfw3.a -Iinclude -lgdi32 -lopengl32 -o main 