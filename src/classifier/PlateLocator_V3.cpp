#include "CharInfo.h"
#include "PlateCategory_SVM.h"
#include "PlateLocator_V3.h"
#include "Utilities.h"

using cv::Point;
#include <utility>
using std::get;
using std::tuple;

using namespace Doit::CV::PlateRecogn;

bool PlateLocator_V3::VerifyPlateSize(const cv::Size &size, int minWidth,
                                      int maxWidth, int minHeight,
                                      int maxHeight, float minRatio,
                                      float maxRatio) {
    int width = size.width;
    int height = size.height;
    if (width == 0 || height == 0)
        return false;
    float ratio = (float)height / width;
    bool result = (width > minWidth && width < maxWidth) &&
                  (height > minHeight && height < maxHeight) &&
                  (ratio > minRatio && ratio < maxRatio);
    return result;
}

vector<PlateInfo> PlateLocator_V3::LocatePlatesForCameraAdjust(
    const Mat &matSource, Mat &matProcess, int blur_Size, int sobel_Scale,
    int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight,
    int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth,
    int minHeight, int maxHeight, float minRatio, float maxRatio) {
    auto ret = LocatePlatesForAutoSample(
        matSource, matProcess, blur_Size, sobel_Scale, sobel_Delta,
        sobel_X_Weight, sobel_Y_Weight, morph_Size_Width, morph_Size_Height,
        minWidth, maxWidth, minHeight, maxHeight, minRatio, maxRatio);
    auto plateInfos = get<0>(ret[0]);
    for (size_t index = plateInfos.size() - 1; index < plateInfos.size();
         index--) {
        if (plateInfos[index].PlateCategory == PlateCategory_t::NonPlate)
            plateInfos.erase(plateInfos.begin() + index);
    }
    return plateInfos;
}

