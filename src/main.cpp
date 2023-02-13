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
#include "texture_cv.h"

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

        /* Test texture for the CV image window */
        TextureCV testCVTex( "res/textures/tiger.jpg" );

        /* IMAGE PROCESSING */
        GLuint srcImTexture;
        GLuint prcImTexture;
        GLuint hsvImTexture;
        GLCall( glGenTextures( 1, &srcImTexture ) );
        GLCall( glGenTextures( 1, &prcImTexture ) );
        GLCall( glGenTextures( 1, &hsvImTexture ) );
        /* The sourse image -- original one container */
        cv::Mat srcImage = cv::imread( "res/textures/tiger.jpg" );
        /* The second container for the processed image */
        cv::Mat prcingImage = srcImage.clone();
        cv::Mat prcedImage = srcImage.clone();
        cv::Mat prcedImage1;
        cv::Mat hsvImage = srcImage.clone();

        /* Filters parameters */
        static float brVl = 0;
        static float brVl1 = 1;
        static float cntVl = 1;
        static int gbVal = 3;
        static int mdbVal = 3;
        static int thrMinVal = 0;
        static int thrMaxVal = 255;
        static int cannyMinVal = 100;
        static int cannyMaxVal = 200;
        /* Filters on/off */
        static bool brightnessIsOn = false;
        static bool contrastIsOn = false;
        static bool gbIsOn = false;
        static bool mdbIsOn = false;
        static bool thrIsOn = false;
        /* HSV convert and edit */
        static float h = .2f;
        static float s = .2f;
        static float v = .2f;
        static float colors[3] = { h, s, v };
        /* Show/off hsv editable image */
        static bool hsvMenu_IsShowing = false;
        /* Show/off src, processed images */
        static bool cvTestWin_IsOpen = false;
        static bool srcImWin_IsShowing = false;
        static bool prcedImWin_IsShowing = false;
        /* Arithmetics */
        static char frstImPath[255] = "res/textures/tiger.jpg";
        static char scndImPath[255] = "res/textures/wc.jpg";
        char buf[255]{};
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
                    if ( ImGui::MenuItem( "Watercolor filter" ) ) {
                        cv::Mat resized_down;
                        double s = .5;
                        cv::resize( srcImage, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        CustomFiltering::Watercolor( &resized_down );
                    }
                    if ( ImGui::MenuItem( "Cartoon filter" ) ) {
                        cv::Mat resized_down;
                        double s = .5;
                        cv::resize( srcImage, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        CustomFiltering::Cartoon( &resized_down );
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
                    if ( ImGui::MenuItem( "Bitwise and" ) ) {
                        cv::Mat im1 = cv::imread( frstImPath );
                        cv::Mat im2 = cv::imread( scndImPath );
                        cv::Mat im2_resized;
                        cv::resize( im2, im2_resized,
                                    cv::Size( im1.cols, im1.rows ) );
                        if ( im1.empty() && im2.empty() ) {
                            printf( "Something wrong with your pics." );
                        } else {
                            cv::Mat result;
                            cv::bitwise_and( im1, im2_resized, result );
                            cv::imshow( "Addition func", result );
                        }
                    }
                    if ( ImGui::MenuItem( "Bitwise or" ) ) {
                        cv::Mat im1 = cv::imread( frstImPath );
                        cv::Mat im2 = cv::imread( scndImPath );
                        cv::Mat im2_resized;
                        cv::resize( im2, im2_resized,
                                    cv::Size( im1.cols, im1.rows ) );
                        if ( im1.empty() && im2.empty() ) {
                            printf( "Something wrong with your pics." );
                        } else {
                            cv::Mat result;
                            cv::bitwise_or( im1, im2_resized, result );
                            cv::imshow( "Addition func", result );
                        }
                    }
                    if ( ImGui::MenuItem( "Bitwise not" ) ) {
                        cv::Mat im1 = cv::imread( frstImPath );
                        cv::Mat im2 = cv::imread( scndImPath );
                        cv::Mat im2_resized;
                        cv::resize( im2, im2_resized,
                                    cv::Size( im1.cols, im1.rows ) );
                        if ( im1.empty() && im2.empty() ) {
                            printf( "Something wrong with your pics." );
                        } else {
                            cv::Mat result;
                            cv::bitwise_not( im1, result, result );
                            cv::imshow( "Addition func", result );
                        }
                    }
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "Open HSV edit" ) ) {
                        hsvMenu_IsShowing = !hsvMenu_IsShowing;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::Text( "Images to do with" );
            ImGui::InputText( "Image path", frstImPath,
                              sizeof( buf ) - 1 );
            ImGui::InputText( "Path to second image", scndImPath,
                              sizeof( buf ) - 1 );
            if ( ImGui::Button( "Load image" ) ) {
                /* Load new image */
                cv::Mat srcImage = cv::imread( frstImPath );
                /* Update variables */
                prcingImage = srcImage.clone();
                prcedImage = srcImage.clone();
                hsvImage = srcImage.clone();

                if ( srcImage.empty() ) {
                    printf( "Something wrong with your pics." );
                }
            }
            /* The window for the text CV texture */
            ImGui::Text( "Built-in functions" );
            ImGui::SliderFloat( "Brightness", &brVl, -255, 255 );
            ImGui::SliderFloat( "Contrast", &cntVl, 0.001, 5 );
            testCVTex.ContrastBrightness( &cntVl, &brVl );
            ImGui::Checkbox( "Gaussian Blur", &gbIsOn );
            if ( gbIsOn ) {
                ImGui::SliderInt( "Gaussian Blur", &gbVal, 3, 55 );

                testCVTex.GaussianBlur( &gbVal );
            }
            ImGui::Checkbox( "Median Blur", &mdbIsOn );
            if ( mdbIsOn ) {
                ImGui::SliderInt( "Median Blur", &mdbVal, 3, 55 );

                testCVTex.MedianBlur( &mdbVal );
            }
            ImGui::Checkbox( "Threshold", &thrIsOn );
            if ( thrIsOn ) {
                ImGui::Columns( 2 );
                ImGui::SliderInt( "Threshold Min", &thrMinVal, 0, 255 );
                ImGui::NextColumn();
                ImGui::SliderInt( "Threshold Max", &thrMaxVal, 0, 255 );
                ImGui::NextColumn();

                testCVTex.Threshold( &thrMinVal, &thrMaxVal,
                                     cv::THRESH_BINARY );
                /* don't remove this, there will be others */
                ImGui::Columns( 1 );
            }
            ImGui::Separator();
            ImGui::Text( "Edge detection" );
            ImGui::Columns( 2 );
            ImGui::SliderInt( "Canny Min", &cannyMinVal, 0, 255 );
            ImGui::NextColumn();
            ImGui::SliderInt( "Canny Max", &cannyMaxVal, 0, 255 );
            ImGui::Columns( 1 ); /* don't remove this, there will be others */

            /* THE WINDOW FOR THE SOURCE IMAGE */
            if ( ImGui::Button( "Open Source Image" ) ) {
                srcImWin_IsShowing = !srcImWin_IsShowing;
            }
            if ( srcImWin_IsShowing ) {
                if ( !ImGui::Begin( "Source Image", &srcImWin_IsShowing ) ) {
                    ImGui::End();
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
            if ( ImGui::Button( "Open Processed Image" ) ) {
                cvTestWin_IsOpen = !cvTestWin_IsOpen;
            }
            if ( cvTestWin_IsOpen ) {
                if ( !ImGui::Begin( "CV Texture test", &cvTestWin_IsOpen ) ) {
                    ImGui::End();
                } else {
                    testCVTex.OnImGuiRender();
                    ImGui::End();
                    GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
                }
            }
            /* HSV menu is shown/off (open with Color->HSV) */
            if ( hsvMenu_IsShowing ) {
                ImGui::Begin( "HSV editable", &hsvMenu_IsShowing );
                ImGui::ColorEdit3( "color", colors, ImGuiColorEditFlags_HSV );
                /* This &colors[0] gives RED channel instead of proper H */
                ImGui::SliderFloat( "H value of HSV", &colors[0], 0, 1 );

                GLCall( glBindTexture( GL_TEXTURE_2D, hsvImTexture ) );
                GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                         GL_LINEAR ) );
                GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                         GL_LINEAR ) );
                GLCall( glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ) );
                /* Don't use RGBA format, idk why but it doesn't do
                 * anything with it */
                GLCall( glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, hsvImage.cols,
                                      hsvImage.rows, 0, GL_RGB,
                                      /* pointer to the processing image */
                                      GL_UNSIGNED_BYTE, prcedImage.ptr() ) );
                ImGui::Image(
                    reinterpret_cast<void*>(
                        static_cast<intptr_t>( hsvImTexture ) ),
                    ImVec2( hsvImage.cols * .5f, hsvImage.rows * .5f ),
                    ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
                ImGui::End();
                GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
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