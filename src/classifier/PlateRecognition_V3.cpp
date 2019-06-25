#include "debug.h"

#include "CharInfo.h"
#include "CharSegment_V3.h"
#include "PlateLocator_V3.h"
#include "PlateRecognition_V3.h"
#include "PlateChar_SVM.h"
#include <numeric>

using namespace Doit::CV::PlateRecogn;

vector<PlateInfo> PlateRecognition_V3::Recognite(Mat &matSource) {
    vector<PlateInfo> result = vector<PlateInfo>();
    vector<PlateInfo> plateInfosLocate =
        PlateLocator_V3::LocatePlates(matSource);
    for (size_t index = 0; index < plateInfosLocate.size(); index++) {
        PlateInfo plateInfo = plateInfosLocate[index];
        shared_ptr<PlateInfo> plateInfoOfHandled =
            GetPlateInfoByMutilMethodAndMutilColor(plateInfo);
        if (plateInfoOfHandled != null) {
            plateInfoOfHandled->PlateCategory = plateInfo.PlateCategory;

            result.push_back(*plateInfoOfHandled);
        }
    }
    return result;
}

// 返回值可能是null，改成指针
shared_ptr<PlateInfo>
PlateRecognition_V3::GetPlateInfoByMutilMethodAndMutilColor(
    PlateInfo &plateInfo) {
    PlateInfo *result = null;
    if (plateInfo.OriginalMat.empty())
        return shared_ptr<PlateInfo>(result);
    PlateInfo plateInfo_Blue =
        GetPlateInfoByMutilMethod(plateInfo, PlateColor_t::BluePlate);
    PlateInfo plateInfo_Yello =
        GetPlateInfoByMutilMethod(plateInfo, PlateColor_t::YellowPlate);

    if (GetCharCount(plateInfo_Blue) > GetCharCount(plateInfo_Yello)) {
        plateInfo_Blue.PlateColor = PlateColor_t::BluePlate;
        return shared_ptr<PlateInfo>(new PlateInfo(plateInfo_Blue));
    } else {
        plateInfo_Yello.PlateColor = PlateColor_t::YellowPlate;
        return shared_ptr<PlateInfo>(new PlateInfo(plateInfo_Yello));
    }
}

bool PlateRecognition_V3::JudgePlateRightful(const PlateInfo &plateInfo) {
    if (plateInfo.CharInfos.empty() || plateInfo.CharInfos.size() == 0)
        return false;
    if (plateInfo.PlateColor == PlateColor_t::UnknownPlate)
        return false;
    int charCount = 0;
    for (auto charInfo : plateInfo.CharInfos) {
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            charCount++;
        }
    }
    return (charCount >= 5);
}

int PlateRecognition_V3::GetCharCount(const PlateInfo &plateInfo) {
    if (plateInfo.CharInfos.empty() || plateInfo.CharInfos.size() == 0)
        return 0;
    if (plateInfo.PlateColor == PlateColor_t::UnknownPlate)
        return 0;
    int charCount = 0;
    for (auto charInfo : plateInfo.CharInfos) {
        if (charInfo.PlateChar != PlateChar_t::NonChar) {
            charCount++;
        }
    }
    return charCount;
}

PlateInfo
PlateRecognition_V3::GetPlateInfoByMutilMethod(PlateInfo &plateInfo,
                                               PlateColor_t plateColor) {
    PlateInfo plateInfoByOriginal =
        GetPlateInfo(plateInfo, plateColor, CharSplitMethod_t::Origin);
    PlateInfo plateInfoByGamma =
        GetPlateInfo(plateInfo, plateColor, CharSplitMethod_t::Gamma);
    PlateInfo plateInfoByIndex =
        GetPlateInfo(plateInfo, plateColor, CharSplitMethod_t::Exponential);
    PlateInfo plateInfoByLog =
        GetPlateInfo(plateInfo, plateColor, CharSplitMethod_t::Log);
    vector<PlateInfo> plateInfos = vector<PlateInfo>();
    if (!plateInfoByOriginal.CharInfos.empty()) {
        plateInfos.push_back(plateInfoByOriginal);
    }
    if (!plateInfoByGamma.CharInfos.empty()) {
        plateInfos.push_back(plateInfoByGamma);
    }
    if (!plateInfoByIndex.CharInfos.empty()) {
        plateInfos.push_back(plateInfoByIndex);
    }
    if (!plateInfoByLog.CharInfos.empty()) {
        plateInfos.push_back(plateInfoByLog);
    }
    if (plateInfos.size() == 0)
        return PlateInfo();
    sort(plateInfos.begin(), plateInfos.end(), PlateInfoComparer_DESC());

    PlateInfo result = plateInfos[0];
    return result;
}