// color:  PlateInfos, threshold, after erode and close, contours, rected
// sobel(optional): PlateInfos, threshold, after erode and close, contours, rected
vector<tuple<vector<PlateInfo>, Mat, Mat, vector<vector<Point>>, Mat>>
PlateLocator_V3::LocatePlatesForAutoSample(
    const Mat &matSource, Mat &matProcess, int blur_Size, int sobel_Scale,
    int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight,
    int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth,
    int minHeight, int maxHeight, float minRatio, float maxRatio) {
    vector<PlateInfo> plateInfosForColor = vector<PlateInfo>();
    //优先使⽤颜⾊法定位可能是⻋牌的区域，如果没有发现⻋牌，再使⽤Sobel法
    Mat gray;
    Mat blur;
    if (matSource.empty() || matSource.rows == 0 || matSource.cols == 0) {
        matProcess = Mat(0, 0, CV_8UC1);
        return {{}, {}, {}, {}, {}};
    }
    cvtColor(matSource, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(blur_Size, blur_Size), 0, 0,
                 cv::BorderTypes::BORDER_DEFAULT);
    Mat hsv;
    cv::cvtColor(matSource, hsv, cv::COLOR_BGR2HSV);
    vector<Mat> hsvSplits;
    split(hsv, hsvSplits);
    equalizeHist(hsvSplits[2], hsvSplits[2]);
    Mat hsvEqualizeHist;
    merge(hsvSplits, hsvEqualizeHist);
    Scalar blueStart = cv::Scalar(100, 70, 70);
    Scalar blueEnd = cv::Scalar(140, 255, 255);
    Mat blue;
    cv::inRange(hsvEqualizeHist, blueStart, blueEnd, blue);
    Scalar yellowStart = Scalar(15, 70, 70);
    Scalar yellowEnd = Scalar(40, 255, 255);
    Mat yellow;
    cv::inRange(hsvEqualizeHist, yellowStart, yellowEnd, yellow);
    Mat add = blue + yellow;
    Mat thresholdForColor;
    cv::threshold(add, thresholdForColor, 0, 255,
                  cv::ThresholdTypes::THRESH_OTSU |
                      cv::ThresholdTypes::THRESH_BINARY);
    Mat element = cv::getStructuringElement(
        cv::MorphShapes::MORPH_RECT,
        cv::Size(morph_Size_Width, morph_Size_Height));
    Mat threshold_Close;
    cv::morphologyEx(thresholdForColor, threshold_Close,
                     cv::MorphTypes::MORPH_CLOSE, element);
    Mat element_Erode =
        cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(3, 3));
    Mat threshold_ErodeForColor;
    cv::erode(threshold_Close, threshold_ErodeForColor, element_Erode);
    matProcess = threshold_ErodeForColor;
    vector<vector<cv::Point>> contoursForColor;
    vector<cv::Vec4i> hierarchys;
    cv::findContours(threshold_ErodeForColor, contoursForColor, hierarchys,
                     cv::RetrievalModes::RETR_EXTERNAL,
                     cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    int isPlateCount = 0;
    for (size_t index = 0; index < contoursForColor.size(); index++) {
        RotatedRect rotatedRect = cv::minAreaRect(contoursForColor[index]);
        Rect rectROI = cv::boundingRect(contoursForColor[index]);
        if (VerifyPlateSize(rectROI.size(), minWidth, maxWidth, minHeight,
                            maxHeight, minRatio, maxRatio)) {
            Mat matROI = matSource(rectROI);
            PlateCategory_t plateCategory = PlateCategory_SVM::Test(matROI);
            if (plateCategory != PlateCategory_t::NonPlate)
                isPlateCount++;
            PlateInfo plateInfo;
            plateInfo.RotatedRect = rotatedRect;
            plateInfo.OriginalRect = rectROI;
            plateInfo.OriginalMat = matROI;
            plateInfo.PlateCategory = plateCategory;
            plateInfo.PlateLocateMethod = PlateLocateMethod_t::Color;
            plateInfosForColor.push_back(plateInfo);
        }
    }
    if (isPlateCount > 0) {
        Mat rectImageForColor = matSource.clone();
        reserveBoundingRects(rectImageForColor, contoursForColor, -1, {0, 0, 255});
        return {{plateInfosForColor, thresholdForColor, threshold_ErodeForColor,
                 contoursForColor, rectImageForColor}};
    }

    vector<PlateInfo> plateInfosForSobel = vector<PlateInfo>();
    cv::GaussianBlur(matSource, blur, cv::Size(blur_Size, blur_Size), 0, 0,
                     cv::BorderTypes::BORDER_DEFAULT);
    cv::cvtColor(blur, gray, cv::COLOR_BGR2GRAY);
    // 对图像进⾏Sobel 运算，得到的是图像的⼀阶⽔平⽅向导数。
    auto ddepth = CV_16S;
    Mat grad_x;
    cv::Sobel(gray, grad_x, ddepth, 1, 0, 3, sobel_Scale, sobel_Delta,
              cv::BorderTypes::BORDER_DEFAULT);
    Mat abs_grad_x;
    cv::convertScaleAbs(grad_x, abs_grad_x);
    Mat grad_y;
    cv::Sobel(gray, grad_y, ddepth, 0, 1, 3, sobel_Scale, sobel_Delta,
              cv::BorderTypes::BORDER_DEFAULT);
    Mat abs_grad_y;
    cv::convertScaleAbs(grad_y, abs_grad_y);
    Mat grad = Mat();
    cv::addWeighted(abs_grad_x, sobel_X_Weight, abs_grad_y, sobel_Y_Weight, 0,
                    grad);
    // 对图像进⾏⼆值化。将灰度图像（每个像素点有256
    // 个取值可能）转化为⼆值图像（每个像素点仅有1 和0 两个取值可能）。
    Mat thresholdForSobel;
    cv::threshold(grad, thresholdForSobel, 0, 255,
                  cv::ThresholdTypes::THRESH_OTSU |
                      cv::ThresholdTypes::THRESH_BINARY);
    // 使⽤闭操作。对图像进⾏闭操作以后，可以看到⻋牌区域被连接成⼀个矩形装的区域。
    element = cv::getStructuringElement(
        cv::MorphShapes::MORPH_RECT,
        cv::Size(morph_Size_Width, morph_Size_Height));
    cv::morphologyEx(thresholdForSobel, threshold_Close,
                     cv::MorphTypes::MORPH_CLOSE, element);
    element_Erode =
        cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(5, 5));

    Mat threshold_CloseForSobel;
    cv::erode(threshold_Close, threshold_CloseForSobel, element_Erode);
    matProcess = threshold_CloseForSobel;

    // Find 轮廓 of possibles plates
    // 求轮廓。求出图中所有的轮廓。这个算法会把全图的轮廓都计算出来，
    // 因此要进⾏ 筛选。
    vector<vector<cv::Point>> contoursForClose = {};
    hierarchys = {};
    cv::findContours(threshold_CloseForSobel, contoursForClose, hierarchys,
                     cv::RetrievalModes::RETR_EXTERNAL,
                     cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    // 筛选。对轮廓求最⼩外接矩形，然后验证，不满⾜条件的淘汰。
    for (size_t index = 0; index < contoursForClose.size(); index++) {
        Rect rectROI = cv::boundingRect(contoursForClose[index]);
        if (VerifyPlateSize(rectROI.size(), minWidth, maxWidth, minHeight,
                            maxHeight, minRatio, maxRatio)) {
            RotatedRect rotatedRect = cv::minAreaRect(contoursForClose[index]);
            Mat matROI = matSource(rectROI);
            PlateCategory_t plateCategory = PlateCategory_SVM::Test(matROI);

            PlateInfo plateInfo;
            plateInfo.RotatedRect = rotatedRect;
            plateInfo.OriginalRect = rectROI;
            plateInfo.OriginalMat = matROI;
            plateInfo.PlateCategory = plateCategory;
            plateInfo.PlateLocateMethod = PlateLocateMethod_t::Sobel;
            plateInfosForSobel.push_back(plateInfo);
        }
    }

    Mat rectImageForColor = matSource.clone();
    reserveBoundingRects(rectImageForColor, contoursForColor, -1, {0, 0, 255});
    Mat rectImageForClose = matSource.clone();
    reserveBoundingRects(rectImageForClose, contoursForClose, -1, {0, 0, 255});
    return {{plateInfosForColor, thresholdForColor, threshold_ErodeForColor,
             contoursForColor, rectImageForColor},
            {plateInfosForSobel, thresholdForSobel, threshold_CloseForSobel,
             contoursForClose, rectImageForClose}};
}

