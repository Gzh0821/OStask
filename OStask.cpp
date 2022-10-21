#include "OStask.h"

namespace ost {
    const std::unordered_map<DWORD, std::string> mbiStateMap = {{MEM_COMMIT,  "Committed"},
                                                                {MEM_FREE,    "Free"},
                                                                {MEM_RESERVE, "Reserved"}};
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
    const std::unordered_map<DWORD, std::string> mbiTypeMap = {{MEM_IMAGE, "Image"},
                                                               {MEM_MAPPED, "Mapped"},
                                                               {MEM_PRIVATE, "Private"}};
}


void ost::showTotal() {
    printf("[TOTAL]:\n");
    MEMORYSTATUSEX lpBuffer;
    lpBuffer.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&lpBuffer) == 0) {
        ost::printError("Get Memory Status");
    }
    printf("Percent of memory in use: %ld%%.\n", lpBuffer.dwMemoryLoad);
    printf("\nPhysical memory usage:\n    Available / Total: ");
    if (ost::divByte.first) {
        printf("%*I64d / %*I64d %cB.\n",
               ost::NUM_WIDTH, lpBuffer.ullAvailPhys / ost::divByte.second,
               ost::NUM_WIDTH, lpBuffer.ullTotalPhys / ost::divByte.second, ost::divByte.first);
    } else {
        WCHAR szAvaSize[MAX_PATH];
        WCHAR szTotSize[MAX_PATH];
        ost::btoStrDL(lpBuffer.ullAvailPhys, szAvaSize);
        ost::btoStrDL(lpBuffer.ullTotalPhys, szTotSize);
        printf("%*ls / %*ls.\n", ost::NUM_WIDTH, szAvaSize, ost::NUM_WIDTH, szTotSize);
    }
    putchar('\n');
}

void ost::showSys() {
    SYSTEM_INFO si;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);
    auto &&sysType = ost::ARCH_LIST.at(si.wProcessorArchitecture);
    printf("[SYSTEM]:\n");
    printf("Process architecture: %s.\n", sysType.c_str());
    printf("Number of logical processors: %ld.\nPage size: ", si.dwNumberOfProcessors);
    WCHAR szPageSize[MAX_PATH];
    ost::btoStrDL(si.dwPageSize, szPageSize);
    printf("%ls.\n", szPageSize);

    printf("Accessible memory address range: 0x%p - 0x%p\n",
           si.lpMinimumApplicationAddress,
           si.lpMaximumApplicationAddress);
    putchar('\n');
}

void ost::showPerformance() {
    PERFORMANCE_INFORMATION pi;
    pi.cb = sizeof(PERFORMANCE_INFORMATION);
    GetPerformanceInfo(&pi, pi.cb);
    printf("[PERFORMANCE]:\nPage size: ");

    WCHAR szPageSize[MAX_PATH];
    ost::btoStrDL(pi.PageSize, szPageSize);
    printf("%ls.\n", szPageSize);

    printf("Currently committed pages amount:\n");
    printf("\tCurrent / Max: %llu / %llu.\n", pi.CommitTotal, pi.CommitLimit);

    printf("Max committed pages amount in history: %llu\n\n", pi.CommitPeak);

    printf("Currently physical pages amount:\n");
    printf("\tAvailable / Max: %llu / %llu.\n", pi.PhysicalAvailable, pi.PhysicalTotal);

    printf("System cache pages amount: %llu.\n\n", pi.SystemCache);

    printf("Currently kernel pools:\n");
    printf("\tPaged/Nonpaged: %llu / %llu.\n    All: %llu.\n\n",
           pi.KernelPaged, pi.KernelNonpaged, pi.KernelTotal);

    printf("Currently program amount:\n");
    printf("\tOpened handles: %lu\n", pi.HandleCount);
    printf("\tProcesses: %lu\n", pi.ProcessCount);
    printf("\tThreads: %lu\n", pi.ThreadCount);

    putchar('\n');
}

