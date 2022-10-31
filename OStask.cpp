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

#include "OStask.h"

namespace ost {
    // 系统架构表
    const std::unordered_map<WORD, std::string> archList = {{9,      "x64"},
                                                            {5,      "ARM"},
                                                            {12,     "ARM64"},
                                                            {6,      "Intel Itanium"},
                                                            {0,      "x86"},
                                                            {0xffff, "Unknown"}};
    // 内存块状态表
    const std::unordered_map<DWORD, std::string> mbiStateMap = {{MEM_COMMIT,  "Committed"},
                                                                {MEM_FREE,    "Free"},
                                                                {MEM_RESERVE, "Reserved"}};
    // 内存块保护表
    const std::unordered_map<DWORD, std::string> mbiProtectMap = {{PAGE_EXECUTE,           "Exec"},
                                                                  {PAGE_EXECUTE_READ,      "Read/Exec"},
                                                                  {PAGE_EXECUTE_READWRITE, "Read/Write/Exec"},
                                                                  {PAGE_EXECUTE_WRITECOPY, "Read/Copy/Exec"},
                                                                  {PAGE_NOACCESS,          "No Access"},
                                                                  {PAGE_READONLY,          "Read"},
                                                                  {PAGE_READWRITE,         "Read/Write"},
                                                                  {PAGE_WRITECOPY,         "Read/Copy"},
                                                                  {PAGE_TARGETS_INVALID,   "Invalid"},
                                                                  {PAGE_GUARD,             "Guard"},
                                                                  {PAGE_NOCACHE,           "No Cache"},
                                                                  {PAGE_WRITECOMBINE,      "Write Combine"},};
    // 内存块类型表
    const std::unordered_map<DWORD, std::string> mbiTypeMap = {{MEM_IMAGE,   "Image"},
                                                               {MEM_MAPPED,  "Mapped"},
                                                               {MEM_PRIVATE, "Private"}};
}

// 显示全局内存信息
void ost::showTotal() {
    printf("[TOTAL]:\n");
    MEMORYSTATUSEX lpBuffer;
    lpBuffer.dwLength = sizeof(MEMORYSTATUSEX);

    // 获取系统内存信息
    if (GlobalMemoryStatusEx(&lpBuffer) == 0) {
        ost::printError("Get Memory Status");
    }
    // 显示内存使用百分比
    printf("Percent of memory in use: %ld%%.\n", lpBuffer.dwMemoryLoad);
    printf("\nPhysical memory usage:\n    Available / Total: ");

    if (ost::divByte.first) {
        // 使用输入的单位显示
        printf("%*I64d / %*I64d %cB.\n",
               ost::NUM_WIDTH, lpBuffer.ullAvailPhys / ost::divByte.second,
               ost::NUM_WIDTH, lpBuffer.ullTotalPhys / ost::divByte.second, ost::divByte.first);
    } else {
        // 使用默认单位显示
        WCHAR szAvaSize[MAX_PATH];
        WCHAR szTotSize[MAX_PATH];
        ost::btoStrDL(lpBuffer.ullAvailPhys, szAvaSize);
        ost::btoStrDL(lpBuffer.ullTotalPhys, szTotSize);
        printf("%*ls / %*ls.\n", ost::NUM_WIDTH, szAvaSize, ost::NUM_WIDTH, szTotSize);
    }
    putchar('\n');
}

// 显示系统信息
void ost::showSys() {
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));

    GetSystemInfo(&si);

    // 获取系统架构
    auto &&sysType = ost::archList.count(si.wProcessorArchitecture) ?
                     ost::archList.at(si.wProcessorArchitecture) : "Unable";

    printf("[SYSTEM]:\n");
    printf("Process architecture: %s.\n", sysType.c_str());

    // 逻辑处理器数量
    printf("Number of logical processors: %ld.\nPage size: ", si.dwNumberOfProcessors);

    // 页面大小
    WCHAR szPageSize[MAX_PATH];
    ost::btoStrDL(si.dwPageSize, szPageSize);
    printf("%ls.\n", szPageSize);

    // 可访问内存地址范围
    printf("Accessible memory address range: 0x%p - 0x%p.\n",
           si.lpMinimumApplicationAddress,
           si.lpMaximumApplicationAddress);
    putchar('\n');
}

