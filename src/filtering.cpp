#include "filtering.h"

cv::Mat Filtering::Brightness( cv::Mat prcingImage, float *ctrlVal ) {
    cv::Mat prcedImage;

    /* Identity kernel */
    cv::Mat idKernel =
        ( cv::Mat_<double>( 3, 3 ) << 0, 0, 0, 0, *ctrlVal, 0, 0, 0, 0 );
    /* Apply this one instatnly */
    cv::filter2D( prcingImage, prcedImage, -1, idKernel, cv::Point( -1, -1 ),
                  4 );
    return prcedImage;
}

cv::Mat Filtering::GaussianBlur( cv::Mat prcingImage, int *ctrlVal ) {
    cv::Mat prcedImage;

    /* Built-in Gaussian Blur */
    if ( *ctrlVal % 2 == 0 ) *ctrlVal += 1;
    /* Apply this one instatnly */
    cv::GaussianBlur( prcingImage, prcedImage, cv::Size( *ctrlVal, *ctrlVal ),
                      0., 0. );
    return prcedImage;
}

cv::Mat Filtering::Thresholding(
    cv::Mat prcingImage, int *minVal, int *maxVal,
    cv::ThresholdTypes thrType = cv::THRESH_BINARY ) {
    cv::Mat prcedImage;
    cv::threshold( prcingImage, prcedImage, *minVal, *maxVal, thrType );
    return prcedImage;
}

cv::Mat Filtering::EdgeDetPrep( cv::Mat prcingImg ) {
    cv::Mat img_gray;
    cv::cvtColor( prcingImg, img_gray, cv::COLOR_BGR2GRAY );
    cv::Mat img_blur;
    cv::GaussianBlur( img_gray, img_blur, cv::Size( 3, 3 ), 0.);
    return img_blur;
}

cv::Mat Filtering::SobelEdgeDet( cv::Mat prcingImg, int x, int y ) {
    prcingImg = EdgeDetPrep( prcingImg );
    cv::Mat prcedImg;
    cv::Sobel( prcingImg, prcedImg, CV_64F, x, y, 5 );
    cv::imshow("Sobel edge detection", prcedImg);
    return prcedImg;
}

cv::Mat Filtering::CannyEdgeDet( cv::Mat prcingImg, int thrMin, int thrMax ) {
    cv::Mat img_blur = EdgeDetPrep( prcingImg );
    cv::Mat prcedImg;
    cv::Canny( img_blur, prcedImg, thrMin, thrMax );
    cv::imshow("Canny edge detection", prcedImg);
    return prcedImg;
}