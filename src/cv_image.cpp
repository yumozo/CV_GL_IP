#include "cv_image.h"

CVImage::CVImage( const std::string& path ) {
    m_Filepath = "res/textures/somepic.png";
    m_Image = cv::imread( m_Filepath, 1 );
    m_Texture = std::make_unique<Texture>( m_Filepath );
    // if ( !image.data ) {
    //     std::cout << "No image data \n" << std::endl;
    //     return -1;
    // }
}

CVImage::~CVImage() {}

void CVImage::ShowImageInWindow() const {
    // ImGui::Begin( "Image" );  //, &is_show );
    // ImGui::Image( reinterpret_cast<void*>(
    //                   static_cast<intptr_t>( m_Texture ) ),
    //               ImVec2( m_Image.cols, m_Image.rows ) );

    /* Create a window */
    // cv::namedWindow( "Display Image", cv::WINDOW_AUTOSIZE );
    /* Show */
    // cv::imshow( "Display Image", m_Image );
    // cv::destroyAllWindows();

    // ImGui::End();
}
