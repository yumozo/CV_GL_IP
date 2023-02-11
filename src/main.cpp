#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw_gl3.h>
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw_gl3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "renderer.h"

#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"
#include "cv_image.h"
#include "filtering.h"
#include "custom_filtering.h"

#include "tests/test_clear_color.h"
#include "tests/test_texture_2D.h"

/* Remove this if the VC Image class has implemeted */
#include "../include/stb_image/stb_image.h"

// using namespace cv;

int main( void ) {
    /* Initialize the library */
    if ( !glfwInit() ) return -1;
    GLFWwindow* window;
    /* GL version setting */
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow( 960, 540, "Hello World", NULL, NULL );
    if ( !window ) {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent( window );
    /* Frame rate setting */
    glfwSwapInterval( 1 );

    /* init glad  */
    gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );

    // std::cout << glGetString(GL_VERSION) << std::endl;
    {
        GLCall( glEnable( GL_BLEND ) );
        GLCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );

        Renderer renderer;

        /* Init ImGui, setup */
        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init( window, true );
        ImGui::StyleColorsDark();

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu( currentTest );
        currentTest = testMenu;

        testMenu->RegisterTest<test::TestClearColor>( "Clear Color" );
        testMenu->RegisterTest<test::TestTexture2D>( "2D Texture" );
        // testMenu->RegisterTest<test:Testcvimage>

        /* IMAGE PROCESSING */
        GLuint srcImTexture;
        GLuint prcImTexture;
        GLCall( glGenTextures( 1, &srcImTexture ) );
        GLCall( glGenTextures( 1, &prcImTexture ) );
        /* The sourse image -- original one container */
        cv::Mat srcImage = cv::imread( "res/textures/tiger.jpg" );
        /* The second container for the processed image */
        cv::Mat prcingImage = srcImage.clone();
        cv::Mat prcedImage = srcImage.clone();
        cv::Mat prcedImage1;

        /* Control slider -- testing */
        static float brVl = 1;
        static float cntVl = 1;
        static int gbVal = 3;
        static int mdbVal = 3;
        static int thrMinVal = 0;
        static int thrMaxVal = 255;
        static int cannyMinVal = 100;
        static int cannyMaxVal = 200;

        static bool brightnessIsOn = false;
        static bool contrastIsOn = false;
        static bool gbIsOn = false;
        static bool mdbIsOn = false;
        static bool thrIsOn = false;

        static bool srcImWin_IsShowing = false;
        static bool prcedImWin_IsShowing = false;

        /* Loop until the user closes the window */
        while ( !glfwWindowShouldClose( window ) ) {
            GLCall( glClearColor( 0.f, 0.f, 0.f, 1.f ) );
            /* Render here */
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();
            if ( currentTest ) {
                currentTest->OnUpdate( 0.0f );
                currentTest->OnRender();
                ImGui::Begin( "Test" );
                if ( currentTest != testMenu && ImGui::Button( "<-" ) ) {
                    delete currentTest;
                    currentTest = testMenu;
                }
                currentTest->OnImGuiRender();
                ImGui::End();
            }
            /* OpenCV image in ImGui window */
            /* Image Processing Window */
            ImGui::Begin( "Image Processing" );
            /* Options */
            if ( ImGui::BeginMainMenuBar() ) {
                if ( ImGui::BeginMenu( "File" ) ) {
                    ImGui::EndMenu();
                }
                if ( ImGui::BeginMenu( "Edit" ) ) {
                    if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) ) {
                    }
                    if ( ImGui::MenuItem( "Redo", "CTRL+Y" ) ) {
                    }
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "Cut", "CTRL+X" ) ) {
                    }
                    if ( ImGui::MenuItem( "Copy", "CTRL+C" ) ) {
                    }
                    if ( ImGui::MenuItem( "Paste", "CTRL+V" ) ) {
                    }
                    ImGui::EndMenu();
                }
                /* FILTERS */
                if ( ImGui::BeginMenu( "Filter" ) ) {
                    if ( ImGui::MenuItem( "B&W" ) ) {
                        cv::Mat img_gray;
                        cv::cvtColor( srcImage, img_gray, cv::COLOR_BGR2GRAY );
                        cv::Mat resized_down;
                        double s = .5;
                        cv::resize( img_gray, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        cv::imshow( "B&W", resized_down );
                    }
                    ImGui::Separator();
                    ImGui::Text( "--ADD GAUSSIAN BLUR--" );
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "Sobel edge detection map" ) ) {
                        cv::Mat resized_down;
                        /* Should be controlled parameters */
                        double s = .6;
                        cv::resize( srcImage, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        /* x & y should be controlled parameters */
                        int x = 1;
                        int y = 1;
                        Filtering::SobelEdgeDet( resized_down, x, y );
                    }
                    if ( ImGui::MenuItem( "Canny edge detection map" ) ) {
                        cv::Mat resized_down;
                        double s = .6;
                        cv::resize( srcImage, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        /* min & max vals should be controlled parameters */
                        Filtering::CannyEdgeDet( resized_down, cannyMinVal,
                                                 cannyMaxVal );
                    }
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "Sepia filter" ) ) {
                        cv::Mat resized_down;
                        double s = .5;
                        cv::resize( srcImage, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        CustomFiltering::Sepia( &resized_down );
                    }
                    ImGui::EndMenu();
                }
                /* COLORS */
                if ( ImGui::BeginMenu( "Color" ) ) {
                    if ( ImGui::MenuItem( "Get R values" ) ) {
                        std::vector<cv::Mat> chans( 3 );
                        cv::split( srcImage, chans );
                        cv::Mat resized_down;
                        cv::resize( chans[2], resized_down, cv::Size(), .5, .5,
                                    cv::INTER_LINEAR );
                        cv::imshow( "Green ch only", resized_down );
                    }
                    if ( ImGui::MenuItem( "Get G values" ) ) {
                        std::vector<cv::Mat> chans( 3 );
                        cv::split( srcImage, chans );
                        cv::Mat resized_down;
                        cv::resize( chans[1], resized_down, cv::Size(), .5, .5,
                                    cv::INTER_LINEAR );
                        cv::imshow( "Green ch only", resized_down );
                    }
                    if ( ImGui::MenuItem( "Get B values" ) ) {
                        std::vector<cv::Mat> chans( 3 );
                        cv::split( srcImage, chans );
                        cv::Mat resized_down;
                        cv::resize( chans[0], resized_down, cv::Size(), .5, .5,
                                    cv::INTER_LINEAR );
                        cv::imshow( "Green ch only", resized_down );
                    }
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "To HSV" ) ) {
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::Text( "2D-Convolution Kernel" );
            ImGui::SliderFloat( "Brightness", &brVl, 0, 3 );
            prcedImage = Filtering::Brightness( &prcingImage, &brVl );
            ImGui::Text( "Built-in functions" );
            ImGui::SliderFloat( "Contrast", &cntVl, 0.001, 5 );
            prcedImage = Filtering::Contrast( &prcedImage, &cntVl );
            ImGui::Checkbox( "Gaussian Blur", &gbIsOn );
            if ( gbIsOn ) {
                ImGui::SliderInt( "Gaussian Blur", &gbVal, 3, 55 );

                prcedImage = Filtering::GaussianBlur( &prcedImage, &gbVal );
            }
            ImGui::Checkbox( "Median Blur", &mdbIsOn );
            if ( mdbIsOn ) {
                ImGui::SliderInt( "Median Blur", &mdbVal, 3, 55 );

                prcedImage = Filtering::MedianBlur( &prcedImage, &mdbVal );
            }
            ImGui::Checkbox( "Threshold", &thrIsOn );
            if ( thrIsOn ) {
                ImGui::Columns( 2 );
                ImGui::SliderInt( "Threshold Min", &thrMinVal, 0, 255 );
                ImGui::NextColumn();
                ImGui::SliderInt( "Threshold Max", &thrMaxVal, 0, 255 );
                ImGui::NextColumn();

                prcedImage = Filtering::Thresholding(
                    &prcedImage, &thrMinVal, &thrMaxVal, cv::THRESH_BINARY );
                /* don't remove this, there will be others */
                ImGui::Columns( 1 );
            }
            ImGui::Separator();
            ImGui::Columns( 2 );
            ImGui::SliderInt( "Canny Min", &cannyMinVal, 0, 255 );
            ImGui::NextColumn();
            ImGui::SliderInt( "Canny Max", &cannyMaxVal, 0, 255 );
            ImGui::Columns( 1 ); /* don't remove this, there will be others */

            // prcedImage = Filtering::Brightness( &prcingImage, &brVl );
            // prcedImage1 = Filtering::Contrast( &prcingImage, &cntVl );
            // prcedImage = Filtering::GaussianBlur( &prcedImage1, &gbVal );
            // prcedImage1 = Filtering::MedianBlur( &prcedImage, &mdbVal );
            // prcedImage = Filtering::Thresholding(
            //     &prcedImage1, &thrMinVal, &thrMaxVal, cv::THRESH_BINARY );

            /* THE WINDOW FOR THE SOURCE IMAGE */
            if ( ImGui::Button( "Open Source Image" ) ) {
                srcImWin_IsShowing = !srcImWin_IsShowing;
            }
            if ( srcImWin_IsShowing ) {
                if ( !ImGui::Begin( "Source Image", &srcImWin_IsShowing ) ) {
                    // ImGui::End();
                } else {
                    // GLCall( glGenTextures( 1, &texture ) );
                    GLCall( glBindTexture( GL_TEXTURE_2D, srcImTexture ) );
                    GLCall( glTexParameteri(
                        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
                    GLCall( glTexParameteri(
                        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
                    GLCall( glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ) );
                    /* Don't use RGBA format, idk why but it doesn't do
                     * anything with it */
                    GLCall( glTexImage2D(
                        GL_TEXTURE_2D, 0, GL_RGB, srcImage.cols, srcImage.rows,
                        0, GL_RGB, GL_UNSIGNED_BYTE, srcImage.ptr() ) );
                    ImGui::Image(
                        reinterpret_cast<void*>(
                            static_cast<intptr_t>( srcImTexture ) ),
                        ImVec2( srcImage.cols * .5f, srcImage.rows * .5f ),
                        ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
                    ImGui::End();
                    GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );

                    // if ( srcImage.ptr() ) free( srcImage.ptr() );
                }
            }
            /* THE WINDOW FOR THE PROCESSED IMAGE */
            if ( ImGui::Button( "Open Processed Image" ) ) {
                prcedImWin_IsShowing = !prcedImWin_IsShowing;
            }
            if ( prcedImWin_IsShowing ) {
                if ( !ImGui::Begin( "Processing Image",
                                    &prcedImWin_IsShowing ) ) {
                    // ImGui::End();
                } else {
                    // GLCall( glGenTextures( 1, &texture ) );
                    GLCall( glBindTexture( GL_TEXTURE_2D, prcImTexture ) );
                    GLCall( glTexParameteri(
                        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
                    GLCall( glTexParameteri(
                        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
                    GLCall( glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ) );
                    /* Don't use RGBA format, idk why but it doesn't do
                     * anything with it */
                    GLCall(
                        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, srcImage.cols,
                                      srcImage.rows, 0, GL_RGB,
                                      /* pointer to the processing image */
                                      GL_UNSIGNED_BYTE, prcedImage.ptr() ) );
                    ImGui::Image(
                        reinterpret_cast<void*>(
                            static_cast<intptr_t>( prcImTexture ) ),
                        ImVec2( srcImage.cols * .5f, srcImage.rows * .5f ),
                        ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
                    ImGui::End();
                    GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );

                    // if ( prcedImage.ptr() ) free( prcedImage.ptr() );
                }
            }

            ImGui::End();

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData( ImGui::GetDrawData() );
            /* Swap front and back buffers */
            glfwSwapBuffers( window );

            /* Poll for and process events */
            glfwPollEvents();
        }
        delete currentTest;
        if ( currentTest != testMenu ) {
            delete testMenu;
        }
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}