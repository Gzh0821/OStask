//
// Created by Gaozih on 2022/10/19.
//
#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <map>
#include <utility>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <exception>

#include <cstring>
#include <cstdlib>
#include <ctime>

#include <windows.h>
#include <tchar.h>
#include <Dbghelp.h>
#include <shlwapi.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <atlstr.h>


#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")

namespace ost {
    constexpr int NUM_WIDTH = 7;
    constexpr int DIV = 1024;
    constexpr int SH_STR_LEN = 48;
    constexpr int FLUSH_TIME = 1000;
    constexpr int MIN_TIME = 1;
    constexpr int MAX_TIME = 65535;
    constexpr int PID_SIZE = 8;
    constexpr int PNAME_SIZE = 40;
    constexpr int PWORKSET_SIZE = 10;
    constexpr unsigned int PID_MIN = 0x4;
    constexpr unsigned int PID_MAX = 0xFFFFFFFC;
    constexpr unsigned int FLAG_FBS = 1;
    constexpr unsigned int DEFAULT_BTYPE = 1;
    constexpr char BTYPE_NAME[4] = {' ','K','M','G'};
    struct ArguFunc {
        char shortName;
        std::string desc;
        std::function<void()> func;

        ArguFunc(char shortName, std::string desc, std::function<void()> func) : shortName(shortName),
                                                                                 desc(std::move(desc)),
                                                                                 func(std::move(func)) {};
    };

    extern std::pair<char, unsigned long> divByte;
    extern const std::unordered_map<WORD, std::string> ARCH_LIST;

    inline void pLine() {
        printf("\n");
    }

    inline void btoStrDL(DWORDLONG src,PWSTR dst){
        StrFormatByteSizeEx(src,FLAG_FBS,dst,MAX_PATH);
    }

    void showTotal();

    void showSys();

    void showPerformance();

    void showEachProcess();

    void showHardwareInfo();

    void processInfo(DWORD pid);

    void printError(const std::string &msg);


}