PlateInfo PlateRecognition_V3::GetPlateInfo(PlateInfo &plateInfo,
                                            PlateColor_t plateColor,
                                            CharSplitMethod_t splitMethod) {
    PlateInfo result = PlateInfo();
    result.PlateCategory = plateInfo.PlateCategory;
    result.OriginalMat = plateInfo.OriginalMat;
    result.OriginalRect = plateInfo.OriginalRect;
    result.PlateLocateMethod = plateInfo.PlateLocateMethod;
    result.PlateColor = plateColor;
    vector<CharInfo> charInfos = vector<CharInfo>();

    std::vector<std::vector<cv::Point>> contours;

    switch (splitMethod) {
    case CharSplitMethod_t::Gamma:
        charInfos = CharSegment_V3::SplitePlateByGammaTransform(
            contours, plateInfo.OriginalMat, plateColor);
        break;
    case CharSplitMethod_t::Exponential:
        charInfos = CharSegment_V3::SplitePlateByIndexTransform(
            contours, plateInfo.OriginalMat, plateColor);
        break;
    case CharSplitMethod_t::Log:
        charInfos = CharSegment_V3::SplitePlateByLogTransform(
            contours, plateInfo.OriginalMat, plateColor);
        break;
    case CharSplitMethod_t::Origin:
    default:
        charInfos = CharSegment_V3::SplitePlateByOriginal(
            contours, plateInfo.OriginalMat, plateInfo.OriginalMat, plateColor);
        break;
    }

    auto combineVerticalOrigin = [](vector<CharInfo> &charInfos,
                                    PlateInfo &plateInfo) {
        for (size_t index = 1; index < min(charInfos.size(), 3ul); ++index) {
            CharInfo &lastCharInfo = charInfos[index - 1];
            Rect &lastRect = charInfos[index - 1].OriginalRect;
            Rect &rect = charInfos[index].OriginalRect;
            int l1 = rect.x, l2 = lastRect.x, r1 = rect.x + rect.width,
                r2 = lastRect.x + lastRect.width;
            int intersectionLength = -1;
            if ((intersectionLength = min(r1, r2) - max(l1, l2)) > 0) {
                float p1 = float(intersectionLength) / (r1 - l1);
                float p2 = float(intersectionLength) / (r2 - l2);
                if (p1 > 0.5 || p2 > 0.5) {
                    int t1 = rect.y, b1 = rect.y + rect.height, t2 = lastRect.y,
                        b2 = lastRect.y + lastRect.height;
                    lastRect = {min(l1, l2), min(t1, t2),
                                max(r1, r2) - min(l1, l2),
                                max(b1, b2) - min(t1, t2)};
                    lastCharInfo.OriginalMat = plateInfo.OriginalMat(lastRect);

                    charInfos.erase(charInfos.begin() + index);
                    --index;
                }
            }
        }
    };
    if (plateInfo.PlateCategory != PlateCategory_t::HongkomgPlate2Row &&
        plateInfo.PlateCategory != PlateCategory_t::MacaoPlate2Row &&
        plateInfo.PlateCategory != PlateCategory_t::MilitrayPlate2Row &&
        plateInfo.PlateCategory != PlateCategory_t::NormalPlate2Row)
        combineVerticalOrigin(charInfos, plateInfo);

    // Mat combinedMat = plateInfo.OriginalMat.clone();
    // for(auto &charInfo : charInfos){
    //     cv::rectangle(combinedMat, charInfo.OriginalRect, {0, 0, 255});
    // }
    // DebugVisualize("combined Rects ", combinedMat);

    for (size_t index = charInfos.size() - 1; index < charInfos.size();
         index--) {
        CharInfo &charInfo = charInfos[index];
        PlateChar_t plateChar = PlateChar_SVM::Test(charInfo.OriginalMat);
        if (plateChar == PlateChar_t::NonChar) {
            charInfos.erase(index + charInfos.begin());
        } else
            charInfo.PlateChar = plateChar;
    }
    result.CharInfos = charInfos;

    CheckLeftAndRightToRemove(result);
    CheckPlateColor(result);

    auto rectCenter = [](const Rect &rect) {
        return cv::Point2i{rect.x + rect.width / 2, rect.y + rect.height / 2};
    };
    auto checkStartChineseCharacter = [&](PlateInfo &result) {
        const vector<CharInfo> &charInfos = result.CharInfos;
        if (charInfos.size() == 0) {
            return;
        }
        const CharInfo &firstChar = charInfos[0];
        if (firstChar.PlateChar >= PlateChar_t::BeiJing &&
            firstChar.PlateChar <= PlateChar_t::JingChe) {
            return;
        }

        if (charInfos.size() == 6) {
            float xInterval = (rectCenter(charInfos[5].OriginalRect).x -
                               rectCenter(charInfos[1].OriginalRect).x) /
                              4.f;
            int xFirst = charInfos[0].OriginalRect.x - xInterval;
            float meanHeight =
                std::accumulate(charInfos.begin(), charInfos.end(), 0.f,
                                [](float init, const CharInfo &next) -> float {
                                    return init += next.OriginalRect.height;
                                });
            meanHeight /= charInfos.size();
            float meanWidth =
                std::accumulate(charInfos.begin(), charInfos.end(), 0.f,
                                [](float init, const CharInfo &next) -> float {
                                    return init += next.OriginalRect.width;
                                });
            meanWidth /= charInfos.size();
            float meanY =
                std::accumulate(charInfos.begin(), charInfos.end(), 0.f,
                                [](float init, const CharInfo &next) -> float {
                                    return init += next.OriginalRect.y +
                                                   next.OriginalRect.height / 2;
                                });
            meanY /= charInfos.size();

            Rect first = Rect(cv::Point2i(xFirst, meanY - meanHeight / 2),
                              cv::Size(meanWidth, meanHeight));
            Rect firstOutLimit =
                Rect(cv::Point2i(xFirst - 5, meanY - meanHeight / 2 - 5),
                     cv::Size(meanWidth + 5 * 2, meanHeight + 5 * 2));
            vector<CharInfo> intersected = {};
            Rect firstCharRect;
            for (auto &ct : contours) {
                Rect bRect = cv::boundingRect(ct);
                bool intersectInternal = (bRect & first).area() > 0;
                bool containOuter = (bRect & firstOutLimit) == bRect;
                if (intersectInternal && containOuter) {
                    firstCharRect |= bRect;
                }
            }
            if(firstCharRect.area() == 0)
                return;

            Mat fistInner = plateInfo.OriginalMat.clone();
            cv::rectangle(fistInner, first, {0, 0, 255});
            cv::rectangle(fistInner, firstOutLimit, {0, 255, 0});
            DebugVisualize("fistInner", fistInner);

            Mat firstMat = plateInfo.OriginalMat(firstCharRect);
            PlateChar_t firstRecoginzedChar = PlateChar_SVM::Test(firstMat);
            if (firstRecoginzedChar >= PlateChar_t::BeiJing &&
                firstRecoginzedChar <= PlateChar_t::JingChe) {
                result.CharInfos.insert(
                    result.CharInfos.begin(),
                    {firstRecoginzedChar, plateInfo.OriginalMat(firstCharRect),
                     firstCharRect, PlateLocateMethod_t::Unknown, splitMethod});
            }
        }
    };
    checkStartChineseCharacter(result);

    return result;
}

