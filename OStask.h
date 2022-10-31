//    Copyright 2022 Gzh0821/Gaozih
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

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
    // 数据宽度
    constexpr int NUM_WIDTH = 7;
    // 用于字节转换
    constexpr int DIV = 1024;
    // 程序循环次数的的上下届
    constexpr int MIN_TIME = 1;
    constexpr int MAX_TIME = 65535;
    // 进程id长度限制
    constexpr int PID_SIZE = 8;
    // 进程名长度限制
    constexpr int PNAME_SIZE = 40;
    // 进程工作集长度限制
    constexpr int PWORKSET_SIZE = 10;
    // 进程id的上下界
    constexpr unsigned int PID_MIN = 0x4;
    constexpr unsigned int PID_MAX = 0xFFFFFFFC;
    // StrFormatByteSizeEx 函数标志位
    constexpr unsigned int FLAG_FBS = 1;
    // 默认单位，1，2，3，4为KB,MB,GB,TB
    constexpr unsigned int DEFAULT_BTYPE = 1;
    // 地址长度限制
    constexpr unsigned int ADD_LEN = 16;
    // 长字符串长度限制
    constexpr unsigned int LONG_STR_LEN = 16;
    // 短字符串长度限制
    constexpr unsigned int SHORT_STR_LEN = 10;
    // 单位名称
    constexpr char BTYPE_NAME[4] = {' ', 'K', 'M', 'G'};

    // 参数和函数绑定结构
    struct ArguFunc {
        char shortName;
        std::string desc;
        std::function<void()> func;

        ArguFunc(char shortName, std::string desc, std::function<void()> func) : shortName(shortName),
                                                                                 desc(std::move(desc)),
                                                                                 func(std::move(func)) {};
    };
    // 用于设置字节
    extern std::pair<char, unsigned long> divByte;

    // 将byte转换为合适的单位
    inline void btoStrDL(DWORDLONG src, PWSTR dst) {
        StrFormatByteSizeEx(src, FLAG_FBS, dst, MAX_PATH);
    }

    // 输出错误
    inline void printError(const std::string &msg) {
        DWORD eNum;
        TCHAR sysMsg[256];

        eNum = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      nullptr, eNum,
                      0, sysMsg, 256, nullptr);

        printf("[ERROR]: %s failed with error no: 0x%lx.\n%s\n", msg.c_str(), eNum, sysMsg);
    }

    void showTotal();

    void showSys();

    void showPerformance();

    void showEachProcess();

    void showHardwareInfo();

    void processInfo(DWORD pid);


}




