#include <opencv2/core.hpp>
using cv::Rect;

#include "debug.h"


Size gridSize = {200, 100};
vector<Mat> outImages;
#include "debug.h"


void addtoOutput(Mat image){
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

Mat concatenteImags(vector<Mat> &images, int colCount){
    int count = images.size();
    int rows = count / colCount + (count % colCount != 0 ? 1 : 0);
    Mat out(vector<int>{gridSize.height * rows, gridSize.width * colCount}, CV_8UC3);
    out = 0;
    auto it = images.begin();
    for(int r = 0; r < rows; ++ r){
        for(int c = 0; c < colCount; ++ c){
            if(it == images.end())
                break;
            out(Rect({gridSize.width*c, gridSize.height*r}, gridSize)) = *it + 0;
            ++ it;
        }
    }
    return out;
}
