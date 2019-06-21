#ifndef PLATE_RECOGNITION_H
#define PLATE_RECOGNITION_H


#include <memory>
using std::shared_ptr;
#include <algorithm>
using std::sort;
#include <string>
using std::string;

#include "CharInfo.h"
#include "csharpImplementations.h"

#include "PlateChar_SVM.h"

// class PlateLocator_V3{
// public:
//     static vector<Doit::CV::PlateRecogn::PlateInfo> LocatePlates(Mat &);

// };

// class CharSegment_V3{
// public:
//     static vector<Doit::CV::PlateRecogn::CharInfo> SplitePlateByGammaTransform(Mat, Doit::CV::PlateRecogn::PlateColor_t);
//     static vector<Doit::CV::PlateRecogn::CharInfo> SplitePlateByIndexTransform(Mat, Doit::CV::PlateRecogn::PlateColor_t);
//     static vector<Doit::CV::PlateRecogn::CharInfo> SplitePlateByLogTransform(Mat, Doit::CV::PlateRecogn::PlateColor_t);
//     static vector<Doit::CV::PlateRecogn::CharInfo> SplitePlateByOriginal(Mat, Mat, Doit::CV::PlateRecogn::PlateColor_t);
// };

namespace Doit {
namespace CV {
namespace PlateRecogn {
    class PlateRecognition_V3{
        public:
        static vector<PlateInfo> Recognite (Mat &matSource) {
            vector<PlateInfo> result = vector<PlateInfo> ();
            vector<PlateInfo> plateInfosLocate = PlateLocator_V3::LocatePlates (matSource);
            for (int index = 0; index < plateInfosLocate.size(); index++) {
                PlateInfo plateInfo = plateInfosLocate[index];
                shared_ptr<PlateInfo> plateInfoOfHandled = GetPlateInfoByMutilMethodAndMutilColor (plateInfo);
                if (plateInfoOfHandled != null) {
                    plateInfoOfHandled->PlateCategory = plateInfo.PlateCategory;

                    result.push_back(*plateInfoOfHandled);
                }
            }
            return result;
        }

