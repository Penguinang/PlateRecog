#include <string>
using std::string;
#include "CharInfo.h"
#include "CharSegment_V3.h"
#include "PlateChar_SVM.h"
#include "Utilities.h"
#include "debug.h"

using namespace Doit::CV::PlateRecogn;

cv::Mat CharSegment_V3::ClearMaoding(cv::Mat &threshold) {
    vector<float> jumps;
    cv::Mat jump = cv::Mat(threshold.rows, 1, CV_32F).clone();
    for (int rowIndex = 0; rowIndex < threshold.rows; rowIndex++) {
        int jumpCount = 0;
        for (int colIndex = 0; colIndex < threshold.cols - 1; colIndex++) {
            if (threshold.at<uchar>(rowIndex, colIndex) !=
                threshold.at<uchar>(rowIndex, colIndex + 1))
                jumpCount++;
        }
        jump.at<float>(rowIndex, 0) = (float)jumpCount;
    }

    int x = 7;
    cv::Mat result = threshold.clone();
    for (int rowIndex = 0; rowIndex < threshold.rows; rowIndex++) {
        if (jump.at<float>(rowIndex) <= x) {
            for (int colIndex = 0; colIndex < threshold.cols; colIndex++) {
                result.at<uchar>(rowIndex, colIndex) = 0;
            }
        }
    }
    return result;
}

cv::Mat CharSegment_V3::ClearBorder(cv::Mat &threshold) {
    int rows = threshold.rows;
    int cols = threshold.cols;

    int noJumpCountThresh = (int)(0.15f * cols);

    cv::Mat border = cv::Mat(rows, 1, CV_8UC1).clone();

    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
        int noJumpCount = 0;
        uchar isBorder = 0;

        for (int colIndex = 0; colIndex < cols - 1; colIndex++) {
            if (threshold.at<uchar>(rowIndex, colIndex) ==
                threshold.at<uchar>(rowIndex, colIndex + 1)) {
                noJumpCount++;
            }
            if (noJumpCount > noJumpCountThresh) {
                noJumpCount = 0;
                isBorder = 1;
                break;
            }
        }
        border.at<uchar>(rowIndex, 0) = isBorder;
    }

    int minTop = (int)(0.1f * rows);
    int maxTop = (int)(0.9f * rows);

    cv::Mat result = threshold.clone();

    for (int rowIndex = 0; rowIndex < minTop; rowIndex++) {
        if (border.at<uchar>(rowIndex, 0) == 1) {
            for (int colIndex = 0; colIndex < cols; colIndex++) {
                result.at<uchar>(rowIndex, colIndex) = 0;
            }
        }
    }

    for (int rowIndex = rows - 1; rowIndex > maxTop; rowIndex--) {
        if (border.at<uchar>(rowIndex, 0) == 1) {
            for (int colIndex = 0; colIndex < cols; colIndex++) {
                result.at<uchar>(rowIndex, colIndex) = 0;
            }
        }
    }
    return result;
}

cv::Mat CharSegment_V3::ClearMaodingAndBorder(cv::Mat &gray,
                                              PlateColor_t &plateColor) {
    cv::Mat threshold;

    switch (plateColor) {
    case PlateColor_t::BluePlate:
    case PlateColor_t::BlackPlate:
        cv::threshold(gray, threshold, 1, 255,
                      cv::THRESH_OTSU | cv::THRESH_BINARY);
        break;
    case PlateColor_t::YellowPlate:
    case PlateColor_t::WhitePlate:
    case PlateColor_t::GreenPlate:
        cv::threshold(gray, threshold, 1, 255,
                      cv::THRESH_OTSU | cv::THRESH_BINARY);
        break;
    case PlateColor_t::UnknownPlate:
        cv::threshold(gray, threshold, 1, 255,
                      cv::THRESH_OTSU | cv::THRESH_BINARY);
        break;
    }

    cv::Mat matOfClearBorder = ClearBorder(threshold);
    cv::Mat matOfClearMaodingAndBorder = ClearMaoding(matOfClearBorder);

    return matOfClearMaodingAndBorder;
}