// 显示性能信息
void ost::showPerformance() {
    PERFORMANCE_INFORMATION pi;
    pi.cb = sizeof(PERFORMANCE_INFORMATION);
    GetPerformanceInfo(&pi, pi.cb);

    // 页面大小
    printf("[PERFORMANCE]:\nPage size: ");
    WCHAR szPageSize[MAX_PATH];
    ost::btoStrDL(pi.PageSize, szPageSize);
    printf("%ls.\n", szPageSize);

    // 已提交/总体页面数
    printf("Currently committed pages amount:\n");
    printf("\tCurrent / Max: %llu / %llu.\n", pi.CommitTotal, pi.CommitLimit);

    // 最大历史页面数
    printf("Max committed pages amount in history: %llu\n\n", pi.CommitPeak);

    // 可用/总体物理页面数
    printf("Currently physical pages amount:\n");
    printf("\tAvailable / Max: %llu / %llu.\n", pi.PhysicalAvailable, pi.PhysicalTotal);

    // 系统缓存页面数
    printf("System cache pages amount: %llu.\n\n", pi.SystemCache);

    // 分页/未分页/全部内存页数
    printf("Currently kernel pools:\n");
    printf("\tPaged/Nonpaged: %llu / %llu.\n    All: %llu.\n\n",
           pi.KernelPaged, pi.KernelNonpaged, pi.KernelTotal);

    printf("Currently program amount:\n");
    // 当前句柄数
    printf("\tOpened handles: %lu\n", pi.HandleCount);
    // 当前进程数
    printf("\tProcesses: %lu\n", pi.ProcessCount);
    // 当前线程数
    printf("\tThreads: %lu\n", pi.ThreadCount);

    putchar('\n');
}

// 显示所有进程信息
void ost::showEachProcess() {

    using namespace std;
    //获得所有进程的快照
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pointOfSnap;
    pointOfSnap.dwSize = sizeof(PROCESSENTRY32);
    printf("[PROCESS LIST]:\n");

    // 显示每个进程的 ID，名字，工作集大小，分页池大小
    printf("%-*s", ost::PID_SIZE, "ID");
    printf("\t%-*s", ost::PNAME_SIZE, "Name");
    printf("\t%-*s", ost::PWORKSET_SIZE, "WorkSet");
    printf("\t%-*s", ost::PWORKSET_SIZE, "PagePool");
    putchar('\n');

    // 获取第一个进程快照
    BOOL snapExist = Process32First(hSnapShot, &pointOfSnap);
    while (snapExist) {
        //循环读取所有进程快照
        HANDLE pHandle = OpenProcess(
                PROCESS_ALL_ACCESS,
                FALSE,
                pointOfSnap.th32ProcessID);

        PROCESS_MEMORY_COUNTERS pMemCount;
        ZeroMemory(&pMemCount, sizeof(PROCESS_MEMORY_COUNTERS));

        if (GetProcessMemoryInfo(pHandle, &pMemCount, sizeof(PROCESS_MEMORY_COUNTERS)) == TRUE) {
            // ID
            printf("%-*lu", ost::PID_SIZE, pointOfSnap.th32ProcessID);
            // 进程名
            printf("\t%-*s", ost::PNAME_SIZE, pointOfSnap.szExeFile);
            // 工作集和分页池大小
            if (ost::divByte.first) {
                printf("\t%-*llu%cB", ost::PWORKSET_SIZE, pMemCount.WorkingSetSize / ost::divByte.second,
                       ost::divByte.first);
                printf("\t%-*llu%cB", ost::PWORKSET_SIZE, pMemCount.QuotaPagedPoolUsage / ost::divByte.second,
                       ost::divByte.first);
            } else {
                WCHAR szWorkSize[MAX_PATH];
                WCHAR szQuoSize[MAX_PATH];
                ost::btoStrDL(pMemCount.WorkingSetSize, szWorkSize);
                ost::btoStrDL(pMemCount.QuotaPagedPoolUsage, szQuoSize);
                printf("\t%-*ls", ost::PWORKSET_SIZE, szWorkSize);
                printf("\t%-*ls", ost::PWORKSET_SIZE, szQuoSize);
            }
            putchar('\n');
        }
        // 获取下一个进程快照
        snapExist = Process32Next(hSnapShot, &pointOfSnap);
    }

}

