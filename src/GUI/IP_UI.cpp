#include "IP_UI.h"

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_impl_glfw.h"
#include "../../include/imgui/imgui_impl_opengl3.h"

#include "../filtering.h"
#include "../custom_filtering.h"

ImageProcessingUI::ImageProcessingUI()
    : m_InputTexture( "res/textures/tiger.jpg" ),
      m_OutputTexture( "res/textures/tiger.jpg" ),
      m_HSVTexture( "res/textures/tiger.jpg" ) {
    GLCall( glGenTextures( 1, &m_InImageID ) );
    GLCall( glGenTextures( 1, &m_OutImID ) );
    GLCall( glGenTextures( 1, &m_HSVImID ) );
    /*  */
    m_Images.src = cv::imread( "res/textures/tiger.jpg" );
    m_Images.processing = m_Images.src.clone();
    m_Images.out = m_Images.src.clone();
    m_Images.hsv = m_Images.src.clone();
    /* Source paths */
    static char frstImPath[255] = "res/textures/tiger.jpg";
    static char scndImPath[255] = "res/textures/wc.jpg";
    char buf[255]{};
    /* Preset Kernels */
    const char* kernels[] = { "blur",     "bottom sobel", "custom",
                              "emboss",   "identity",     "left sobel",
                              "outline",  "right sobel",  "sharpen",
                              "top sobel" };
    static const char* current_kernel = NULL;
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
}

ImageProcessingUI::~ImageProcessingUI() {}