vector<CharInfo> CharSegment_V3::SpliteCharsInPlateMat(cv::Mat &plateMat,
                                                       vector<Rect> &rects) {
    if (PlateChar_SVM::IsReady == false) {
        throw logic_error("The character recognition library is not ready");
    }

    vector<CharInfo> result;
    for (size_t index = 0; index < rects.size(); index++) {
        Rect rect = rects[index];
        rect = Utilities::GetSafeRect(rect, plateMat);

        CharInfo charInfo;
        cv::Mat originalMat = plateMat(rect);
        charInfo.OriginalMat = originalMat;
        charInfo.OriginalRect = rect;
        charInfo.PlateChar = PlateChar_SVM::Test(originalMat);

        result.push_back(charInfo);
    }

    std::sort(result.begin(), result.end(), CharInfoLeftComparer);
    return result;
}

vector<CharInfo> CharSegment_V3::SplitePlateForAutoSample(cv::Mat &plateMat) {
    vector<CharInfo> result;
    std::vector<std::vector<cv::Point>> contours_Original_Blue;
    std::vector<std::vector<cv::Point>> contours_IndexTransform_Blue;
    std::vector<std::vector<cv::Point>> contours_GammaTransform_Blue;
    std::vector<std::vector<cv::Point>> contours_LogTransform_Blue;
    vector<CharInfo> charInfos_Original_Blue = SplitePlateByOriginal(
        contours_Original_Blue, plateMat, plateMat, PlateColor_t::BluePlate);
    vector<CharInfo> charInfos_IndexTransform_Blue =
        SplitePlateByIndexTransform(contours_IndexTransform_Blue, plateMat,
                                    PlateColor_t::BluePlate);
    vector<CharInfo> charInfos_GammaTransform_Blue =
        SplitePlateByGammaTransform(contours_GammaTransform_Blue, plateMat,
                                    PlateColor_t::BluePlate);
    vector<CharInfo> charInfos_LogTransform_Blue = SplitePlateByLogTransform(
        contours_LogTransform_Blue, plateMat, PlateColor_t::BluePlate);

    vector<CharInfo> charInfos_Blue;
    charInfos_Blue.insert(charInfos_Blue.end(), charInfos_Original_Blue.begin(),
                          charInfos_Original_Blue.end());
    charInfos_Blue.insert(charInfos_Blue.end(),
                          charInfos_IndexTransform_Blue.begin(),
                          charInfos_IndexTransform_Blue.end());
    charInfos_Blue.insert(charInfos_Blue.end(),
                          charInfos_GammaTransform_Blue.begin(),
                          charInfos_GammaTransform_Blue.end());
    charInfos_Blue.insert(charInfos_Blue.end(),
                          charInfos_LogTransform_Blue.begin(),
                          charInfos_LogTransform_Blue.end());

    int isCharCount_Blue = 0;
    for (size_t index = 0; index < charInfos_Blue.size(); index++) {
        CharInfo &charInfo = charInfos_Blue[index];
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);

        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Blue++;
        }
    }

    // if (isCharCount_Blue >= 15) return charInfos_Blue;

    std::vector<std::vector<cv::Point>> contours_Original_Yellow;
    std::vector<std::vector<cv::Point>> contours_IndexTransform_Yellow;
    std::vector<std::vector<cv::Point>> contours_GammaTransform_Yellow;
    std::vector<std::vector<cv::Point>> contours_LogTransform_Yellow;
    vector<CharInfo> charInfos_Original_Yellow =
        SplitePlateByOriginal(contours_Original_Yellow, plateMat, plateMat,
                              PlateColor_t::YellowPlate);
    vector<CharInfo> charInfos_IndexTransform_Yellow =
        SplitePlateByIndexTransform(contours_IndexTransform_Yellow, plateMat,
                                    PlateColor_t::YellowPlate);
    vector<CharInfo> charInfos_GammaTransform_Yellow =
        SplitePlateByGammaTransform(contours_GammaTransform_Yellow, plateMat,
                                    PlateColor_t::YellowPlate);
    vector<CharInfo> charInfos_LogTransform_Yellow = SplitePlateByLogTransform(
        contours_LogTransform_Yellow, plateMat, PlateColor_t::YellowPlate);

    vector<CharInfo> charInfos_Yellow;
    charInfos_Yellow.insert(charInfos_Yellow.end(),
                            charInfos_Original_Yellow.begin(),
                            charInfos_Original_Yellow.end());
    charInfos_Yellow.insert(charInfos_Yellow.end(),
                            charInfos_IndexTransform_Yellow.begin(),
                            charInfos_IndexTransform_Yellow.end());
    charInfos_Yellow.insert(charInfos_Yellow.end(),
                            charInfos_GammaTransform_Yellow.begin(),
                            charInfos_GammaTransform_Yellow.end());
    charInfos_Yellow.insert(charInfos_Yellow.end(),
                            charInfos_LogTransform_Yellow.begin(),
                            charInfos_LogTransform_Yellow.end());

    int isCharCount_Yellow = 0;
    for (size_t index = 0; index < charInfos_Yellow.size(); index++) {
        CharInfo &charInfo = charInfos_Yellow[index];
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar)
            isCharCount_Yellow++;
    }
    // if (isCharCount_Yellow >= 15) return charInfos_Yellow;
    if (isCharCount_Blue > isCharCount_Yellow) {
        return charInfos_Blue;
    } else {
        return charInfos_Yellow;
    }

    // charInfos_Blue.clear();
    // return charInfos_Blue;
}

