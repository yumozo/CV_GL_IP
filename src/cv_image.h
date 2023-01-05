#pragma once

#include <opencv2/opencv.hpp>
#include "../include/imgui/imgui.h"

#include "texture.h"

#include <memory>

class CVImage {
   private:
    std::string m_Filepath;
    std::unique_ptr<Texture> m_Texture;
    cv::Mat m_Image;

   public:
    CVImage( const std::string &path );
    ~CVImage();

    void ShowImageInWindow() const;
};