// 显示硬件信息
void ost::showHardwareInfo() {
    printf("[HARDWARE INFO]:\n");

    // CPU
    printf("[CPU]:\n");
    // 读取CPU信息
    int cpuInfo[4] = {-1};
    unsigned nExIds, i;
    char CPUBrandString[0x40];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));
    __cpuid(cpuInfo, 0);
    memcpy(CPUBrandString, cpuInfo + 1, sizeof(int));
    memcpy(CPUBrandString + 4, cpuInfo + 3, sizeof(int));
    memcpy(CPUBrandString + 8, cpuInfo + 2, sizeof(int));
    // CPU供应商
    printf("\tCPU Vendor: %s\n", CPUBrandString);

    __cpuid(cpuInfo, 0x80000000);
    nExIds = cpuInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i) {
        __cpuid(cpuInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(CPUBrandString, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, cpuInfo, sizeof(cpuInfo));
    }
    // CPU类型名
    printf("\tCPU Type: %s\n", CPUBrandString);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    // CPU逻辑处理器数量
    printf("\tNumber of logical processors: %ld.\n\n", sysInfo.dwNumberOfProcessors);

    // GPU
    printf("[GPU]:\n");
    for (int j = 0;; j++) {
        // 循环显示GPU信息
        DISPLAY_DEVICE dd = {sizeof(dd), 0};
        BOOL f = EnumDisplayDevices(nullptr, j, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
        if (!f)
            break;
        // GPU名
        printf("\t%s\n", dd.DeviceName);
        // GPU类型
        printf("\t%s\n", dd.DeviceString);
    }
    putchar('\n');
}

// 显示单个进程信息
void ost::processInfo(DWORD pid) {
    printf("[PROCESS INFO]: id: %lu\n", pid);
    printf("[FORMAT]:Region Address(Length) | Status | Protect | Type | Model\n");
    // 打开进程快照
    HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hp == nullptr) {
        ost::printError("Open process");
        return;
    }

    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);
    // 进程虚拟内存空间
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));

    // 起始内存地址
    auto accAdd = si.lpMinimumApplicationAddress;

    // 结束内存地址
    auto maxAdd = si.lpMaximumApplicationAddress;
    while (accAdd < maxAdd) {
        if (VirtualQueryEx(hp, accAdd,
                           &mbi,
                           sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
            ost::printError("Get Virtual Memory");
            break;
        }
        LPVOID endAdd = reinterpret_cast<PBYTE>(accAdd) + mbi.RegionSize;
        // 显示块的起止地址和长度
        printf("%0*llX - %0*llX", ost::ADD_LEN, reinterpret_cast<ULONG_PTR>(accAdd),
               ost::ADD_LEN, reinterpret_cast<ULONG_PTR>(endAdd));
        if (ost::divByte.first) {
            printf("(%llu%cB)\t", mbi.RegionSize / ost::divByte.second, ost::divByte.first);
        } else {
            WCHAR szRegSize[MAX_PATH] = {0};
            ost::btoStrDL(mbi.RegionSize, szRegSize);
            printf("(%ls)\t", szRegSize);
        }
        // 显示块状态
        printf("%-*s\t", ost::SHORT_STR_LEN,
               ost::mbiStateMap.count(mbi.State) ?
               ost::mbiStateMap.at(mbi.State).c_str() :
               "Unknown");
        if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
            mbi.Protect = PAGE_READONLY;
        }
        // 显示块保护类型
        printf("%-*s ", ost::LONG_STR_LEN,
               ost::mbiProtectMap.count(mbi.Protect) ?
               ost::mbiProtectMap.at(mbi.Protect).c_str() :
               "Unknown");
        // 显示块类型
        printf("%-*s", ost::SHORT_STR_LEN,
               ost::mbiTypeMap.count(mbi.Type) ?
               ost::mbiTypeMap.at(mbi.Type).c_str() :
               "Unknown");
        TCHAR szFilename[MAX_PATH];
        // 当前进程已加载模块
        if (GetModuleFileName(
                reinterpret_cast<HMODULE>(accAdd),
                szFilename,
                MAX_PATH) > 0) {
            // 除去前缀路径
            PathStripPath(szFilename);
            printf("\tModule: %s", szFilename);
        }
        putchar('\n');
        accAdd = endAdd;

    }
}