vector<CharInfo> CharSegment_V3::SplitePlateByIndexTransform(
    std::vector<std::vector<cv::Point>> &contours, cv::Mat &originalMat,
    PlateColor_t plateColor, int leftLimit, int rightLimit, int topLimit,
    int bottomLimit, int minWidth, int maxWidth, int minHeight, int maxHeight,
    float minRatio, float maxRatio) {

    cv::Mat plateMat = Utilities::IndexTransform(originalMat);
    return SplitePlateByOriginal(contours, originalMat, plateMat, plateColor,
                                 CharSplitMethod_t::Exponential, leftLimit,
                                 rightLimit, topLimit, bottomLimit, minWidth,
                                 maxWidth, minHeight, maxHeight, minRatio,
                                 maxRatio);
}

vector<CharInfo> CharSegment_V3::SplitePlateByLogTransform(
    std::vector<std::vector<cv::Point>> &contours, cv::Mat &originalMat,
    PlateColor_t plateColor, int leftLimit, int rightLimit, int topLimit,
    int bottomLimit, int minWidth, int maxWidth, int minHeight, int maxHeight,
    float minRatio, float maxRatio) {
    cv::Mat plateMat = Utilities::LogTransform(originalMat);
    return SplitePlateByOriginal(contours, originalMat, plateMat, plateColor,
                                 CharSplitMethod_t::Log, leftLimit, rightLimit,
                                 topLimit, bottomLimit, minWidth, maxWidth,
                                 minHeight, maxHeight, minRatio, maxRatio);
}

vector<CharInfo> CharSegment_V3::SplitePlateByGammaTransform(
    std::vector<std::vector<cv::Point>> &contours, Mat &originalMat,
    PlateColor_t plateColor, float gammaFactor, int leftLimit, int rightLimit,
    int topLimit, int bottomLimit, int minWidth, int maxWidth, int minHeight,
    int maxHeight, float minRatio, float maxRatio) {
    cv::Mat plateMat = Utilities::GammaTransform(originalMat, gammaFactor);
    return SplitePlateByOriginal(
        contours, originalMat, plateMat, plateColor, CharSplitMethod_t::Gamma,
        leftLimit, rightLimit, topLimit, bottomLimit, minWidth, maxWidth,
        minHeight, maxHeight, minRatio, maxRatio);
}