void PlateRecognition_V3::CheckLeftAndRightToRemove(PlateInfo &plateInfo) {
    if (plateInfo.PlateCategory == PlateCategory_t::NonPlate)
        return;
    if (plateInfo.CharInfos.empty())
        return;
    if (plateInfo.CharInfos.size() < 4)
        return;
    int charCount = plateInfo.CharInfos.size();

    //两头的先除开，去掉中间的汉字
    for (int index = charCount - 1 - 1; index > 0 + 1; index--) {
        const CharInfo &charInfo = plateInfo.CharInfos[index];
        int charInfoValue = (int)charInfo.PlateChar;

        if (charInfoValue >= (int)PlateChar_t::BeiJing &&
            charInfoValue <= (int)PlateChar_t::JingChe) {
            plateInfo.CharInfos.erase(plateInfo.CharInfos.begin() + index);
        }
    }
    charCount = plateInfo.CharInfos.size();

    const CharInfo &second = plateInfo.CharInfos[1];
    int secondValue = (int)second.PlateChar;

    switch (plateInfo.PlateCategory) {
    case PlateCategory_t::NormalPlate:
    case PlateCategory_t::NormalPlate2Row:
    case PlateCategory_t::MacaoPlateInternal:
    case PlateCategory_t::HongkongPlateInternal:
    case PlateCategory_t::PolicePlate:
        if (secondValue >= (int)PlateChar_t::BeiJing &&
            secondValue <= (int)PlateChar_t::JingChe) {
            plateInfo.CharInfos.erase(
                plateInfo.CharInfos
                    .begin()); //如果第⼆个是汉字，那就去掉第⼀个字符
        }
        charCount = plateInfo.CharInfos.size();

        if (charCount > 7)
            plateInfo.CharInfos.erase(plateInfo.CharInfos.begin() + charCount -
                                      1);
        break;
    case PlateCategory_t::MacaoPlate:
        break;
    case PlateCategory_t::MacaoPlate2Row:
        break;
    case PlateCategory_t::HongkongPlate:
        break;
    case PlateCategory_t::HongkomgPlate2Row:
        break;
    case PlateCategory_t::MilitaryPlate:
        break;
    case PlateCategory_t::MilitrayPlate2Row:
        break;
    case PlateCategory_t::AlternativeEnergyPlate:
        break;

    default:
        break;
    }
    charCount = plateInfo.CharInfos.size();
    if (charCount < 7)
        return;
    const CharInfo &first = plateInfo.CharInfos[0];
    int firstValue = (int)first.PlateChar;
    const CharInfo &second2 = plateInfo.CharInfos[1];
    int secondValue2 = (int)second2.PlateChar;
    const CharInfo &lastFirst = plateInfo.CharInfos[charCount - 1];
    int lastFirstValue = (int)lastFirst.PlateChar;
    switch (plateInfo.PlateCategory) {
    case PlateCategory_t::NormalPlate:
        if (lastFirstValue >= (int)PlateChar_t::BeiJing &&
            lastFirstValue <= (int)PlateChar_t::JingChe) {
            plateInfo.CharInfos.erase(plateInfo.CharInfos.begin() + charCount -
                                      1); //去掉最后⼀位汉字
        }
        if (firstValue <= (int)PlateChar_t::Point) {
            plateInfo.CharInfos.erase(
                plateInfo.CharInfos.begin()); //如果第⼀位为⾮汉字，删除
        }
        if (secondValue2 >= (int)PlateChar_t::_0 &&
            secondValue2 <= (int)PlateChar_t::_9) {
            plateInfo.CharInfos.erase(plateInfo.CharInfos.begin() +
                                      1); //如果第⼆位为数字，删除
        }
        break;
    default:
        break;
    }
}