void ost::showEachProcess() {

    using namespace std;
    //获得所有进程的快照
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pointOfSnap;
    pointOfSnap.dwSize = sizeof(PROCESSENTRY32);
    printf("[PROCESS LIST]:\n");
    printf("%-*s", ost::PID_SIZE, "ID");
    printf("\t%-*s", ost::PNAME_SIZE, "Name");
    printf("\t%-*s", ost::PWORKSET_SIZE, "WorkSet");
    printf("\t\t%-*s", ost::PWORKSET_SIZE, "PagePool");
    putchar('\n');

    //循环获取进程信息
    BOOL snapExist = Process32First(hSnapShot, &pointOfSnap);
    while (snapExist)//遍历所有进程
    {
        HANDLE pHandle = OpenProcess(
                PROCESS_ALL_ACCESS,
                FALSE,
                pointOfSnap.th32ProcessID);

        PROCESS_MEMORY_COUNTERS pMemCount;
        ZeroMemory(&pMemCount, sizeof(PROCESS_MEMORY_COUNTERS));

        if (GetProcessMemoryInfo(pHandle, &pMemCount, sizeof(PROCESS_MEMORY_COUNTERS)) == TRUE) {
            printf("%-*lu", ost::PID_SIZE, pointOfSnap.th32ProcessID);
            printf("\t%-*s", ost::PNAME_SIZE, pointOfSnap.szExeFile);
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
        snapExist = Process32Next(hSnapShot, &pointOfSnap);
    }

}

void ost::showHardwareInfo() {
    printf("[HARDWARE INFO]:\n");
    printf("\t[CPU]:\n");
    int cpuInfo[4] = {-1};
    unsigned nExIds, i;
    char CPUBrandString[0x40];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));
    __cpuid(cpuInfo, 0);
    memcpy(CPUBrandString, cpuInfo + 1, sizeof(int));
    memcpy(CPUBrandString + 4, cpuInfo + 3, sizeof(int));
    memcpy(CPUBrandString + 8, cpuInfo + 2, sizeof(int));
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
    printf("\tCPU Type: %s\n", CPUBrandString);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("\tNumber of logical processors: %ld.\n\n", sysInfo.dwNumberOfProcessors);
    printf("\t[GPU]:\n");
    for (int j = 0;; j++) {
        DISPLAY_DEVICE dd = {sizeof(dd), 0};
        BOOL f = EnumDisplayDevices(nullptr, j, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
        if (!f)
            break;
        printf("\t%s\n", dd.DeviceName);
        printf("\t%s\n", dd.DeviceString);
    }
    putchar('\n');
}

void ost::processInfo(DWORD pid) {
    printf("[PROCESS INFO]: id: %lu\n", pid);
    HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hp == nullptr) {
        ost::printError("Open process");
    }
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);

    MEMORY_BASIC_INFORMATION mbi;  // 进程虚拟内存空间
    ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));

    auto accAdd = si.lpMinimumApplicationAddress; // 起始内存地址
    auto maxAdd = si.lpMaximumApplicationAddress; // 内存地址边界
    while (accAdd < maxAdd) {
        if (VirtualQueryEx(hp, accAdd,
                           &mbi, sizeof(MEMORY_BASIC_INFORMATION))) {
            LPVOID endAdd = reinterpret_cast<PBYTE>(accAdd) + mbi.RegionSize;
            printf("%0*llX - %0*llX", ost::ADD_LEN, reinterpret_cast<ULONG_PTR>(accAdd),
                   ost::ADD_LEN, reinterpret_cast<ULONG_PTR>(endAdd));
            if (ost::divByte.first) {
                printf("(%llu%cB)\t", mbi.RegionSize / ost::divByte.second, ost::divByte.first);
            } else {
                WCHAR szRegSize[MAX_PATH] = {0};
                ost::btoStrDL(mbi.RegionSize, szRegSize);
                printf("(%ls)\t", szRegSize);
            }
            printf("%-*s\t", ost::SHORT_STR_LEN,
                   ost::mbiStateMap.count(mbi.State) ?
                   ost::mbiStateMap.at(mbi.State).c_str() :
                   "Unknown");
            if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
                mbi.Protect = PAGE_READONLY;
            }
            printf("%-*s ", ost::LONG_STR_LEN,
                   ost::mbiProtectMap.count(mbi.Protect) ?
                   ost::mbiProtectMap.at(mbi.Protect).c_str() :
                   "Unknown");
            printf("%-*s", ost::SHORT_STR_LEN,
                   ost::mbiTypeMap.count(mbi.Type) ?
                   ost::mbiTypeMap.at(mbi.Type).c_str() :
                   "Unknown");
            TCHAR szFilename[MAX_PATH];
            //获取当前进程已加载模块的文件的路径
            if (GetModuleFileName(
                    reinterpret_cast<HMODULE>(accAdd),            //实际虚拟内存的模块句柄
                    szFilename,                    //完全指定的文件名称
                    MAX_PATH) > 0)                //实际使用的缓冲区长度
            {
                //除去路径并显示
                PathStripPath(szFilename);
                printf("\tModule: %s", szFilename);
            }
            putchar('\n');
            accAdd = endAdd;
        }
    }
}

void ost::printError(const std::string &msg) {
    DWORD eNum;
    TCHAR sysMsg[256];
    TCHAR *p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr, eNum,
                  0, sysMsg, 256, nullptr);

    printf("[ERROR]: %s failed with error no: %lu.\n%s\n", msg.c_str(), eNum, sysMsg);
}