void ImageProcessingUI::OnImGuiRender() {
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
                m_MenuFlags.resize_IsShown = !m_MenuFlags.resize_IsShown;
            }
            ImGui::EndMenu();
        }
        /* IMAGE ADJUSTMENTS */
        if ( ImGui::BeginMenu( "Adj" ) ) {
            if ( ImGui::MenuItem( "Brightness/Contrast" ) ) {
                m_MenuFlags.bc_IsShown = !m_MenuFlags.bc_IsShown;
            }
            ImGui::EndMenu();
        }
        /* FILTERS */
        if ( ImGui::BeginMenu( "Filter" ) ) {
            if ( ImGui::MenuItem( "B&W" ) ) {
                cv::Mat img_gray;
                cv::cvtColor( m_Images.src, img_gray, cv::COLOR_BGR2GRAY );
                cv::Mat resized_down;
                double s = .5;
                cv::resize( img_gray, resized_down, cv::Size(), s, s,
                            cv::INTER_LINEAR );
                cv::imshow( "B&W", resized_down );
            }
            if ( ImGui::BeginMenu( "Blur" ) ) {
                if ( ImGui::MenuItem( "Gaussian Blur" ) ) {
                    m_MenuFlags.gb_IsShown = !m_MenuFlags.gb_IsShown;
                }
                if ( ImGui::MenuItem( "Median Blur" ) ) {
                    m_MenuFlags.mb_IsShown = !m_MenuFlags.mb_IsShown;
                }
                ImGui::EndMenu();
            }
            if ( ImGui::BeginMenu( "Custom Filters" ) ) {
                if ( ImGui::MenuItem( "Sepia filter" ) ) {
                    cv::Mat resized_down;
                    double s = .5;
                    cv::resize( m_Images.src, resized_down, cv::Size(), s, s,
                                cv::INTER_LINEAR );
                    CustomFiltering::Sepia( &resized_down );
                }
                if ( ImGui::MenuItem( "Watercolor filter" ) ) {
                    cv::Mat resized_down;
                    double s = .5;
                    cv::resize( m_Images.src, resized_down, cv::Size(), s, s,
                                cv::INTER_LINEAR );
                    CustomFiltering::Watercolor( &resized_down );
                }
                if ( ImGui::MenuItem( "Cartoon filter" ) ) {
                    cv::Mat resized_down;
                    double s = .5;
                    cv::resize( m_Images.src, resized_down, cv::Size(), s, s,
                                cv::INTER_LINEAR );
                    CustomFiltering::Cartoon( &resized_down );
                }
                ImGui::EndMenu();
            }
            if ( ImGui::MenuItem( "Custom Kernel" ) ) {
                m_MenuFlags.ck_IsShown = !m_MenuFlags.ck_IsShown;
            }
            ImGui::EndMenu();
        }
        /* COLORS */
        if ( ImGui::BeginMenu( "Color" ) ) {
            if ( ImGui::BeginMenu( "Get color values" ) ) {
                if ( ImGui::MenuItem( "Get Red" ) ) {
                    std::vector<cv::Mat> chans( 3 );
                    cv::split( m_Images.src, chans );
                    cv::Mat resized_down;
                    cv::resize( chans[2], resized_down, cv::Size(), .5, .5,
                                cv::INTER_LINEAR );
                    cv::imshow( "Green ch only", resized_down );
                }
                if ( ImGui::MenuItem( "Get Green" ) ) {
                    std::vector<cv::Mat> chans( 3 );
                    cv::split( m_Images.src, chans );
                    cv::Mat resized_down;
                    cv::resize( chans[1], resized_down, cv::Size(), .5, .5,
                                cv::INTER_LINEAR );
                    cv::imshow( "Green ch only", resized_down );
                }
                if ( ImGui::MenuItem( "Get Blue" ) ) {
                    std::vector<cv::Mat> chans( 3 );
                    cv::split( m_Images.src, chans );
                    cv::Mat resized_down;
                    cv::resize( chans[0], resized_down, cv::Size(), .5, .5,
                                cv::INTER_LINEAR );
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
                    m_MenuFlags.ced_IsShown = !m_MenuFlags.ced_IsShown;
                }
                if ( ImGui::MenuItem( "Sobel" ) ) {
                    cv::Mat resized_down;
                    /* Should be controlled parameters */
                    double s = .6;
                    cv::resize( m_Images.src, resized_down, cv::Size(), s, s,
                                cv::INTER_LINEAR );
                    /* x & y should be controlled parameters */
                    int x = 1;
                    int y = 1;
                    Filtering::SobelEdgeDet( resized_down, x, y );
                }
                ImGui::EndMenu();
            }
            if ( ImGui::MenuItem( "Open HSV edit" ) ) {
                m_MenuFlags.hsv_IsShown = !m_MenuFlags.hsv_IsShown;
            }
            if ( ImGui::MenuItem( "Threshold" ) ) {
                m_MenuFlags.thr_IsShown = !m_MenuFlags.thr_IsShown;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Text( "Images to do with" );
    ImGui::InputText( "Image path", frstImPath, sizeof( buf ) - 1 );
    ImGui::InputText( "Path to second image", scndImPath, sizeof( buf ) - 1 );
    /* Doesn't work */
    if ( ImGui::Button( "Load images" ) ) {
        /* Load new image */
        cv::Mat srcImage = cv::imread( frstImPath );
        /* Update variables */
        m_Images.processing = srcImage.clone();
        m_Images.out = srcImage.clone();
        m_Images.hsv = srcImage.clone();

        if ( srcImage.empty() ) {
            printf( "Something wrong with your pics." );
        }
    }

    ImGui::Separator();
    /* ADDITIONAL MENUES */
    /* The window for the source image */
    if ( ImGui::Button( "Show Source Image" ) ) {
        m_MenuFlags.input_IsShown = !m_MenuFlags.input_IsShown;
    }
    if ( m_MenuFlags.input_IsShown ) {
        if ( !ImGui::Begin( "Source Image", &m_MenuFlags.input_IsShown ) ) {
            ImGui::End();
        } else {
            m_InputTexture.OnImGuiRender();
            ImGui::End();
            // GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
        }
    }
    ImGui::SameLine();
    /* The window for the processed image */
    if ( ImGui::Button( "Show Processed Image" ) ) {
        m_MenuFlags.cvTestWin_IsShown = !m_MenuFlags.cvTestWin_IsShown;
    }
    if ( m_MenuFlags.cvTestWin_IsShown ) {
        if ( !ImGui::Begin( "CV Texture test",
                            &m_MenuFlags.cvTestWin_IsShown ) ) {
            ImGui::End();
        } else {
            m_OutputTexture.OnImGuiRender();
            ImGui::End();
            // GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
        }
    }

    /* MainMenu -> Image -> */
    /* The Resize image menu */
    if ( m_MenuFlags.resize_IsShown ) {
        if ( !ImGui::Begin( "Resize", &m_MenuFlags.resize_IsShown ) ) {
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
                if ( ImGui::Checkbox( "Maintain aspect ratio", &aspRat ) ) {
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
                    cv::resize( m_Images.src, img_resized, cv::Size(),
                                sizeByPrc / 100., sizeByPrc / 100. );
                }
                if ( byAbs ) { /* By absolute size */
                    cv::resize( m_Images.src, img_resized,
                                cv::Size( width, height ) );
                }
                // outCVTex.SetWidth(img_resized.cols);
                // outCVTex.SetHeight(img_resized.rows);
                m_OutputTexture.UpdateTex( img_resized );
                m_MenuFlags.resize_IsShown = false;
            }
            ImGui::SameLine();
            if ( ImGui::Button( "Cancel" ) ) {
                m_MenuFlags.resize_IsShown = false;
            }
            ImGui::End();
        }
    }
    /* Threshold edit menu */
    if ( m_MenuFlags.thr_IsShown ) {
        if ( !ImGui::Begin( "Threshold", &m_MenuFlags.thr_IsShown ) ) {
            ImGui::End();
        } else {
            ImGui::Checkbox( "Threshold", &m_FilterFlags.thr_IsOn );
            if ( m_FilterFlags.thr_IsOn ) {
                ImGui::SliderInt( "Threshold min value",
                                  &m_FilterParameters.thrMinVal, 0, 255 );
                ImGui::NextColumn();
                ImGui::SliderInt( "Threshold max value",
                                  &m_FilterParameters.thrMaxVal, 0, 255 );
                ImGui::NextColumn();

                m_OutputTexture.Threshold( &m_FilterParameters.thrMinVal,
                                           &m_FilterParameters.thrMaxVal,
                                           cv::THRESH_BINARY );
            }
            ImGui::End();
        }
    }
    /* Brightness/Contrast */
    if ( m_MenuFlags.bc_IsShown ) {
        if ( !ImGui::Begin( "Brightness/Contrast", &m_MenuFlags.bc_IsShown ) ) {
            ImGui::End();
        } else {
            ImGui::SliderFloat( "Brightness", &m_FilterParameters.brVl, -255,
                                255 );
            ImGui::SliderFloat( "Contrast", &m_FilterParameters.cntVl, 0.001,
                                5 );
            m_OutputTexture.ContrastBrightness( &m_FilterParameters.cntVl,
                                                &m_FilterParameters.brVl );
            if ( ImGui::Button( "Reset" ) ) {
                m_FilterParameters.brVl = 0;
                m_FilterParameters.cntVl = 1;
            }
            ImGui::End();
        }
    }
    /*  Gaussian blur menu is shown/off
        (open with Filter->Gaussian Blur) */
    if ( m_MenuFlags.gb_IsShown ) {
        if ( !ImGui::Begin( "Gaussian Blur", &m_MenuFlags.gb_IsShown ) ) {
            ImGui::End();
        } else {
            ImGui::Checkbox( "Gaussian blur", &m_FilterFlags.gb_IsOn );
            if ( m_FilterFlags.gb_IsOn ) {
                ImGui::SliderInt( "Gaussian blur", &m_FilterParameters.gbVal, 3,
                                  55 );
                m_OutputTexture.GaussianBlur( &m_FilterParameters.gbVal );
            }
            ImGui::End();
        }
    }
    /*  Median blur menu */
    if ( m_MenuFlags.mb_IsShown ) {
        if ( !ImGui::Begin( "Gaussian Blur", &m_MenuFlags.mb_IsShown ) ) {
            ImGui::End();
        } else {
            ImGui::Checkbox( "Median blur", &m_FilterFlags.mdb_IsOn );
            if ( m_FilterFlags.mdb_IsOn ) {
                ImGui::SliderInt( "Median blur", &m_FilterParameters.mdbVal, 3,
                                  55 );
                m_OutputTexture.MedianBlur( &m_FilterParameters.mdbVal );
            }
            ImGui::End();
        }
    }
    /* Canny Edge Detection menu */
    if ( m_MenuFlags.ced_IsShown ) {
        if ( !ImGui::Begin( "Canny ED", &m_MenuFlags.ced_IsShown ) ) {
            ImGui::End();
        } else {
            ImGui::Columns( 2 );
            ImGui::SliderInt( "Canny Min", &m_FilterParameters.cannyMinVal, 0,
                              255 );
            ImGui::NextColumn();
            ImGui::SliderInt( "Canny Max", &m_FilterParameters.cannyMaxVal, 0,
                              255 );
            ImGui::Columns( 1 );
            if ( ImGui::Button( "Apply" ) ) {
                cv::Mat resized_down;
                double s = .6;
                cv::resize( m_Images.src, resized_down, cv::Size(), s, s,
                            cv::INTER_LINEAR );
                /* min & max vals should be controlled parameters */
                Filtering::CannyEdgeDet( resized_down,
                                         m_FilterParameters.cannyMinVal,
                                         m_FilterParameters.cannyMaxVal );
            }
            ImGui::End();
        }
    }
    /* Custom Conv Kernel menu NOT IMPLEMENTED YET*/
    if ( m_MenuFlags.ck_IsShown ) {
        if ( !ImGui::Begin( "Apply Custom Convoluton Kernel",
                            &m_MenuFlags.ck_IsShown ) ) {
            ImGui::End();
        } else {
            if ( ImGui::BeginCombo( "Kernel", current_kernel ) ) {
                for ( int n = 0; n < IM_ARRAYSIZE( kernels ); n++ ) {
                    bool is_selected = ( current_kernel == kernels[n] );
                    if ( ImGui::Selectable( kernels[n], is_selected ) ) {
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
                cv::filter2D( m_Images.src, result, -1, kernel,
                              cv::Point( -1, -1 ), 4 );
                cv::imshow( "Result", result );
            }
            ImGui::End();
        }
    }
    /* HSV menu is shown/off (open with Color->HSV) */
    if ( m_MenuFlags.hsv_IsShown ) {
        ImGui::Begin( "HSV editable", &m_MenuFlags.hsv_IsShown );
        /* This &colors[0] gives RED channel instead of proper H */

        cv::Mat hsvim;
        cv::cvtColor( m_Images.src, hsvim, cv::COLOR_BGR2HSV );
        cv::Mat hsvim_edit = hsvim.clone();

        /* HSV channels splitted out to process each one */
        std::vector<cv::Mat> hsv_chans( 3 );
        std::vector<cv::Mat> hsv_chans_edit( 3 );
        /* slpit */
        cv::split( hsvim, hsv_chans );
        /* editing HSV channels */
        cv::split( hsvim_edit, hsv_chans_edit );

        ImGui::SliderInt( "H", &m_FilterParameters.hsv_values.h, 0, 180 );
        cv::add( hsv_chans[0], m_FilterParameters.hsv_values.h,
                 hsv_chans_edit[0] );
        ImGui::SliderInt( "S", &m_FilterParameters.hsv_values.s, -255, 255 );
        cv::add( hsv_chans[1], m_FilterParameters.hsv_values.s,
                 hsv_chans_edit[1] );
        ImGui::SliderInt( "V", &m_FilterParameters.hsv_values.v, -255, 255 );
        cv::add( hsv_chans[2], m_FilterParameters.hsv_values.v,
                 hsv_chans_edit[2] );

        cv::merge( hsv_chans_edit, hsvim_edit );

        cv::Mat out;
        cv::cvtColor( hsvim_edit, out, cv::COLOR_HSV2BGR );

        m_HSVTexture.UpdateTex( out );
        m_HSVTexture.OnImGuiRender();
        ImGui::End();
        GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
    }

    ImGui::End();
}