void PlateRecognition_V3::CheckPlateColor(PlateInfo &plateInfo) {
    if (plateInfo.PlateCategory == PlateCategory_t::NonPlate)
        return;
    switch (plateInfo.PlateCategory) {
    case PlateCategory_t::NormalPlate:
        break;
    case PlateCategory_t::NormalPlate2Row:
        plateInfo.PlateColor = PlateColor_t::YellowPlate;
        break;
    case PlateCategory_t::MacaoPlateInternal:
    case PlateCategory_t::HongkongPlateInternal:
        plateInfo.PlateColor = PlateColor_t::BlackPlate;
        break;
    case PlateCategory_t::PolicePlate:
    case PlateCategory_t::HongkongPlate:
    case PlateCategory_t::HongkomgPlate2Row:
        plateInfo.PlateColor = PlateColor_t::WhitePlate;
        break;
    case PlateCategory_t::MacaoPlate:
        break;
    case PlateCategory_t::MacaoPlate2Row:
        break;
    case PlateCategory_t::MilitaryPlate:
        break;
    case PlateCategory_t::MilitrayPlate2Row:
        break;
    case PlateCategory_t::AlternativeEnergyPlate:
        plateInfo.PlateColor = PlateColor_t::GreenPlate;
        break;
    default:
        break;
    }
}

bool PlateRecognition_V3::PlateInfoComparer_DESC::operator()(const PlateInfo &x, const PlateInfo &y) {
    return y.CharInfos.size() < x.CharInfos.size();
}