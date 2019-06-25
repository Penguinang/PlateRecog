#ifndef DEBUG_H
#define DEBUG_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using cv::Mat;
using cv::Size;

#include <vector>
using std::vector;

extern Size gridSize;
extern vector<Mat> outImages;
inline void addtoOutput(Mat image){
    Size size = image.size();
    if(size.width > gridSize.width){
        cv::resize(image, image, {gridSize.width, size.height});
    }
    size = image.size();
    if(size.height > gridSize.height){
        cv::resize(image, image, {size.width, gridSize.height});
    }
    size = image.size();

    cv::copyMakeBorder(image, image, 0, gridSize.height - size.height, 0, gridSize.width - size.width, CV_HAL_BORDER_CONSTANT);
    outImages.push_back(image);
}

inline void DebugVisualize(const char *WindowName, const Mat &mat) {
#ifdef VISUALIZE_DEBUG
    cv::imshow(WindowName, mat);
    cv::waitKey(0);
#endif // VISUALIZE_DEBUG
#if 1
    addtoOutput(mat.clone());
#endif
}

inline void DebugVisualizeNotWait(const char *WindowName, const Mat &mat) {
#ifdef VISUALIZE_DEBUG
    cv::imshow(WindowName, mat);
#endif // VISUALIZE_DEBUG
#if 1
    // addtoOutput(mat);
#endif
}

#endif // !DEBUG_H
