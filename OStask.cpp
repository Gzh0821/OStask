#include "OStask.h"


void ost::showTotal() {
    printf("[TOTAL]:\n");
    MEMORYSTATUSEX lpBuffer;
    lpBuffer.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&lpBuffer) == 0) {
        ost::printError("Get Memory Status");
    }
    printf("Percent of memory in use: %ld%%.\n", lpBuffer.dwMemoryLoad);
    printf("\nPhysical memory usage:\n");
    printf("    Available / Total: %*I64d / %*I64d %cB.\n",
           ost::NUM_WIDTH, lpBuffer.ullAvailPhys / ost::divByte.second,
           ost::NUM_WIDTH, lpBuffer.ullTotalPhys / ost::divByte.second, ost::divByte.first);
    putchar('\n');
}

void ost::showSys() {
    SYSTEM_INFO si;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);
    auto &&sysType = ost::ARCH_LIST.at(si.wProcessorArchitecture);
    printf("[SYSTEM]:\n");
    printf("Process architecture: %s.\n", sysType.c_str());
    printf("Number of logical processors: %ld.\n", si.dwNumberOfProcessors);
    printf("Page size: %ld %cB.\n", si.dwPageSize / ost::divByte.second, ost::divByte.first);
    printf("Accessible memory address range: 0x%p - 0x%p\n",
           si.lpMinimumApplicationAddress,
           si.lpMaximumApplicationAddress);
    putchar('\n');
}

void ost::showPerformance() {
    PERFORMANCE_INFORMATION pi;
    pi.cb = sizeof(PERFORMANCE_INFORMATION);
    GetPerformanceInfo(&pi, pi.cb);
    printf("[PERFORMANCE]:\n");

    printf("Page size: %llu %cB.\n", pi.PageSize / ost::divByte.second, ost::divByte.first);
    printf("Currently committed pages amount:\n");
    printf("    Current / Max: %llu / %llu.\n", pi.CommitTotal, pi.CommitLimit);

    printf("Max committed pages amount in history: %llu\n\n", pi.CommitPeak);

    printf("Currently physical pages amount:\n");
    printf("    Available / Max: %llu / %llu.\n", pi.PhysicalAvailable, pi.PhysicalTotal);

    printf("System cache pages amount: %llu.\n\n", pi.SystemCache);

    printf("Currently kernel pools:\n");
    printf("    Paged/Nonpaged: %llu / %llu.\n    All: %llu.\n\n",
           pi.KernelPaged, pi.KernelNonpaged, pi.KernelTotal);

    printf("Currently program amount:\n");
    printf("    Opened handles: %lu\n", pi.HandleCount);
    printf("    Processes: %lu\n", pi.ProcessCount);
    printf("    Threads: %lu\n", pi.ThreadCount);

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
    printf("    %-*s", ost::PNAME_SIZE, "Name");
    printf("    %-*s", ost::PWORKSET_SIZE, "WorkSet");
    printf("      %-*s", ost::PWORKSET_SIZE, "PagePool");
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
            printf("    %-*s", ost::PNAME_SIZE, pointOfSnap.szExeFile);
            printf("    %-*llu%cB", ost::PWORKSET_SIZE, pMemCount.WorkingSetSize / ost::divByte.second,
                   ost::divByte.first);
            printf("    %-*llu%cB", ost::PWORKSET_SIZE, pMemCount.QuotaPagedPoolUsage / ost::divByte.second,
                   ost::divByte.first);
            putchar('\n');
        }
        snapExist = Process32Next(hSnapShot, &pointOfSnap);//获取下一个进程的快照
    }

}

void ost::showHardwareInfo() {
    printf("[HARDWARE INFO]:\n");
    printf("    [CPU]:\n");
    int cpuInfo[4] = {-1};
    unsigned nExIds, i;
    char CPUBrandString[0x40];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));
    __cpuid(cpuInfo, 0);
    memcpy(CPUBrandString, cpuInfo + 1, sizeof(int));
    memcpy(CPUBrandString + 4, cpuInfo + 3, sizeof(int));
    memcpy(CPUBrandString + 8, cpuInfo + 2, sizeof(int));
    printf("    CPU Vendor: %s\n", CPUBrandString);

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
    printf("    CPU Type: %s\n", CPUBrandString);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("    Number of logical processors: %ld.\n\n", sysInfo.dwNumberOfProcessors);
    printf("    [GPU]:\n");
    for (int j = 0;; j++) {
        DISPLAY_DEVICE dd = {sizeof(dd), 0};
        BOOL f = EnumDisplayDevices(nullptr, j, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
        if (!f)
            break;
        printf("    %s\n", dd.DeviceName);
        printf("    %s\n", dd.DeviceString);
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
