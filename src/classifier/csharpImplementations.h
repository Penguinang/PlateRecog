#ifndef CSHARP_IMPLEMENTATIONS
#define CSHARP_IMPLEMENTATIONS

#include <random>
#include <string>
using std::basic_ostream;
using std::string;
#include <time.h>
using std::vector;

#if __cplusplus >= 201703L
#include <filesystem>
using std::filesystem::create_directory;
using std::filesystem::directory_iterator;
using std::filesystem::exists;
using std::filesystem::filesystem_error;
#else
//#error("至少c++17")
#include <Windows.h>

#endif

/*--------  c# class implementations for cpp  --------*/
class Random {
    std::default_random_engine e;

  public:
    Random() {}
    int Next(int max) {
        std::uniform_int_distribution<int> generator(0, max);
        return generator(e);
    }
};

class DateTime {
  public:
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    static DateTime Now() {
        time_t n = time(NULL);
        tm *now = localtime(&n);
        return {now->tm_year, now->tm_mon, now->tm_mday,
                now->tm_hour, now->tm_min, now->tm_sec};
    }
};

class Directory {
  public:
#if __cplusplus >= 201703L
    static bool Exists(const string &path) { return exists(path); }
    static void CreateDirectory(const string &path) { create_directory(path); }
    static vector<string> GetFiles(const string &path) {
        vector<string> ret = {};
        for (auto &p : directory_iterator(path)) {
            ret.push_back(p.path());
        }
        return ret;
    }
#else
    //#error("至少c++17")
    static bool Exists(const string &path) {
        WIN32_FIND_DATAA FindFileData;
        HANDLE hFind;
        hFind = FindFirstFileA(path.c_str(), &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
            return TRUE;
        }
        FindClose(hFind);
        return FALSE;
    }
    static void CreateDirectory(const string &path) { ::CreateDirectory(path.c_str(),0); }
    static vector<string> GetFiles(const string &path) {
        HANDLE hFind;
        WIN32_FIND_DATAA findData;
        vector<string> ret;
        hFind = FindFirstFileA((path + "\\*").c_str(), &findData);
        if (hFind == INVALID_HANDLE_VALUE) {
            return {};
        }
        do {
            if (strcmp(findData.cFileName, ".") == 0 ||
                strcmp(findData.cFileName, "..") == 0)
                continue;
            ret.push_back(path + "\\" + findData.cFileName);
        } while (FindNextFileA(hFind, &findData));
        return ret;
    }

#endif
};

#ifdef __linux__
#define DIRECTORY_DELIMITER "/"
#else
#ifdef _WIN32
#define DIRECTORY_DELIMITER "\\"
#else error("无法识别的系统，自己设置文件分隔符")
#endif
#endif

constexpr std::nullptr_t null = nullptr;

#endif // !CSHARP_IMPLEMENTATIONS
