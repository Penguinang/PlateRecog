#include <string>
using std::string;
#include <utility>
using std::tuple;

#include "CharInfo.h"
#include "CharSegment_V3.h"
#include "PlateChar_SVM.h"
#include "Utilities.h"
#include "debug.h"

using cv::Point;

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

// vector<CharInfo>
// @return {charinfos, binary image, rected image}
vector<tuple<vector<CharInfo>, Mat, Mat>>
CharSegment_V3::SplitePlateForAutoSample(cv::Mat &plateMat) {

    // For blue
    std::vector<std::vector<cv::Point>> contours_Original_Blue;
    std::vector<std::vector<cv::Point>> contours_IndexTransform_Blue;
    std::vector<std::vector<cv::Point>> contours_GammaTransform_Blue;
    std::vector<std::vector<cv::Point>> contours_LogTransform_Blue;
    vector<CharInfo> charInfos_Original_Blue = SplitePlateByOriginal(
        contours_Original_Blue, plateMat, plateMat, PlateColor_t::BluePlate);
    Mat binaryMat_Original_Blue = plateMat.clone() = 0;
    cv::drawContours(binaryMat_Original_Blue, contours_Original_Blue, -1,
                     {255, 255, 255}, cv::FILLED);
    Mat rectedMat_Original_Blue = plateMat.clone();
    reserveBoundingRects(rectedMat_Original_Blue, contours_Original_Blue, -1,
                     {0, 0, 255});

    vector<CharInfo> charInfos_IndexTransform_Blue =
        SplitePlateByIndexTransform(contours_IndexTransform_Blue, plateMat,
                                    PlateColor_t::BluePlate);
    Mat binaryMat_IndexTransform_Blue = plateMat.clone() = 0;
    cv::drawContours(binaryMat_IndexTransform_Blue,
                     contours_IndexTransform_Blue, -1, {255, 255, 255}, cv::FILLED);
    Mat rectedMat_IndexTransform_Blue = plateMat.clone();
    reserveBoundingRects(rectedMat_IndexTransform_Blue, contours_IndexTransform_Blue, -1,
                     {0, 0, 255});

    vector<CharInfo> charInfos_GammaTransform_Blue =
        SplitePlateByGammaTransform(contours_GammaTransform_Blue, plateMat,
                                    PlateColor_t::BluePlate);
    Mat binaryMat_GammaTransform_Blue = plateMat.clone() = 0;
    cv::drawContours(binaryMat_GammaTransform_Blue,
                     contours_GammaTransform_Blue, -1, {255, 255, 255}, cv::FILLED);
    Mat rectedMat_GammaTransform_Blue = plateMat.clone();
    reserveBoundingRects(rectedMat_GammaTransform_Blue, contours_GammaTransform_Blue, -1,
                     {0, 0, 255});

    vector<CharInfo> charInfos_LogTransform_Blue = SplitePlateByLogTransform(
        contours_LogTransform_Blue, plateMat, PlateColor_t::BluePlate);
    Mat binaryMat_LogTransform_Blue = plateMat.clone() = 0;
    cv::drawContours(binaryMat_LogTransform_Blue, contours_LogTransform_Blue,
                     -1, {255, 255, 255}, cv::FILLED);
    Mat rectedMat_LogTransform_Blue = plateMat.clone();
    reserveBoundingRects(rectedMat_LogTransform_Blue, contours_LogTransform_Blue, -1,
                     {0, 0, 255});

    int isCharCount_Blue = 0;

    for (auto &charInfo : charInfos_Original_Blue) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Blue++;
        }
    }
    for (auto &charInfo : charInfos_IndexTransform_Blue) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Blue++;
        }
    }
    for (auto &charInfo : charInfos_GammaTransform_Blue) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Blue++;
        }
    }
    for (auto &charInfo : charInfos_LogTransform_Blue) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Blue++;
        }
    }

    // For yellow
    std::vector<std::vector<cv::Point>> contours_Original_Yellow;
    std::vector<std::vector<cv::Point>> contours_IndexTransform_Yellow;
    std::vector<std::vector<cv::Point>> contours_GammaTransform_Yellow;
    std::vector<std::vector<cv::Point>> contours_LogTransform_Yellow;
    vector<CharInfo> charInfos_Original_Yellow =
        SplitePlateByOriginal(contours_Original_Yellow, plateMat, plateMat,
                              PlateColor_t::YellowPlate);
    Mat binaryMat_Original_Yellow = plateMat.clone() = 0;
    cv::drawContours(binaryMat_Original_Yellow, contours_Original_Yellow, -1,
                     {255, 255, 255}, cv::FILLED);
    Mat rectedMat_Original_Yellow = plateMat.clone();
    reserveBoundingRects(rectedMat_Original_Yellow, contours_Original_Yellow, -1,
                     {0, 0, 255});

    vector<CharInfo> charInfos_IndexTransform_Yellow =
        SplitePlateByIndexTransform(contours_IndexTransform_Yellow, plateMat,
                                    PlateColor_t::YellowPlate);
    Mat binaryMat_IndexTransform_Yellow = plateMat.clone() = 0;
    cv::drawContours(binaryMat_IndexTransform_Yellow,
                     contours_IndexTransform_Yellow, -1, {255, 255, 255}, cv::FILLED);
    Mat rectedMat_IndexTransform_Yellow = plateMat.clone();
    reserveBoundingRects(rectedMat_IndexTransform_Yellow, contours_IndexTransform_Yellow, -1,
                     {0, 0, 255});

    vector<CharInfo> charInfos_GammaTransform_Yellow =
        SplitePlateByGammaTransform(contours_GammaTransform_Yellow, plateMat,
                                    PlateColor_t::YellowPlate);
    Mat binaryMat_GammaTransform_Yellow = plateMat.clone() = 0;
    cv::drawContours(binaryMat_GammaTransform_Yellow,
                     contours_GammaTransform_Yellow, -1, {255, 255, 255}, cv::FILLED);
    Mat rectedMat_GammaTransform_Yellow = plateMat.clone();
    reserveBoundingRects(rectedMat_GammaTransform_Yellow, contours_GammaTransform_Yellow, -1,
                     {0, 0, 255});

    vector<CharInfo> charInfos_LogTransform_Yellow = SplitePlateByLogTransform(
        contours_LogTransform_Yellow, plateMat, PlateColor_t::YellowPlate);
    Mat binaryMat_LogTransform_Yellow = plateMat.clone() = 0;
    cv::drawContours(binaryMat_LogTransform_Yellow, contours_LogTransform_Yellow,
                     -1, {255, 255, 255}, cv::FILLED);
    Mat rectedMat_LogTransform_Yellow = plateMat.clone();
    reserveBoundingRects(rectedMat_LogTransform_Yellow, contours_LogTransform_Yellow, -1,
                     {0, 0, 255});


    int isCharCount_Yellow = 0;

    for (auto &charInfo : charInfos_Original_Yellow) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Yellow++;
        }
    }
    for (auto &charInfo : charInfos_IndexTransform_Yellow) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Yellow++;
        }
    }
    for (auto &charInfo : charInfos_GammaTransform_Yellow) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Yellow++;
        }
    }
    for (auto &charInfo : charInfos_LogTransform_Yellow) {
        charInfo.PlateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            isCharCount_Yellow++;
        }
    }

    if (isCharCount_Blue > isCharCount_Yellow) {
        return {{charInfos_Original_Blue, binaryMat_Original_Blue, rectedMat_Original_Blue},
                {charInfos_IndexTransform_Blue, binaryMat_IndexTransform_Blue, rectedMat_IndexTransform_Blue},
                {charInfos_GammaTransform_Blue, binaryMat_GammaTransform_Blue, rectedMat_GammaTransform_Blue},
                {charInfos_LogTransform_Blue, binaryMat_LogTransform_Blue, rectedMat_LogTransform_Blue}};
    } else {
        return {{charInfos_Original_Yellow, binaryMat_Original_Yellow, rectedMat_Original_Yellow},
                {charInfos_IndexTransform_Yellow, binaryMat_IndexTransform_Yellow, rectedMat_IndexTransform_Yellow},
                {charInfos_GammaTransform_Yellow, binaryMat_GammaTransform_Yellow, rectedMat_GammaTransform_Yellow},
                {charInfos_LogTransform_Yellow, binaryMat_LogTransform_Yellow, rectedMat_LogTransform_Yellow}};
    }
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

    rects = RejectInnerRectFromRects(rects);

    rects = AdjustRects(rects);
    // DEBUG
    Mat rejectedRect = matOfClearMaodingAndBorder.clone();
    cv::cvtColor(rejectedRect, rejectedRect, cv::COLOR_GRAY2BGR);
    for (auto &rect : rects) {
        cv::rectangle(rejectedRect, rect, {0, 0, 255});
    }
    DebugVisualize("AfterClipBorder ", rejectedRect);
    if (rects.size() == 0)
        return result;

    /*vector<Rect> candidateRects;
    for (size_t index = 0; index < rects.size(); index++)
    {
        Mat matTest;
        originalMat(rects[index]).convertTo(matTest, cv::ImreadModes::IMREAD_GRAYSCALE);
        if (PlateChar_SVM::Test(matTest) != PlateChar_t::NonChar)
        {
            candidateRects.push_back(rects[index]);
        }
    }*/
    /*int midHeight = CharSegment_V3::GetRectsMaxHeight(rects);
    int midWidth = CharSegment_V3::GetRectsMidWidth(rects);*/
    for (size_t index = 0; index < rects.size(); index++) {
        //printf("midWidth %d\n", midWidth);
        //printf("rectWidth %d\n", rects[index].width);
        // broaden thin characters
        Rect rect = rects[index];
       /* midWidth = rect.width * 3 / 2;
        if (rect.height >= midHeight * 2 / 3 && rect.height <= midHeight * 4 / 3 &&
            rect.width <= midWidth * 2 / 3)
        {
            int updateX = rect.x + rect.width/2 - midWidth/2;
            int updateWidth = midWidth;
            if (updateX >0 && updateX + updateWidth >= originalMat.cols)
            {
                updateWidth = originalMat.cols - rect.x - rect.width / 2;
                updateX = rect.x + rect.width / 2 - updateWidth / 2;
                rect.x = updateX;
                rect.width = updateWidth;
            }
            else if (updateX + updateWidth < originalMat.cols && updateX >0)
            {
                rect.x = updateX;
                rect.width = updateWidth;
            }
        }*/
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
    rectLimit.width = widthLimit;
    rectLimit.height = heightLimit;

    return rectLimit.contains(cv::Point(rectToJudge.x, rectToJudge.y)) &&
           rectLimit.contains(
               cv::Point(rectToJudge.x, rectToJudge.y + rectToJudge.height-1)) &&
           rectLimit.contains(
               cv::Point(rectToJudge.x + rectToJudge.width-1, rectToJudge.y)) &&
           rectLimit.contains(cv::Point(rectToJudge.x + rectToJudge.width-1,
                                        rectToJudge.y + rectToJudge.height-1));
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
            int offsetTop = std::abs(rect.y - medianTop);
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

float CharSegment_V3::GetRectsMidHeight(vector<Rect> &rects) {
    if (rects.size() == 0)
    {
        return 0;
    }
    vector<Rect> tempRects;
    tempRects.assign(rects.begin(), rects.end());
    std::sort(tempRects.begin(), tempRects.end(), RectHeightComparer);
    return tempRects[int(tempRects.size() / 2)].height;
}

float CharSegment_V3::GetRectsMidWidth(vector<Rect> &rects) {
    if (rects.size() == 0)
    {
        return 0;
    }
    vector<Rect> tempRects;
    tempRects.assign(rects.begin(), rects.end());
    std::sort(tempRects.begin(), tempRects.end(), RectHeightComparer);
    return tempRects[int(tempRects.size() / 2)].width;
}

int CharSegment_V3::GetRectsMaxWidth(vector<Rect>& rects)
{
    int maxWidth = 0;
    if (rects.size() == 0)
        return maxWidth;
    for (Rect rect : rects) {
        for (Rect rect : rects) {
            if (maxWidth < rect.width)
                maxWidth = rect.width;
        }
        return maxWidth;
    }
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
bool CharSegment_V3::RectWidthComparer(const Rect&x, const Rect &y) {
    return x.width < y.width;
}
bool CharSegment_V3::RectLeftComparer(const Rect &x, const Rect &y) {
    return x.x < y.x;
}
bool CharSegment_V3::CharInfoLeftComparer(const CharInfo &x,
                                          const CharInfo &y) {
    return x.OriginalRect.x < y.OriginalRect.x;
}
