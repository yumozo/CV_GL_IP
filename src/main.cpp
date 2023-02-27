#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw_gl3.h>
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"

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
#include "filtering.h"
#include "custom_filtering.h"
#include "texture_cv.h"

#include "tests/test_clear_color.h"
#include "tests/test_texture_2D.h"
#include "tests/test_3D.h"

/* Remove this if the VC Image class has implemeted */
#include "../include/stb_image/stb_image.h"

// using namespace cv;

int main( void ) {
    /* Initialize the library */
    if ( !glfwInit() ) return -1;
    GLFWwindow* window;
    /* GL version setting */
    const char* glsl_version = "#version 130";
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow( 1280, 720, "Hello World", NULL, NULL );
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
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
        io.ConfigFlags |=
            ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport /
                                               // Platform Windows
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so
        // platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL( window, true );
        ImGui_ImplOpenGL3_Init( glsl_version );

        // Tests
        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu( currentTest );
        currentTest = testMenu;

        testMenu->RegisterTest<test::TestClearColor>( "Clear Color" );
        testMenu->RegisterTest<test::TestTexture2D>( "2D Texture" );
        testMenu->RegisterTest<test::Test3D>( "Test 3D" );

        /* Test texture for the CV image window */
        TextureCV inCVTex( "res/textures/tiger.jpg" );
        TextureCV outCVTex( "res/textures/tiger.jpg" );
        TextureCV hsvCVTex( "res/textures/tiger.jpg" );

        /* IMAGE PROCESSING */
        GLuint srcImTexture;
        GLuint prcImTexture;
        GLuint hsvImTexture;
        GLCall( glGenTextures( 1, &srcImTexture ) );
        GLCall( glGenTextures( 1, &prcImTexture ) );
        GLCall( glGenTextures( 1, &hsvImTexture ) );
        /* The sourse image -- original one container */
        struct Images {
            cv::Mat src = cv::imread( "res/textures/tiger.jpg" );
            cv::Mat processing = src.clone();
            cv::Mat processed = src.clone();
            cv::Mat hsv = src.clone();
        } images;

        /* Filters parameters */
        static float brVl = 0;
        static float cntVl = 1;
        static int gbVal = 3;
        static int mdbVal = 3;
        static int thrMinVal = 0;
        static int thrMaxVal = 255;
        static int cannyMinVal = 100;
        static int cannyMaxVal = 200;
        /* Filters on/off */
        static bool brightChange_IsOn = false;
        static bool contrastChange_IsOn = false;
        static bool gb_IsOn = false;
        static bool mdb_IsOn = false;
        static bool thr_IsOn = false;
        struct hsvVal {
            int h = 0;
            int s = 0;
            int v = 0;
        } hsv_values;
        /* EDIT GUI MENUES */
        static bool resizeMenu_IsShown = false;
        static bool gbMenu_IsShown = false;
        static bool bcMenu_IsShown = false;
        static bool mbMenu_IsShown = false;
        static bool hsvMenu_IsShown = false;
        static bool cedMenu_IsShown = false;
        static bool sedMenu_IsShown = false;
        static bool ckMenu_IsShown = false;
        static bool thrMenu_IsShown = false;
        /* Show/off src, processed images */
        static bool cvTestWin_IsShown = false;
        static bool srcImWin_IsShown = false;
        static bool prcedImWin_IsShowing = false;
        /* Preset Kernels */
        const char* kernels[] = { "blur",     "bottom sobel", "custom",
                                  "emboss",   "identity",     "left sobel",
                                  "outline",  "right sobel",  "sharpen",
                                  "top sobel" };
        static const char* current_kernel = NULL;

        static char frstImPath[255] = "res/textures/tiger.jpg";
        static char scndImPath[255] = "res/textures/wc.jpg";
        char buf[255]{};

        struct convKernel {
            float Row1[3]{ 0., 0., 0. };
            float Row2[3]{ 0., 0., 0. };
            float Row3[3]{ 0., 0., 0. };
        };
        struct IdKernel {
            float Row1[3]{ 0., 0., 0. };
            float Row2[3]{ 0., 1., 0. };
            float Row3[3]{ 0., 0., 0. };
        };
        struct blurKernel {
            float Row1[3]{ .25 / 4, .25 / 2, .25 / 4 };
            float Row2[3]{ .25 / 2, .25, .25 / 2 };
            float Row3[3]{ .25 / 4, .25 / 2, .25 / 4 };
        };
        struct bSobelKernel {
            float Row1[3]{ -1., -2., -1. };
            float Row2[3]{ 0., 0., 0. };
            float Row3[3]{ 1., 2., 1. };
        };
        struct tSobelKernel {
            float Row3[3]{ 1., 2., 1. };
            float Row2[3]{ 0., 0., 0. };
            float Row1[3]{ -1., -2., -1. };
        };
        struct rSobelKernel {
            float Row3[3]{ -1., 0., 1. };
            float Row2[3]{ -2., 0., 2. };
            float Row1[3]{ -1., 0., 1. };
        };
        struct lSobelKernel {
            float Row3[3]{ 1., 0., -1. };
            float Row2[3]{ 2., 0., -2. };
            float Row1[3]{ 1., 0., -1. };
        };
        struct embossKernel {
            float Row3[3]{ -2., -1., 0. };
            float Row2[3]{ -1., 1., 1. };
            float Row1[3]{ 0., 1., 2. };
        };
        struct outlineKernel {
            float Row3[3]{ -1., -1., -1. };
            float Row2[3]{ -1., 8., -1. };
            float Row1[3]{ -1., -1., -1. };
        };
        struct sharpenKernel {
            float Row3[3]{ 0., -1., 0. };
            float Row2[3]{ -1., 5., -1. };
            float Row1[3]{ 0., -1., 0. };
        };
        convKernel ck;
        /* Loop until the user closes the window */
        while ( !glfwWindowShouldClose( window ) ) {
            GLCall( glClearColor( 0.f, 0.f, 0.f, 1.f ) );
            /* Render here */
            renderer.Clear();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
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
                    if ( ImGui::MenuItem( "Open", "CTRL+O" ) ) {
                        /**/
                    }
                    if ( ImGui::MenuItem( "Save", "CTRL+S" ) ) {
                        /**/
                    }
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "Save As...", "SHIFT+CTRL+S" ) ) {
                        /**/
                    }
                    if ( ImGui::MenuItem( "Exit", "CTRL+Q" ) ) {
                        printf( "The program could be terminated.\n" );
                    }
                    ImGui::EndMenu();
                }
                if ( ImGui::BeginMenu( "Edit" ) ) {
                    if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) ) {
                        /**/
                    }
                    if ( ImGui::MenuItem( "Redo", "CTRL+Y" ) ) {
                        /**/
                    }
                    ImGui::Separator();
                    if ( ImGui::MenuItem( "Cut", "CTRL+X" ) ) {
                        /**/
                    }
                    if ( ImGui::MenuItem( "Copy", "CTRL+C" ) ) {
                        /**/
                    }
                    if ( ImGui::MenuItem( "Paste", "CTRL+V" ) ) {
                        /**/
                    }
                    ImGui::EndMenu();
                }
                /* Operations with the image */
                if ( ImGui::BeginMenu( "Image" ) ) {
                    if ( ImGui::MenuItem( "Resize..." ) ) {
                        resizeMenu_IsShown = !resizeMenu_IsShown;
                    }
                    ImGui::EndMenu();
                }
                /* IMAGE ADJUSTMENTS */
                if ( ImGui::BeginMenu( "Adj" ) ) {
                    if ( ImGui::MenuItem( "Brightness/Contrast" ) ) {
                        bcMenu_IsShown = !bcMenu_IsShown;
                    }
                    ImGui::EndMenu();
                }
                /* FILTERS */
                if ( ImGui::BeginMenu( "Filter" ) ) {
                    if ( ImGui::MenuItem( "B&W" ) ) {
                        cv::Mat img_gray;
                        cv::cvtColor( images.src, img_gray,
                                      cv::COLOR_BGR2GRAY );
                        cv::Mat resized_down;
                        double s = .5;
                        cv::resize( img_gray, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        cv::imshow( "B&W", resized_down );
                    }
                    if ( ImGui::BeginMenu( "Blur" ) ) {
                        if ( ImGui::MenuItem( "Gaussian Blur" ) ) {
                            gbMenu_IsShown = !gbMenu_IsShown;
                        }
                        if ( ImGui::MenuItem( "Median Blur" ) ) {
                            mbMenu_IsShown = !mbMenu_IsShown;
                        }
                        ImGui::EndMenu();
                    }
                    if ( ImGui::BeginMenu( "Custom Filters" ) ) {
                        if ( ImGui::MenuItem( "Sepia filter" ) ) {
                            cv::Mat resized_down;
                            double s = .5;
                            cv::resize( images.src, resized_down, cv::Size(), s,
                                        s, cv::INTER_LINEAR );
                            CustomFiltering::Sepia( &resized_down );
                        }
                        if ( ImGui::MenuItem( "Watercolor filter" ) ) {
                            cv::Mat resized_down;
                            double s = .5;
                            cv::resize( images.src, resized_down, cv::Size(), s,
                                        s, cv::INTER_LINEAR );
                            CustomFiltering::Watercolor( &resized_down );
                        }
                        if ( ImGui::MenuItem( "Cartoon filter" ) ) {
                            cv::Mat resized_down;
                            double s = .5;
                            cv::resize( images.src, resized_down, cv::Size(), s,
                                        s, cv::INTER_LINEAR );
                            CustomFiltering::Cartoon( &resized_down );
                        }
                        ImGui::EndMenu();
                    }
                    if ( ImGui::MenuItem( "Custom Kernel" ) ) {
                        ckMenu_IsShown = !ckMenu_IsShown;
                    }
                    ImGui::EndMenu();
                }
                /* COLORS */
                if ( ImGui::BeginMenu( "Color" ) ) {
                    if ( ImGui::BeginMenu( "Get color values" ) ) {
                        if ( ImGui::MenuItem( "Get Red" ) ) {
                            std::vector<cv::Mat> chans( 3 );
                            cv::split( images.src, chans );
                            cv::Mat resized_down;
                            cv::resize( chans[2], resized_down, cv::Size(), .5,
                                        .5, cv::INTER_LINEAR );
                            cv::imshow( "Green ch only", resized_down );
                        }
                        if ( ImGui::MenuItem( "Get Green" ) ) {
                            std::vector<cv::Mat> chans( 3 );
                            cv::split( images.src, chans );
                            cv::Mat resized_down;
                            cv::resize( chans[1], resized_down, cv::Size(), .5,
                                        .5, cv::INTER_LINEAR );
                            cv::imshow( "Green ch only", resized_down );
                        }
                        if ( ImGui::MenuItem( "Get Blue" ) ) {
                            std::vector<cv::Mat> chans( 3 );
                            cv::split( images.src, chans );
                            cv::Mat resized_down;
                            cv::resize( chans[0], resized_down, cv::Size(), .5,
                                        .5, cv::INTER_LINEAR );
                            cv::imshow( "Green ch only", resized_down );
                        }
                        ImGui::EndMenu();
                    }
                    if ( ImGui::BeginMenu( "Bitwise operations" ) ) {
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
                        ImGui::EndMenu();
                    }
                    if ( ImGui::BeginMenu( "Edge Detection" ) ) {
                        if ( ImGui::MenuItem( "Canny" ) ) {
                            cedMenu_IsShown = !cedMenu_IsShown;
                        }
                        if ( ImGui::MenuItem( "Sobel" ) ) {
                            cv::Mat resized_down;
                            /* Should be controlled parameters */
                            double s = .6;
                            cv::resize( images.src, resized_down, cv::Size(), s,
                                        s, cv::INTER_LINEAR );
                            /* x & y should be controlled parameters */
                            int x = 1;
                            int y = 1;
                            Filtering::SobelEdgeDet( resized_down, x, y );
                        }
                        ImGui::EndMenu();
                    }
                    if ( ImGui::MenuItem( "Open HSV edit" ) ) {
                        hsvMenu_IsShown = !hsvMenu_IsShown;
                    }
                    if ( ImGui::MenuItem( "Threshold" ) ) {
                        thrMenu_IsShown = !thrMenu_IsShown;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::Text( "Images to do with" );
            ImGui::InputText( "Image path", frstImPath, sizeof( buf ) - 1 );
            ImGui::InputText( "Path to second image", scndImPath,
                              sizeof( buf ) - 1 );
            /* Doesn't work */
            if ( ImGui::Button( "Load images" ) ) {
                /* Load new image */
                cv::Mat srcImage = cv::imread( frstImPath );
                /* Update variables */
                images.processing = srcImage.clone();
                images.processed = srcImage.clone();
                images.hsv = srcImage.clone();

                if ( srcImage.empty() ) {
                    printf( "Something wrong with your pics." );
                }
            }

            ImGui::Separator();
            /* ADDITIONAL MENUES */
            /* The window for the source image */
            if ( ImGui::Button( "Show Source Image" ) ) {
                srcImWin_IsShown = !srcImWin_IsShown;
            }
            if ( srcImWin_IsShown ) {
                if ( !ImGui::Begin( "Source Image", &srcImWin_IsShown ) ) {
                    ImGui::End();
                } else {
                    inCVTex.OnImGuiRender();
                    ImGui::End();
                    // GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
                }
            }
            ImGui::SameLine();
            /* The window for the processed image */
            if ( ImGui::Button( "Show Processed Image" ) ) {
                cvTestWin_IsShown = !cvTestWin_IsShown;
            }
            if ( cvTestWin_IsShown ) {
                if ( !ImGui::Begin( "CV Texture test", &cvTestWin_IsShown ) ) {
                    ImGui::End();
                } else {
                    outCVTex.OnImGuiRender();
                    ImGui::End();
                    // GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
                }
            }

            /* MainMenu -> Image -> */
            /* The Resize image menu */
            if ( resizeMenu_IsShown ) {
                if ( !ImGui::Begin( "Resize", &resizeMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    /* Parameters*/
                    static int width = 320;
                    static int height = 200;
                    static bool byPrc = false;
                    static int sizeByPrc = 100;
                    static bool byAbs = true;
                    static bool aspRat = false;

                    ImGui::Text( "Resampling: " );
                    ImGui::SameLine( 200 );
                    ImGui::Text( "*here will be a combobox*" );
                    // ImGui::combo();

                    ImGui::Checkbox( "By percentage: ", &byPrc );
                    if ( byPrc ) {
                        byAbs = false;
                        ImGui::SameLine( 200 );
                        ImGui::InputInt( "%", &sizeByPrc );
                    }
                    ImGui::Checkbox( "By absolute size: ", &byAbs );
                    if ( byAbs ) {
                        byPrc = false;
                        ImGui::Text( "    " );
                        ImGui::SameLine();
                        if ( ImGui::Checkbox( "Maintain aspect ratio",
                                              &aspRat ) ) {
                        }
                        ImGui::Separator();
                        ImGui::Text( "Pixel size" );
                        /* Setting width */
                        ImGui::Text( "Width: " );
                        ImGui::SameLine( 100 );
                        ImGui::InputInt( "wpixels", &width, 10 );
                        /* Setting height */
                        ImGui::Text( "Height: " );
                        ImGui::SameLine( 100 );
                        ImGui::InputInt( "hpixels", &height, 10 );
                    }
                    ImGui::Separator();
                    if ( ImGui::Button( "OK" ) ) { /* TextureCV func needed */
                        cv::Mat img_resized;
                        if ( byPrc ) { /* By percentage */
                            cv::resize( images.src, img_resized, cv::Size(),
                                        sizeByPrc / 100., sizeByPrc / 100. );
                        }
                        if ( byAbs ) { /* By absolute size */
                            cv::resize( images.src, img_resized,
                                        cv::Size( width, height ) );
                        }
                        // outCVTex.SetWidth(img_resized.cols);
                        // outCVTex.SetHeight(img_resized.rows);
                        outCVTex.UpdateTex( img_resized );
                        resizeMenu_IsShown = false;
                    }
                    ImGui::SameLine();
                    if ( ImGui::Button( "Cancel" ) ) {
                        resizeMenu_IsShown = false;
                    }
                    ImGui::End();
                }
            }
            /* Threshold edit menu */
            if ( thrMenu_IsShown ) {
                if ( !ImGui::Begin( "Threshold", &thrMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    ImGui::Checkbox( "Threshold", &thr_IsOn );
                    if ( thr_IsOn ) {
                        ImGui::SliderInt( "Threshold min value", &thrMinVal, 0,
                                          255 );
                        ImGui::NextColumn();
                        ImGui::SliderInt( "Threshold max value", &thrMaxVal, 0,
                                          255 );
                        ImGui::NextColumn();

                        outCVTex.Threshold( &thrMinVal, &thrMaxVal,
                                            cv::THRESH_BINARY );
                    }
                    ImGui::End();
                }
            }
            /* Brightness/Contrast */
            if ( bcMenu_IsShown ) {
                if ( !ImGui::Begin( "Brightness/Contrast", &bcMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    ImGui::SliderFloat( "Brightness", &brVl, -255, 255 );
                    ImGui::SliderFloat( "Contrast", &cntVl, 0.001, 5 );
                    outCVTex.ContrastBrightness( &cntVl, &brVl );
                    if ( ImGui::Button( "Reset" ) ) {
                        brVl = 0;
                        cntVl = 1;
                    }
                    ImGui::End();
                }
            }
            /*  Gaussian blur menu is shown/off
                (open with Filter->Gaussian Blur) */
            if ( gbMenu_IsShown ) {
                if ( !ImGui::Begin( "Gaussian Blur", &gbMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    ImGui::Checkbox( "Gaussian blur", &gb_IsOn );
                    if ( gb_IsOn ) {
                        ImGui::SliderInt( "Gaussian blur", &gbVal, 3, 55 );
                        outCVTex.GaussianBlur( &gbVal );
                    }
                    ImGui::End();
                }
            }
            /*  Median blur menu */
            if ( mbMenu_IsShown ) {
                if ( !ImGui::Begin( "Gaussian Blur", &mbMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    ImGui::Checkbox( "Median blur", &mdb_IsOn );
                    if ( mdb_IsOn ) {
                        ImGui::SliderInt( "Median blur", &mdbVal, 3, 55 );
                        outCVTex.MedianBlur( &mdbVal );
                    }
                    ImGui::End();
                }
            }
            /* Canny Edge Detection menu */
            if ( cedMenu_IsShown ) {
                if ( !ImGui::Begin( "Canny ED", &cedMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    ImGui::Columns( 2 );
                    ImGui::SliderInt( "Canny Min", &cannyMinVal, 0, 255 );
                    ImGui::NextColumn();
                    ImGui::SliderInt( "Canny Max", &cannyMaxVal, 0, 255 );
                    ImGui::Columns( 1 );
                    if ( ImGui::Button( "Apply" ) ) {
                        cv::Mat resized_down;
                        double s = .6;
                        cv::resize( images.src, resized_down, cv::Size(), s, s,
                                    cv::INTER_LINEAR );
                        /* min & max vals should be controlled parameters */
                        Filtering::CannyEdgeDet( resized_down, cannyMinVal,
                                                 cannyMaxVal );
                    }
                    ImGui::End();
                }
            }
            /* Custom Conv Kernel menu NOT IMPLEMENTED YET*/
            if ( ckMenu_IsShown ) {
                if ( !ImGui::Begin( "Apply Custom Convoluton Kernel",
                                    &ckMenu_IsShown ) ) {
                    ImGui::End();
                } else {
                    if ( ImGui::BeginCombo( "Kernel", current_kernel ) ) {
                        for ( int n = 0; n < IM_ARRAYSIZE( kernels ); n++ ) {
                            bool is_selected = ( current_kernel == kernels[n] );
                            if ( ImGui::Selectable( kernels[n],
                                                    is_selected ) ) {
                                current_kernel = kernels[n];
                            }
                            if ( is_selected ) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::Text( "Convoluton kernel matrix" );
                    ImGui::InputFloat3( "row1", ck.Row1, ".2f" );
                    ImGui::InputFloat3( "row2", ck.Row2, ".2f" );
                    ImGui::InputFloat3( "row3", ck.Row3, ".2f" );
                    if ( ImGui::Button( "Apply" ) ) {
                        cv::Mat result;
                        /* Custom kernel settings */
                        cv::Mat kernel = ( cv::Mat_<double>( 3, 3 ) <<  //
                                               ck.Row1[0],              //
                                           ck.Row1[1],                  //
                                           ck.Row1[2],                  //
                                           ck.Row2[0],                  //
                                           ck.Row2[1],                  //
                                           ck.Row2[2],                  //
                                           ck.Row3[0],                  //
                                           ck.Row3[1],                  //
                                           ck.Row3[2] );
                        /* Apply this one instantly */
                        cv::filter2D( images.src, result, -1, kernel,
                                      cv::Point( -1, -1 ), 4 );
                        cv::imshow( "Result", result );
                    }
                    ImGui::End();
                }
            }
            /* HSV menu is shown/off (open with Color->HSV) */
            if ( hsvMenu_IsShown ) {
                ImGui::Begin( "HSV editable", &hsvMenu_IsShown );
                /* This &colors[0] gives RED channel instead of proper H */

                cv::Mat hsvim;
                cv::cvtColor( images.src, hsvim, cv::COLOR_BGR2HSV );
                cv::Mat hsvim_edit = hsvim.clone();

                /* HSV channels splitted out to process each one */
                std::vector<cv::Mat> hsv_chans( 3 );
                std::vector<cv::Mat> hsv_chans_edit( 3 );
                /* slpit */
                cv::split( hsvim, hsv_chans );
                /* editing HSV channels */
                cv::split( hsvim_edit, hsv_chans_edit );

                ImGui::SliderInt( "H", &hsv_values.h, 0, 180 );
                cv::add( hsv_chans[0], hsv_values.h, hsv_chans_edit[0] );
                ImGui::SliderInt( "S", &hsv_values.s, -255, 255 );
                cv::add( hsv_chans[1], hsv_values.s, hsv_chans_edit[1] );
                ImGui::SliderInt( "V", &hsv_values.v, -255, 255 );
                cv::add( hsv_chans[2], hsv_values.v, hsv_chans_edit[2] );

                cv::merge( hsv_chans_edit, hsvim_edit );

                cv::Mat out;
                cv::cvtColor( hsvim_edit, out, cv::COLOR_HSV2BGR );

                hsvCVTex.UpdateTex( out );
                hsvCVTex.OnImGuiRender();
                ImGui::End();
                GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
            }

            ImGui::End();

            /* Rendering */
            ImGui::Render();
            // int display_w, display_h;
            // glfwGetFramebufferSize( window, &display_w, &display_h );
            // glViewport( 0, 0, display_w, display_h );
            // glClearColor( clear_color.x * clear_color.w,
            //               clear_color.y * clear_color.w,
            //               clear_color.z * clear_color.w, clear_color.w );
            // glClear( GL_COLOR_BUFFER_BIT );
            ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
            /* Swap front and back buffers */

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we
            // save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call
            //  glfwMakeContextCurrent(window) directly)
            if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent( backup_current_context );
            }

            glfwSwapBuffers( window );

            /* Poll for and process events */
            glfwPollEvents();
        }
        delete currentTest;
        if ( currentTest != testMenu ) {
            delete testMenu;
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}