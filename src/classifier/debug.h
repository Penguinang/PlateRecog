#ifndef DEBUG_H
#define DEBUG_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using cv::Mat;
using cv::Size;

#include <vector>
using std::vector;


// 保存中间结果
extern Size gridSize;
extern vector<Mat> outImages;
void addtoOutput(Mat image);
Mat concatenteImags(vector<Mat> &images, int colCount = 5);


// 可视化中间矩阵
inline void DebugVisualize(const char *WindowName, const Mat &mat) {
#ifdef VISUALIZE_DEBUG
    cv::imshow(WindowName, mat);
    cv::waitKey(0);
#endif // VISUALIZE_DEBUG
#ifdef SAVE_INTERNAL_IMAGE
    addtoOutput(mat.clone());
#endif
}

inline void DebugVisualizeNotWait(const char *WindowName, const Mat &mat) {
#ifdef VISUALIZE_DEBUG
    cv::imshow(WindowName, mat);
#endif // VISUALIZE_DEBUG
#ifdef SAVE_INTERNAL_IMAGE
    // addtoOutput(mat);
#endif
}

#endif // !DEBUG_H