        // 返回值可能是null，改成指针
        public:
        static shared_ptr<PlateInfo> GetPlateInfoByMutilMethodAndMutilColor (PlateInfo &plateInfo) {
            PlateInfo *result = null;
            if (plateInfo.OriginalMat.empty()) return shared_ptr<PlateInfo>(result);
            PlateInfo plateInfo_Blue = GetPlateInfoByMutilMethod (plateInfo, PlateColor_t::BluePlate);
            if (JudgePlateRightful (plateInfo_Blue) == true) {
                plateInfo_Blue.PlateColor = PlateColor_t::BluePlate;
                return shared_ptr<PlateInfo>(new PlateInfo(plateInfo_Blue));
            }
            PlateInfo plateInfo_Yello = GetPlateInfoByMutilMethod (plateInfo, PlateColor_t::YellowPlate);
            if (JudgePlateRightful (plateInfo_Yello) == true) {
                plateInfo_Yello.PlateColor = PlateColor_t::YellowPlate;
                return shared_ptr<PlateInfo>(new PlateInfo(plateInfo_Yello));
            }
            return shared_ptr<PlateInfo>(result);
        }
        public:
        static bool JudgePlateRightful (const PlateInfo &plateInfo) {
            if (plateInfo.CharInfos.empty() || plateInfo.CharInfos.size() == 0) return false;
            if (plateInfo.PlateColor == PlateColor_t::UnknownPlate) return false;
            int charCount = 0;
            for (auto charInfo : plateInfo.CharInfos) {
                if (charInfo.PlateChar != PlateChar_t::NonChar) {
                    charCount++;
                }
            }
            return (charCount >= 5);
        }
        public:
        static PlateInfo GetPlateInfoByMutilMethod (PlateInfo &plateInfo, PlateColor_t plateColor) {
            PlateInfo plateInfoByOriginal = GetPlateInfo (plateInfo, plateColor, CharSplitMethod_t::Origin);
            PlateInfo plateInfoByGamma = GetPlateInfo (plateInfo, plateColor, CharSplitMethod_t::Gamma);
            PlateInfo plateInfoByIndex = GetPlateInfo (plateInfo, plateColor, CharSplitMethod_t::Exponential);
            PlateInfo plateInfoByLog = GetPlateInfo (plateInfo, plateColor, CharSplitMethod_t::Log);
            vector<PlateInfo> plateInfos = vector<PlateInfo> ();
            if (!plateInfoByOriginal.CharInfos.empty()) {
                plateInfos.push_back (plateInfoByOriginal);
            }
            if (!plateInfoByGamma.CharInfos.empty()) {
                plateInfos.push_back (plateInfoByGamma);
            }
            if (!plateInfoByIndex.CharInfos.empty()) {
                plateInfos.push_back (plateInfoByIndex);
            }
            if (!plateInfoByLog.CharInfos.empty()) {
                plateInfos.push_back (plateInfoByLog);
            }
            if (plateInfos.size() == 0) return PlateInfo ();
                sort(plateInfos.begin(), plateInfos.end(), PlateInfoComparer_DESC());

            PlateInfo result = plateInfos[0];
            return result;
        }
        public:
        static PlateInfo GetPlateInfo (PlateInfo &plateInfo, PlateColor_t plateColor, CharSplitMethod_t splitMethod) {
            PlateInfo result = PlateInfo ();
            result.PlateCategory = plateInfo.PlateCategory;
            result.OriginalMat = plateInfo.OriginalMat;
            result.OriginalRect = plateInfo.OriginalRect;
            result.PlateLocateMethod = plateInfo.PlateLocateMethod;
            result.PlateColor = plateColor;
            vector<CharInfo> charInfos = vector<CharInfo> ();
            switch (splitMethod) {
                case CharSplitMethod_t::Gamma:
                    charInfos = CharSegment_V3::SplitePlateByGammaTransform (plateInfo.OriginalMat,
                        plateColor);
                    break;
                case CharSplitMethod_t::Exponential:
                    charInfos = CharSegment_V3::SplitePlateByIndexTransform (plateInfo.OriginalMat, plateColor);
                    break;
                case CharSplitMethod_t::Log:
                    charInfos = CharSegment_V3::SplitePlateByLogTransform (plateInfo.OriginalMat, plateColor);
                    break;
                case CharSplitMethod_t::Origin:
                default:
                    charInfos = CharSegment_V3::SplitePlateByOriginal (plateInfo.OriginalMat,
                        plateInfo.OriginalMat, plateColor);
                    break;
            }
            for (int index = charInfos.size() - 1; index >= 0; index--) {
                CharInfo &charInfo = charInfos[index];
                PlateChar_t plateChar = PlateChar_SVM::Test (charInfo.OriginalMat);
                // DebugVisualize("charinfo", charInfo.OriginalMat);
                if (plateChar == PlateChar_t::NonChar) {
                    charInfos.erase (index + charInfos.begin());
                }
                charInfo.PlateChar = plateChar;
            }
            result.CharInfos = charInfos;
            CheckLeftAndRightToRemove (result);
            CheckPlateColor (result);
            return result;
        }
        private:
        static void CheckLeftAndRightToRemove (PlateInfo &plateInfo) {
            if (plateInfo.PlateCategory == PlateCategory_t::NonPlate) return;
            if (plateInfo.CharInfos.empty()) return;
            if (plateInfo.CharInfos.size() < 4) return;
            int charCount = plateInfo.CharInfos.size();

            //两头的先除开，去掉中间的汉字
            for (int index = charCount - 1 - 1; index > 0 + 1; index--) {
                const CharInfo &charInfo = plateInfo.CharInfos[index];
                int charInfoValue = (int) charInfo.PlateChar;

                if (charInfoValue >= (int) PlateChar_t::BeiJing && charInfoValue <= (int) PlateChar_t::JingChe) {
                    plateInfo.CharInfos.erase (plateInfo.CharInfos.begin() + index);
                }
            }
            charCount = plateInfo.CharInfos.size();

            const CharInfo &second = plateInfo.CharInfos[1];
            int secondValue = (int) second.PlateChar;

            const CharInfo &lastSecond = plateInfo.CharInfos[charCount - 2];

            int lastSecondValue = (int) lastSecond.PlateChar;
            switch (plateInfo.PlateCategory) {
                case PlateCategory_t::NormalPlate:
                case PlateCategory_t::NormalPlate2Row:
                case PlateCategory_t::MacaoPlateInternal:
                case PlateCategory_t::HongkongPlateInternal:
                case PlateCategory_t::PolicePlate:
                    if (secondValue >= (int) PlateChar_t::BeiJing && secondValue <= (int) PlateChar_t::JingChe) {
                        plateInfo.CharInfos.erase (plateInfo.CharInfos.begin()); //如果第⼆个是汉字，那就去掉第⼀个字符
                    }
                    charCount = plateInfo.CharInfos.size();

                    if (charCount > 7) plateInfo.CharInfos.erase (plateInfo.CharInfos.begin() + charCount - 1);
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
            }
            charCount = plateInfo.CharInfos.size();
            if (charCount < 7) return;
            const CharInfo &first = plateInfo.CharInfos[0];
            int firstValue = (int) first.PlateChar;
            const CharInfo &second2 = plateInfo.CharInfos[1];
            secondValue = (int) second2.PlateChar;
            const CharInfo &lastFirst = plateInfo.CharInfos[charCount - 1];
            int lastFirstValue = (int) lastFirst.PlateChar;
            switch (plateInfo.PlateCategory) {
                case PlateCategory_t::NormalPlate:
                    if (lastFirstValue >= (int) PlateChar_t::BeiJing && lastFirstValue <= (int) PlateChar_t::JingChe) {
                        plateInfo.CharInfos.erase (plateInfo.CharInfos.begin() + charCount - 1); //去掉最后⼀位汉字
                    }
                    if (firstValue <= (int) PlateChar_t::Point) {
                        plateInfo.CharInfos.erase (plateInfo.CharInfos.begin()); //如果第⼀位为⾮汉字，删除
                    }
                    if (secondValue >= (int) PlateChar_t::_0 && secondValue <= (int) PlateChar_t::_9) {
                        plateInfo.CharInfos.erase (plateInfo.CharInfos.begin()+1); //如果第⼆位为数字，删除
                    }
                    break;
            }
        }
        private:
        static void CheckPlateColor (PlateInfo &plateInfo) {
            if (plateInfo.PlateCategory == PlateCategory_t::NonPlate) return;
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
            }
        }

        private:
        class PlateInfoComparer_DESC{
            public:
            bool operator()(const PlateInfo &x, const PlateInfo &y){
                return y.CharInfos.size() < x.CharInfos.size();
            }
            // int Compare (PlateInfo x, PlateInfo y) {
            //     return y.CharInfos.Count.CompareTo (x.CharInfos.Count);
            // }
        };

    };
}
} // namespace CV
} // namespace Doit
#endif // !PLATE_RECOGNITION_H
