﻿#ifndef CHAR_INFO_H
#define CHAR_INFO_H

#include <opencv2/core.hpp>
using cv::Mat;
using cv::Rect;
using RotatedRect_t = cv::RotatedRect;

#include <ios>
using std::basic_ostream;
using std::ostringstream;
#include <vector>
using std::vector;
#include <iomanip>
using std::ios;
#include <string>
using std::string;

/*--------  CharInfo.h  --------*/
namespace Doit {
namespace CV {
namespace PlateRecogn {

#define EnumOstreamOverload(EnumType)                                          \
    template <typename char_t, class Traits>                                   \
    basic_ostream<char_t, Traits> &operator<<(                                 \
        basic_ostream<char_t, Traits> &stream, EnumType enumValue) {           \
        return stream << EnumType##ToString[static_cast<size_t>(enumValue)];   \
    }

/**
 * -----------------------  PlateCategory  -----------------------
 */
enum class PlateCategory_t {
    NonPlate = 0,
    NormalPlate,
    NormalPlate2Row,
    PlateInternal,
    MacaoPlateInternal,
    MacaoPlate,
    MacaoPlate2Row,
    HongkongPlateInternal,
    HongkongPlate,
    HongkomgPlate2Row,
    PolicePlate,
    MilitaryPlate,
    MilitrayPlate2Row,
    AlternativeEnergyPlate
};
// HACK
// convert enum to string
constexpr const char *PlateCategory_tToString[] = {
    "非车牌",   "普通车牌",      "普通车牌_两行", "内集车牌", "澳门车牌_内",
    "澳门车牌", "澳门车牌_两行", "香港车牌_内",   "香港车牌", "香港车牌_两行",
    "警牌",     "军牌",          "军牌_两行",     "新能源"};
EnumOstreamOverload(PlateCategory_t);

/**
 * -----------------------  PlateColor  -----------------------
 */
enum class PlateColor_t {
    BluePlate = 0,
    YellowPlate,
    WhitePlate,
    BlackPlate = 4,
    GreenPlate,
    UnknownPlate
};
// HACK
// convert enum to string
constexpr const char *PlateColor_tToString[] = {
    "BluePlate", "YellowPlate", "WhitePlate",
    // Place Holder
    "PlaceHolder", "BlackPlate", "GreenPlate", "UnknownPlate"};
EnumOstreamOverload(PlateColor_t);

/**
 * -----------------------  PlateChar  -----------------------
 */
enum class PlateChar_t {
    NonChar = 0,
    // 穗
    GuangZhou,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    //点 = 38,
    Point,
    // 京 = 39,
    BeiJing,
    // 津 = 40,
    TianJin,
    // 沪 = 41,
    ShangHai,
    // 渝 = 42,
    ChongQing,
    // 蒙 = 43,
    NeiMengGu,
    // 新 = 44,
    XinJiang,
    // 藏 = 45,
    XiZang,
    // 宁 = 46,
    NingXia,
    // 桂 = 47,
    GuiLin,
    // 港 = 48,
    XiangGang,
    // 澳 = 49,
    AoMen,
    // ⿊ = 50,
    HeiLongJiang,
    // 吉 = 51,
    JiLin,
    // 辽 = 52,
    LiaoNing,
    // 晋 = 53,
    ShanXi,
    // 冀 = 54,
    HeBei,
    // ⻘ = 55,
    QingHai,
    // 鲁 = 56,
    ShanDong,
    // 豫 = 57,
    HeNan,
    // 苏 = 58,
    JiangSu,
    // 皖 = 59,
    AnHui,
    // 浙 = 60,
    ZheJiang,
    // 闽 = 61,
    FuJian,
    // 赣 = 62,
    JiangXi,
    // 湘 = 63,
    HuNan,
    // 鄂 = 64,
    HuBei,
    // 粤 = 65,
    GuangDong,
    // 琼 = 66,
    HaiNan,
    // ⽢ = 67,
    GanSu,
    // 陕 = 68,
    ShaanXi,
    // 贵 = 69,
    GuiZhou,
    // 云 = 70,
    YunNan,
    // 川 = 71,
    SiChuan,
    // 警 = 72
    JingChe
};
// HACK
// convert enum to string
constexpr const char *PlateChar_tToString[] = {
    "非字符", "穗", "A",  "B",  "C",  "D",  "E",  "F",  "G",  "H",  "I",
    "J",      "K",  "L",  "M",  "N",  "O",  "P",  "Q",  "R",  "S",  "T",
    "U",      "V",  "W",  "X",  "Y",  "Z",  "_0", "_1", "_2", "_3", "_4",
    "_5",     "_6", "_7", "_8", "_9", "点", "京", "津", "沪", "渝", "蒙",
    "新",     "藏", "宁", "桂", "港", "澳", "黑", "吉", "辽", "晋", "冀",
    "青",     "鲁", "豫", "苏", "皖", "浙", "闽", "赣", "湘", "鄂", "粤",
    "琼",     "甘", "陕", "贵", "云", "川", "警"};
EnumOstreamOverload(PlateChar_t);

/**
 * -----------------------  PlateLocateMethod  -----------------------
 */
enum class PlateLocateMethod_t {
    Unknown = 0,
    ColorBlueBlack,
    ColorYellowWhite,
    Color,
    Sobel,
    MSER
};
// HACK
// convert enum to string
constexpr const char *PlateLocateMethod_tToString[] = {
    "Unknown", "ColorBlueBlack", "ColorYellowWhite", "Color", "Sobel", "MSER"};
EnumOstreamOverload(PlateLocateMethod_t);

/**
 * -----------------------  CharSplitMethod  -----------------------
 */
enum class CharSplitMethod_t { Unknown = 0, Origin, Gamma, Exponential, Log };
// HACK
// convert enum to string
constexpr const char *CharSplitMethod_tToString[] = {
    "Unknown", "Origin", "Gamma", "Exponential", "Log"};

EnumOstreamOverload(CharSplitMethod_t);

/**
 * -----------------------  CharImage  -----------------------
 */
struct CharImage {
    string FileName;
    string Name;
    PlateChar_t PlateChar;
    cv::Size MatSize;
    CharImage(const string &fileName, const string &name, PlateChar_t plateChar,
              const cv::Size &matSize)
        : FileName(fileName), Name(name), PlateChar(plateChar),
          MatSize(matSize) {}
};

struct PlateImage {
    string FileName;
    string Name;
    PlateCategory_t PlateCategory;
    cv::Size MatSize;
    PlateImage(const string &fileName, const string &name,
               PlateCategory_t plateCategory, const cv::Size &matSize)
        : FileName(fileName), Name(name), PlateCategory(plateCategory),
          MatSize(matSize) {}
};

class CharInfo {
  public:
    PlateChar_t PlateChar;
    Mat OriginalMat;
    Rect OriginalRect;
    PlateLocateMethod_t PlateLocateMethod;
    CharSplitMethod_t CharSplitMethod;

