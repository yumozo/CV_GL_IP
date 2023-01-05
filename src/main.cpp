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
        cv::Mat sourceImage = cv::imread( "res/textures/somepic.png" );
        cv::Mat processingImage;
        /* Test control slider */
        static float ctrlVl = 1;
        static int gbVal = 3;

        static bool showSourceImageWindow = false;
        static bool showProcessedImageWindow = false;

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

            /* Identity kernel */
            cv::Mat testKernel =
                ( cv::Mat_<double>( 3, 3 ) << 0, 0, 0, 0, ctrlVl, 0, 0, 0, 0 );
            cv::filter2D( sourceImage, processingImage, -1, testKernel,
                          cv::Point( -1, -1 ), 4 );
            /* Built-in Gaussian Blur */
            if ( gbVal % 2 == 0 ) gbVal += 1;
            cv::GaussianBlur( sourceImage, processingImage,
                              cv::Size( gbVal, gbVal ), 0., 0. );

            /* Open/Close the Image Window */
            if ( ImGui::Button( "Open Source Image" ) ) {
                showSourceImageWindow = !showSourceImageWindow;
            }
            /* Source Image Window */
            if ( showSourceImageWindow ) {
                if ( !ImGui::Begin( "Source Image", &showSourceImageWindow ) ) {
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
                    GLCall( glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,
                                          sourceImage.cols, sourceImage.rows, 0,
                                          GL_RGB, GL_UNSIGNED_BYTE,
                                          sourceImage.ptr() ) );
                    ImGui::Image( reinterpret_cast<void*>(
                                      static_cast<intptr_t>( texture ) ),
                                  ImVec2( sourceImage.cols * .5f,
                                          sourceImage.rows * .5f ),
                                  ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
                    ImGui::End();
                }
            }
            /* PROCESSED IMAGE WINDOW */
            if ( ImGui::Button( "Open Processed Image" ) ) {
                showProcessedImageWindow = !showProcessedImageWindow;
            }
            /* Source Image Window */
            if ( showProcessedImageWindow ) {
                if ( !ImGui::Begin( "Processing Image",
                                    &showProcessedImageWindow ) ) {
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
                        GL_TEXTURE_2D, 0, GL_RGB, sourceImage.cols,
                        sourceImage.rows, 0, GL_RGB,
                        /* pointer to the processing image */
                        GL_UNSIGNED_BYTE, processingImage.ptr() ) );
                    ImGui::Image( reinterpret_cast<void*>(
                                      static_cast<intptr_t>( texture ) ),
                                  ImVec2( sourceImage.cols * .5f,
                                          sourceImage.rows * .5f ),
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