vector<PlateInfo> PlateLocator_V3::LocatePlates(
    const Mat &matSource, int blur_Size, int sobel_Scale, int sobel_Delta,
    int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width,
    int morph_Size_Height, int minWidth, int maxWidth, int minHeight,
    int maxHeight, float minRatio, float maxRatio) {
    vector<PlateInfo> plateInfos = LocatePlatesByColor(
        matSource, blur_Size, morph_Size_Width, morph_Size_Height, minWidth,
        maxWidth, minHeight, maxHeight, minRatio, maxRatio);
    if (plateInfos.size() > 0)
        return plateInfos;
    plateInfos = LocatePlatesBySobel(
        matSource, blur_Size, sobel_Scale, sobel_Delta, sobel_X_Weight,
        sobel_Y_Weight, morph_Size_Width, morph_Size_Height, minWidth, maxWidth,
        minHeight, maxHeight, minRatio, maxRatio);
    return plateInfos;
}

vector<PlateInfo> PlateLocator_V3::LocatePlatesByColor(
    const Mat &matSource, int blur_Size, int morph_Size_Width,
    int morph_Size_Height, int minWidth, int maxWidth, int minHeight,
    int maxHeight, float minRatio, float maxRatio) {
    vector<PlateInfo> plateInfos = vector<PlateInfo>();
    if (matSource.empty())
        return plateInfos;
    // Mat hsv = matSource.CvtColor(ColorConversionCodes.BGR2HSV);
    Mat hsv;
    cv::cvtColor(matSource, hsv, cv::COLOR_BGR2HSV);

    // Mat[] hsvSplits = hsv.Split();
    vector<Mat> hsvSplits;
    cv::split(hsv, hsvSplits);
    // hsvSplits[2] = hsvSplits[2].EqualizeHist();
    cv::equalizeHist(hsvSplits[2], hsvSplits[2]);
    Mat hsvEqualizeHist;
    cv::merge(hsvSplits, hsvEqualizeHist);

    Scalar blueStart = Scalar(100, 70, 70);
    Scalar blueEnd = Scalar(140, 255, 255);
    // Mat blue = hsvEqualizeHist.InRange(blueStart, blueEnd);
    Mat blue;
    cv::inRange(hsvEqualizeHist, blueStart, blueEnd, blue);

    Scalar yellowStart = Scalar(15, 70, 70);
    Scalar yellowEnd = Scalar(40, 255, 255);
    // Mat yellow = hsvEqualizeHist.InRange(yellowStart, yellowEnd);
    Mat yellow;
    cv::inRange(hsvEqualizeHist, yellowStart, yellowEnd, yellow);
    Mat add = blue + yellow;
    // Mat threshold =
    //     add.Threshold(0, 255, ThresholdTypes.Otsu |
    //     ThresholdTypes.Binary);
    Mat threshold;
    cv::threshold(add, threshold, 0, 255,
                  cv::ThresholdTypes::THRESH_OTSU |
                      cv::ThresholdTypes::THRESH_BINARY);
    Mat element = cv::getStructuringElement(
        cv::MorphShapes::MORPH_RECT,
        cv::Size(morph_Size_Width, morph_Size_Height));
    // Mat threshold_Close = threshold.MorphologyEx(MorphTypes.Close,
    // element);
    Mat threshold_Close;
    cv::morphologyEx(threshold, threshold_Close, cv::MorphTypes::MORPH_CLOSE,
                     element);

    // TODO 腐蚀核大小
    Mat element_Erode =
        cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(3, 3));
    // Mat element_Erode = cv::getStructuringElement(
    //     cv::MorphShapes::MORPH_RECT, cv::Size(10, 10));
    // Mat threshold_Erode = threshold_Close.Erode(element_Erode);
    Mat threshold_Erode;
    cv::erode(threshold_Close, threshold_Erode, element_Erode);

    vector<vector<cv::Point>> contours = {};
    vector<cv::Vec4i> hierarchys = {};
    cv::findContours(threshold_Erode, contours, hierarchys,
                     cv::RetrievalModes::RETR_EXTERNAL,
                     cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    for (size_t index = 0; index < contours.size(); index++) {
        Rect rectROI = cv::boundingRect(contours[index]);
        if (VerifyPlateSize(rectROI.size(), minWidth, maxWidth, minHeight,
                            maxHeight, minRatio, maxRatio)) {
            Mat matROI = matSource(rectROI);
            PlateCategory_t plateCategory = PlateCategory_SVM::Test(matROI);
            if (plateCategory == PlateCategory_t::NonPlate)
                continue;
            PlateInfo plateInfo = PlateInfo();
            plateInfo.OriginalRect = rectROI;
            plateInfo.OriginalMat = matROI;
            plateInfo.PlateCategory = plateCategory;
            plateInfo.PlateLocateMethod = PlateLocateMethod_t::Sobel;
            plateInfos.push_back(plateInfo);
        }
    }
    return plateInfos;
}

