#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

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

#include "tests/test_clear_color.h"
#include "tests/test_texture_2D.h"

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

        /* IMAGE PROCESSING */
        /* The sourse image -- original one container */
        cv::Mat srcImage = cv::imread( "res/textures/tiger.jpg" );
        /* The second container for the processed image */
        cv::Mat prcingImage = srcImage.clone();
        cv::Mat prcedImage;
        cv::Mat prcedImage1;

        /* Control slider -- testing */
        static float ctrlVl = 1;
        static int gbVal = 3;
        static int thrMinVal = 0;
        static int thrMaxVal = 255;
        static int cannyMinVal = 100;
        static int cannyMaxVal = 200;

        static bool srcImWin_IsShowing = false;
        static bool prcedImWin_IsShowing = false;
        static bool cannyIsDone = false;
        static bool sobelIsDone = false;

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
            ImGui::Text( "2D-Convolution Kernel" );
            ImGui::SliderFloat( "Identity (brightness)", &ctrlVl, 0, 1 );
            ImGui::Text( "Built-in functions" );
            ImGui::SliderInt( "Gaussian Blur", &gbVal, 3, 55 );
            ImGui::Columns( 2 );
            ImGui::SliderInt( "Threshold Min", &thrMinVal, 0, 255 );
            ImGui::NextColumn();
            ImGui::SliderInt( "Threshold Max", &thrMaxVal, 0, 255 );
            ImGui::NextColumn();
            ImGui::SliderInt( "Canny Min", &cannyMinVal, 0, 255 );
            ImGui::NextColumn();
            ImGui::SliderInt( "Canny Max", &cannyMaxVal, 0, 255 );
            ImGui::Columns( 1 );

            ImGui::Text( "Add checkboxes" );

            prcedImage = Filtering::Brightness( prcingImage, &ctrlVl );
            prcedImage1 = Filtering::GaussianBlur( prcedImage, &gbVal );
            prcedImage = Filtering::Thresholding(
                prcedImage1, &thrMinVal, &thrMaxVal, cv::THRESH_BINARY );

            if ( ImGui::Button( "Apply Sobel edge detection" ) &&
                 !sobelIsDone ) {
                /* Show the image in the separated window */
                Filtering::SobelEdgeDet( srcImage, 1, 1 );
                sobelIsDone = true;
            }

            if ( ImGui::Button( "Apply Canny edge detection" ) &&
                 !cannyIsDone ) {
                /* Show the image in the separated window */
                Filtering::CannyEdgeDet( srcImage, cannyMinVal,
                                         cannyMaxVal );
                cannyIsDone = true;
            }

            /* Final result (idk how to work with multiple filters tho) */
            // prcedImage = prcedImage + prcedImage1;

            /* Open/Close the Image Window */
            if ( ImGui::Button( "Open Source Image" ) ) {
                srcImWin_IsShowing = !srcImWin_IsShowing;
            }
            /* Source Image Window */
            if ( srcImWin_IsShowing ) {
                if ( !ImGui::Begin( "Source Image", &srcImWin_IsShowing ) ) {
                    ImGui::End();
                } else {
                    // if ( ImGui::Button( "Close OpenCV Window" ) &&
                    //      showMainCVWindow ) {
                    //     showMainCVWindow = !showMainCVWindow;
                    // }
                    GLuint texture;
                    GLCall( glGenTextures( 1, &texture ) );
                    GLCall( glBindTexture( GL_TEXTURE_2D, texture ) );
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
                            static_cast<intptr_t>( texture ) ),
                        ImVec2( srcImage.cols * .5f, srcImage.rows * .5f ),
                        ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
                    ImGui::End();
                }
            }
            /* THE WINDOW FOR THE PROCESSED IMAGE */
            if ( ImGui::Button( "Open Processed Image" ) ) {
                prcedImWin_IsShowing = !prcedImWin_IsShowing;
            }
            /* THE WINDOW FOR THE SOURCE IMAGE */
            if ( prcedImWin_IsShowing ) {
                if ( !ImGui::Begin( "Processing Image",
                                    &prcedImWin_IsShowing ) ) {
                    ImGui::End();
                } else {
                    // if ( ImGui::Button( "Close OpenCV Window" ) &&
                    //      showMainCVWindow ) {
                    //     showMainCVWindow = !showMainCVWindow;
                    // }
                    GLuint texture;
                    GLCall( glGenTextures( 1, &texture ) );
                    GLCall( glBindTexture( GL_TEXTURE_2D, texture ) );
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
                            static_cast<intptr_t>( texture ) ),
                        ImVec2( srcImage.cols * .5f, srcImage.rows * .5f ),
                        ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
                    ImGui::End();
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