vector<CharInfo> CharSegment_V3::SplitePlateByOriginal(
    std::vector<std::vector<cv::Point>> &contours, cv::Mat &originalMat,
    cv::Mat &plateMat, PlateColor_t plateColor,
    CharSplitMethod_t charSplitMethod, int leftLimit, int rightLimit,
    int topLimit, int bottomLimit, int minWidth, int maxWidth, int minHeight,
    int maxHeight, float minRatio, float maxRatio) {
    vector<CharInfo> result;

    cv::Mat gray;
    cv::cvtColor(plateMat, gray, cv::COLOR_BGR2GRAY);

    if (plateColor == PlateColor_t::WhitePlate ||
        plateColor == PlateColor_t::YellowPlate ||
        plateColor == PlateColor_t::GreenPlate)
        gray = 255 - gray;
    // DebugVisualize("gray", gray);
    cv::Mat matOfClearMaodingAndBorder =
        ClearMaodingAndBorder(gray, plateColor);
    // DebugVisualize("gray", gray);
    // DEBUG
    // DebugVisualize("matOfClearMaodingAndBorder", matOfClearMaodingAndBorder);

    // std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(matOfClearMaodingAndBorder, contours, hierarchy,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    // DEBUG
    Mat contimage = matOfClearMaodingAndBorder.clone();
    cv::cvtColor(contimage, contimage, cv::COLOR_GRAY2BGR);
    cv::drawContours(contimage, contours, -1, Scalar(0, 0, 255), 1);
    DebugVisualize(
        (string("contimage ") +
         CharSplitMethod_tToString[static_cast<size_t>(charSplitMethod)])
            .c_str(),
        contimage);

    Mat rectedMat = matOfClearMaodingAndBorder.clone();
    cv::cvtColor(rectedMat, rectedMat, cv::COLOR_GRAY2BGR);
    for (auto &contour : contours) {
        Rect rect = cv::boundingRect(contour);
        cv::rectangle(rectedMat, rect, {0, 0, 255});
    }
    DebugVisualize(
        (string("contRect ") +
         CharSplitMethod_tToString[static_cast<size_t>(charSplitMethod)])
            .c_str(),
        rectedMat);

    vector<Rect> rects;
    for (size_t index = 0; index < contours.size(); index++) {
        Rect rect = cv::boundingRect(contours[index]);

        // DEBUG
        Mat pos = matOfClearMaodingAndBorder.clone();
        cv::cvtColor(pos, pos, cv::COLOR_GRAY2BGR);
        cv::rectangle(pos, rect, {0, 0, 255});
        // DebugVisualizeNotWait("rects", pos);

        if (NotOnBorder(rect, cv::Size(plateMat.cols, plateMat.rows), leftLimit,
                        rightLimit, topLimit, bottomLimit) &&
            VerifyRect(rect, minWidth, maxWidth, minHeight, maxHeight, minRatio,
                       maxRatio)) {
            rects.push_back(rect);
            // DebugVisualize("rects after judgement",
            // matOfClearMaodingAndBorder(rect));
        }
    }

    // DEBUG
    Mat rejectedRect = matOfClearMaodingAndBorder.clone();
    cv::cvtColor(rejectedRect, rejectedRect, cv::COLOR_GRAY2BGR);
    for (auto &rect : rects) {
        cv::rectangle(rejectedRect, rect, {0, 0, 255});
    }
    DebugVisualize("AfterClipBorder ", rejectedRect);

    rects = RejectInnerRectFromRects(rects);
    rects = AdjustRects(rects);

    if (rects.size() == 0)
        return result;

    for (size_t index = 0; index < rects.size(); index++) {
        Rect &rectROI = rects[index];
        rectROI = Utilities::GetSafeRect(rects[index], originalMat);
        CharInfo plateCharInfo;
        cv::Mat matROI = originalMat(rectROI);
        plateCharInfo.OriginalMat = matROI;
        plateCharInfo.OriginalRect = rectROI;
        plateCharInfo.CharSplitMethod = charSplitMethod;

        result.push_back(plateCharInfo);
    }
    sort(result.begin(), result.end(), CharInfoLeftComparer);
    return result;
}

