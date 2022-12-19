CC = g++
CFLAGS = -I.
DEPS = NONE
OBJ = NONE 

main: 
	$(CC) src/main.cpp src/index_buffer.cpp src/renderer.cpp src/vertex_buffer.cpp src/vertex_array.cpp src/shader.cpp src/glad.c lib/libglfw3.a -Iinclude -lgdi32 -lopengl32 -o $@
	
#	g++ src/main.cpp src/index_buffer.cpp src/renderer.cpp src/vertex_buffer.cpp src/vertex_array.cpp src/glad.c lib/libglfw3.a -Iinclude -lgdi32 -lopengl32 -o main 