    CharInfo() {}
    CharInfo(PlateChar_t plateChar, const Mat &originalMat,
             const Rect &originalRect, PlateLocateMethod_t plateLocateMethod,
             CharSplitMethod_t charSplitMethod)
        : PlateChar(plateChar), OriginalMat(originalMat),
          OriginalRect(originalRect), PlateLocateMethod(plateLocateMethod),
          CharSplitMethod(charSplitMethod) {}

    string Info() {
        ostringstream buffer;
        buffer << "字符:" << PlateChar << " \r\n宽:" << OriginalRect.width
               << " \r\n高:" << OriginalRect.height
               << " \r\n宽高比:" << std::setiosflags(ios::fixed)
               << std::setprecision(2)
               << float(OriginalRect.width) / OriginalRect.height
               << " \r\n左:" << OriginalRect.x
               << " \r\n右:" << OriginalRect.x + OriginalRect.width
               << " \r\n上:" << OriginalRect.y
               << " \r\n下:" << OriginalRect.y + OriginalRect.height
               << " \r\n⻋牌定位:" << PlateLocateMethod
               << " \r\n字符切分:" << CharSplitMethod << " \r\n";
        return buffer.str();
    }

    string ToString() {
        string ret = string(PlateChar_tToString[static_cast<int>(PlateChar)]);
        size_t apperance = string::npos;
        while ((apperance = ret.find("_")) != string::npos) {
            ret.erase(apperance, 1);
        }
        return ret;
    }
};

class PlateInfo {
  public:
    PlateCategory_t PlateCategory;
    Rect OriginalRect;
    // 对于空引用使用empty
    Mat OriginalMat;
    vector<CharInfo> CharInfos;
    PlateLocateMethod_t PlateLocateMethod;
    PlateColor_t PlateColor = PlateColor_t::UnknownPlate;
    RotatedRect_t RotatedRect;

    PlateInfo() {}
    PlateInfo(PlateCategory_t plateCategory, const Rect &originalRect,
              const Mat &originalMat, const vector<CharInfo> &charInfos,
              PlateLocateMethod_t plateLocateMethod)
        : PlateCategory(plateCategory), OriginalRect(originalRect),
          OriginalMat(originalMat), CharInfos(charInfos),
          PlateLocateMethod(plateLocateMethod) {}

    string Info() {
        ostringstream buffer;
        buffer << "类型:" << PlateCategory << " \r\n颜色:" << PlateColor
               << " \r\n字符:" << ToString() << " \r\n宽:" << OriginalRect.width
               << " \r\n高:" << OriginalRect.height
               << " \r\n宽高⽐:" << std::setiosflags(ios::fixed)
               << std::setprecision(2)
               << float(OriginalRect.width) / OriginalRect.height
               << " \r\n左 : " << OriginalRect.x
               << " \r\n右: " << OriginalRect.x + OriginalRect.width
               << " \r\n上: " << OriginalRect.y
               << " \r\n下: " << OriginalRect.y + OriginalRect.height
               << " \r\n车牌定位: " << PlateLocateMethod << "\r\n ";
        return buffer.str();
    }

    string ToString() {
        if (CharInfos.empty()) {
            return "";
        }
        ostringstream stringBuilder;
        for (auto charInfo : CharInfos) {
            stringBuilder << charInfo.ToString();
        }
        string result = stringBuilder.str();
        size_t pos = string::npos;
        while ((pos = result.find("非字符")) != string::npos) {
            result.erase(pos, strlen("非字符"));
        }
        return result;
    }
};

} // namespace PlateRecogn
} // namespace CV
} // namespace Doit
#endif // !CHAR_INFO_H
