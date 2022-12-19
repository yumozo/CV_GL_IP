#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
   private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
    // Caching for uniforms
   public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float f0, float f1, float v2,
                      float f3);

   private:
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader,
                              const std::string& fragmentShader);
    unsigned int GetUniformLocation(const std::string& name);
};