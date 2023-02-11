#include "custom_filtering.h"

cv::Mat CustomFiltering::Sepia( cv::Mat *prcingImg ) {
    cv::Mat prcedImg;
    /* Gray scale */
    // cv::Mat img_gray;
    // cv::cvtColor( *prcingImg, img_gray, cv::COLOR_BGR2GRAY );
    /* Normalize */
    // cv::Mat normalized;
    // cv::normalize( img_gray, normalized, 255, cv::NORM_MINMAX );
    /* Sepia color setting */
    cv::Mat kernel = ( cv::Mat_<double>( 3, 3 ) << 0.272, 0.534, 0.131,  //
                                                   0.349, 0.686, 0.168,  //
                                                   0.349, 0.769, 0.189 );
    cv::transform( *prcingImg, prcedImg, kernel );
    cv::imshow( "Sepia filter", prcedImg );

    return prcedImg;
}
