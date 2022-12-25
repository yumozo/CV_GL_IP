#pragma once

#include "test.h"

#include "../vertex_buffer.h"
#include "../vertex_buffer_layout.h"
#include "../texture.h"

#include <memory>

namespace test {
class TestTexture2D : public Test {
   public:
    TestTexture2D();
    ~TestTexture2D();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

   private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;

    glm::vec3 m_TranslationA, m_TranslationB;
};
}  // namespace test