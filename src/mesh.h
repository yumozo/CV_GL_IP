#pragma once

#include "renderer.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};
struct Texture {
    unsigned int id;
    std::string type;
};
class Mesh {
   public:
    /* Mesh data */
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;

    Mesh( std::vector<Vertex> vertices, std::vector<unsigned int> indices,
          std::vector<Texture> textures );

    void Draw( Shader &shader );

   private:
    unsigned int VAO, VBO, EBO;

    void SetupMesh();
}