vector<PlateInfo> PlateLocator_V3::LocatePlatesBySobel(
    Mat matSource, int blur_Size, int sobel_Scale, int sobel_Delta,
    int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width,
    int morph_Size_Height, int minWidth, int maxWidth, int minHeight,
    int maxHeight, float minRatio, float maxRatio) {
    vector<PlateInfo> plateInfos = vector<PlateInfo>();
    if (matSource.empty())
        return plateInfos;
    Mat blur;
    Mat gray;
    // blur =
    //     matSource.GaussianBlur(cv::Size(blur_Size, blur_Size),
    //                            0, 0, BorderTypes.Default);
    cv::GaussianBlur(matSource, blur, cv::Size(blur_Size, blur_Size), 0, 0,
                     cv::BorderTypes::BORDER_DEFAULT);
    // gray = blur.CvtColor(ColorConversionCodes.BGR2GRAY);
    cv::cvtColor(blur, gray, cv::COLOR_BGR2GRAY);
    // 对图像进⾏Sobel 运算，得到的是图像的⼀阶⽔平⽅向导数。
    // Generate grad_x and grad_y
    auto ddepth = CV_16S;
    // Mat grad_x = gray.Sobel(ddepth, 1, 0, 3, sobel_Scale, sobel_Delta,
    //                         BorderTypes.Default);
    Mat grad_x;
    cv::Sobel(gray, grad_x, ddepth, 1, 0, 3, sobel_Scale, sobel_Delta,
              cv::BorderTypes::BORDER_DEFAULT);
    // Mat abs_grad_x = grad_x.ConvertScaleAbs();
    Mat abs_grad_x;
    cv::convertScaleAbs(grad_x, abs_grad_x);
    // Mat grad_y = gray.Sobel(ddepth, 0, 1, 3, sobel_Scale, sobel_Delta,
    //                         BorderTypes.Default);
    Mat grad_y;
    cv::Sobel(gray, grad_y, ddepth, 0, 1, 3, sobel_Scale, sobel_Delta,
              cv::BorderTypes::BORDER_DEFAULT);
    // Mat abs_grad_y = grad_y.ConvertScaleAbs();
    Mat abs_grad_y;
    cv::convertScaleAbs(grad_y, abs_grad_y);
    Mat grad;
    cv::addWeighted(abs_grad_x, sobel_X_Weight, abs_grad_y, sobel_Y_Weight, 0,
                    grad);

    // 对图像进⾏⼆值化。将灰度图像（每个像素点有256
    // 个取值可能）转化为⼆值图像（每个像素点仅有1 和0 两个取值可能）。
    // Mat threshold =
    //     grad.Threshold(0, 255, ThresholdTypes.Otsu |
    //     ThresholdTypes.Binary);
    Mat threshold;
    cv::threshold(grad, threshold, 0, 255,
                  cv::ThresholdTypes::THRESH_OTSU |
                      cv::ThresholdTypes::THRESH_BINARY);

    // 使⽤闭操作。对图像进⾏闭操作以后，可以看到⻋牌区域被连接成⼀个矩形装的区域。
    Mat element = cv::getStructuringElement(
        cv::MorphShapes::MORPH_RECT,
        cv::Size(morph_Size_Width, morph_Size_Height));
    // Mat threshold_Close = threshold.MorphologyEx(MorphTypes.Close,
    // element);
    Mat threshold_Close;
    cv::morphologyEx(threshold, threshold_Close, cv::MorphTypes::MORPH_CLOSE,
                     element);

    Mat element_Erode =
        cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(5, 5));
    // Mat threshold_Erode = threshold_Close.Erode(element_Erode);
    Mat threshold_Erode;
    cv::erode(threshold_Close, threshold_Erode, element_Erode);
    // Find 轮廓 of possibles plates
    // 求轮廓。求出图中所有的轮廓。这个算法会把全图的轮廓都计算出来，
    // 因此要进⾏ 筛选。
    vector<vector<cv::Point>> contours = {};
    vector<cv::Vec4i> hierarchys = {};
    cv::findContours(threshold_Erode, contours, hierarchys,
                     cv::RetrievalModes::RETR_EXTERNAL,
                     cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    // 筛选。对轮廓求最⼩外接矩形，然后验证，不满⾜条件的淘汰。
    for (size_t index = 0; index < contours.size(); index++) {
        Rect rectROI = cv::boundingRect(contours[index]);
        if (VerifyPlateSize(rectROI.size(), minWidth, maxWidth, minHeight,
                            maxHeight, minRatio, maxRatio)) {
            Mat matROI = matSource(rectROI);
            PlateCategory_t plateCategory = PlateCategory_SVM::Test(matROI);
            if (plateCategory == PlateCategory_t::NonPlate)
                continue;
            PlateInfo plateInfo = PlateInfo();
            plateInfo.OriginalRect = rectROI;
            plateInfo.OriginalMat = matROI;
            plateInfo.PlateCategory = plateCategory;
            plateInfo.PlateLocateMethod = PlateLocateMethod_t::Sobel;
            plateInfos.push_back(plateInfo);
        }
    }
    return plateInfos;
}