bool CharSegment_V3::VerifyRect(Rect &rect, int minWidth, int maxWidth,
                                int minHeight, int maxHeight, float minRatio,
                                float maxRatio) {
    int width = rect.width;
    int height = rect.height;

    if (width == 0 || height == 0)
        return false;

    float ratio = (float)width / height;
    // float area = width * height;

    return ((width > minWidth && width < maxWidth) &&
            (height > minHeight && height < maxHeight) &&
            (ratio > minRatio && ratio < maxRatio));
}

bool CharSegment_V3::NotOnBorder(Rect &rectToJudge, cv::Size borderSize,
                                 int leftLimit, int rightLimit, int topLimit,
                                 int bottomLimit) {
    float leftPercent = leftLimit / 100.0;
    float rightPercent = rightLimit / 100.0;
    float topPercent = topLimit / 100.0;
    float bottomPercent = bottomLimit / 100.0;
    float widthPercent = 1.0 - leftPercent - rightPercent;
    float heightPercent = 1.0 - topPercent - bottomPercent;
    int xLimit = (int)(borderSize.width * leftPercent);
    int yLimit = (int)(borderSize.height * topPercent);
    int widthLimit = (int)(borderSize.width * widthPercent);
    int heightLimit = (int)(borderSize.height * heightPercent);

    Rect rectLimit;
    rectLimit.x = xLimit;
    rectLimit.y = yLimit;
    rectLimit.width = widthLimit + 1;
    rectLimit.height = heightLimit + 1;

    // contains 左闭右开，上闭下开
    bool ret =
        rectLimit.contains(cv::Point(rectToJudge.x, rectToJudge.y)) &&
        rectLimit.contains(
            cv::Point(rectToJudge.x, rectToJudge.y + rectToJudge.height)) &&
        rectLimit.contains(
            cv::Point(rectToJudge.x + rectToJudge.width, rectToJudge.y)) &&
        rectLimit.contains(cv::Point(rectToJudge.x + rectToJudge.width,
                                     rectToJudge.y + rectToJudge.height));
    return ret;
}

Rect CharSegment_V3::MergeRect(Rect &A, Rect &B) {
    Rect result;
    int minX = (A.x <= B.x) ? A.x : B.x;
    int minY = (A.y <= B.y) ? A.y : B.y;

    int ARight = A.x + A.width;
    int ABottom = A.y + A.height;
    int BRight = B.x + B.width;
    int BBottom = B.y + B.height;

    int maxRight = (ARight >= BRight) ? ARight : BRight;
    int maxBottom = (ABottom >= BBottom) ? ABottom : BBottom;

    result.x = minX;
    result.y = minY;
    result.width = maxRight - minX;
    result.height = maxBottom - minY;

    return result;
}

vector<Rect> CharSegment_V3::AdjustRects(vector<Rect> &rects) {
    float averageHeight = GetRectsAverageHeight(rects);
    float heightLimit = averageHeight * 0.5f;
    int medianTop = GetMedianRectsTop(rects);
    int medianBottom = GetMedianRectsBottom(rects);
    for (size_t index = rects.size() - 1; index < rects.size(); index--) {
        Rect rect = rects[index];
        if (rect.height >= heightLimit && rect.height < averageHeight) {
            int offsetTop = (rect.y - medianTop);
            int offsetBottom = std::abs(rect.y + rect.height - medianBottom);
            if (offsetTop > offsetBottom) {
                rect.y = (int)(rect.y + rect.height - averageHeight);
            }
            rect.height = (int)averageHeight + 5;
            rects[index] = rect;
        }
    }
    return rects;
}

