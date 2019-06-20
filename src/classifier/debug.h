#ifndef DEBUG_H
#define DEBUG_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using cv::Mat;

inline void DebugVisualize(const char *WindowName, const Mat &mat){
#ifdef VISUALIZE_DEBUG
    cv::imshow(WindowName, mat);
    cv::waitKey(0);
#endif // VISUALIZE_DEBUG
}

#endif // !DEBUG_H
