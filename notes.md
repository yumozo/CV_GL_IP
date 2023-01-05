# OpenGL
How we draw things:
1. Bind our shader
2. Bind our vertex buffer
3. Setup the vertex layout
4. Bind our index buffer
5. Draw call

core:
1. Bind our shader
2. Bind our vertex array
4. Bind our index buffer
5. Draw call

*Vertex array objects are mandatory

# ImGui x OpenCV
To show an image in separate ImGui window

--- Abstraction ---
parameters

constructor(file_path)
- ImGui::Begin(*name of window*, *is show*)
- *file_path*
private