vector<Rect> CharSegment_V3::MergeRects(vector<Rect> &rects) {
    vector<int> indexesOfMerge;
    vector<int> indexesBeMerged;
    // int maxHeight = GetRectsMaxHeight(rects);
    float averageHeight = GetRectsAverageHeight(rects);
    float hightLimit = averageHeight * 0.5f;
    vector<int>::iterator iter;

    for (size_t index = rects.size() - 1; index < rects.size(); index--) {
        if (find(indexesBeMerged.begin(), indexesBeMerged.end(), index) !=
            indexesBeMerged.end())
            continue;
        if (find(indexesOfMerge.begin(), indexesOfMerge.end(), index) !=
            indexesOfMerge.end())
            continue;
        Rect A = rects[index];
        if (A.height < hightLimit)
            continue;
        for (size_t i = rects.size() - 1; i < rects.size(); i--) {
            if (i == index)
                continue;
            Rect B = rects[i];
            if (B.height > hightLimit)
                continue;
            if ((A.x >= B.x && A.x + A.width >= B.x + B.width) ||
                (A.x <= B.x && A.x + A.width <= B.x + B.width)) {
                Rect rectMerge = MergeRect(A, B);
                if (VerifyRect(rectMerge)) {
                    indexesBeMerged.push_back(i);
                    rects[index] = rectMerge;
                    indexesOfMerge.push_back(index);
                }
            }
        }
    }
    vector<Rect> result;
    for (size_t index = 0; index < rects.size(); index++) {
        if (find(indexesBeMerged.begin(), indexesBeMerged.end(), index) ==
            indexesBeMerged.end()) {
            result.push_back(rects[index]);
        }
    }
    return result;
}

vector<Rect> CharSegment_V3::RejectInnerRectFromRects(vector<Rect> &rects) {
    for (size_t index = rects.size() - 1; index < rects.size(); index--) {
        const Rect &rect = rects[index];
        for (size_t i = 0; i < rects.size(); i++) {
            const Rect &rectTemp = rects[i];
            if ((rect.x + rect.width <= rectTemp.x + rectTemp.width &&
                 rect.y + rect.height <= rectTemp.y + rectTemp.height &&
                 rect.x >= rectTemp.x && rect.y >= rectTemp.y) &&
                (rect.width < rectTemp.width ||
                 rect.height < rectTemp.height)) {
                rects.erase(rects.begin() + i);
                break;
            }
        }
    }
    return rects;
}

float CharSegment_V3::GetRectsAverageHeight(vector<Rect> &rects) {
    float heightTotal = 0.0;
    if (rects.size() == 0)
        return heightTotal;
    for (Rect rect : rects) {
        heightTotal += rect.height;
    }
    return heightTotal / (float)rects.size();
}

int CharSegment_V3::GetRectsMaxHeight(vector<Rect> &rects) {
    int maxHeight = 0;
    if (rects.size() == 0)
        return maxHeight;
    for (Rect rect : rects) {
        if (maxHeight < rect.height)
            maxHeight = rect.height;
    }
    return maxHeight;
}

int CharSegment_V3::GetMedianRectsTop(vector<Rect> &rects) {
    if (rects.size() == 0)
        return 0;
    std::sort(rects.begin(), rects.end(), RectTopComparer);
    int midianIndex = rects.size() / 2;
    return rects[midianIndex].y;
}

int CharSegment_V3::GetMedianRectsBottom(vector<Rect> &rects) {
    if (rects.size() == 0)
        return 0;
    std::sort(rects.begin(), rects.end(), RectBottomComparer);
    int midianIndex = rects.size() / 2;
    return rects[midianIndex].y + rects[midianIndex].height;
}

bool CharSegment_V3::RectTopComparer(const Rect &x, const Rect &y) {
    return x.y < y.y;
}
bool CharSegment_V3::RectBottomComparer(const Rect &x, const Rect &y) {
    return x.y + x.height < y.y + y.height;
}
bool CharSegment_V3::RectHeightComparer(const Rect &x, const Rect &y) {
    return x.height < y.height;
}
bool CharSegment_V3::RectLeftComparer(const Rect &x, const Rect &y) {
    return x.x < y.x;
}
bool CharSegment_V3::CharInfoLeftComparer(const CharInfo &x,
                                          const CharInfo &y) {
    return x.OriginalRect.x < y.OriginalRect.x;
}