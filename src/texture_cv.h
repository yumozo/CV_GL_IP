#pragma once

#include <opencv2/opencv.hpp>
#include "renderer.h"

class Texture_CV {
   private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char *m_LocalBuffer;
    int m_Width, m_Height, m_BPP;

   public:
    Texture_CV(const cv::Mat image);
    ~Texture_CV();
    /* OpenGL state */
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;
    /* Size */
    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
};