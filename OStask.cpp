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
    // ϵͳ�ܹ���
    const std::unordered_map<WORD, std::string> archList = {{9,      "x64"},
                                                            {5,      "ARM"},
                                                            {12,     "ARM64"},
                                                            {6,      "Intel Itanium"},
                                                            {0,      "x86"},
                                                            {0xffff, "Unknown"}};
    // �ڴ��״̬��
    const std::unordered_map<DWORD, std::string> mbiStateMap = {{MEM_COMMIT,  "Committed"},
                                                                {MEM_FREE,    "Free"},
                                                                {MEM_RESERVE, "Reserved"}};
    // �ڴ�鱣����
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
    // �ڴ�����ͱ�
    const std::unordered_map<DWORD, std::string> mbiTypeMap = {{MEM_IMAGE,   "Image"},
                                                               {MEM_MAPPED,  "Mapped"},
                                                               {MEM_PRIVATE, "Private"}};
}

// ��ʾȫ���ڴ���Ϣ
void ost::showTotal() {
    printf("[TOTAL]:\n");
    MEMORYSTATUSEX lpBuffer;
    lpBuffer.dwLength = sizeof(MEMORYSTATUSEX);

    // ��ȡϵͳ�ڴ���Ϣ
    if (GlobalMemoryStatusEx(&lpBuffer) == 0) {
        ost::printError("Get Memory Status");
    }
    // ��ʾ�ڴ�ʹ�ðٷֱ�
    printf("Percent of memory in use: %ld%%.\n", lpBuffer.dwMemoryLoad);
    printf("\nPhysical memory usage:\n    Available / Total: ");

    if (ost::divByte.first) {
        // ʹ������ĵ�λ��ʾ
        printf("%*I64d / %*I64d %cB.\n",
               ost::NUM_WIDTH, lpBuffer.ullAvailPhys / ost::divByte.second,
               ost::NUM_WIDTH, lpBuffer.ullTotalPhys / ost::divByte.second, ost::divByte.first);
    } else {
        // ʹ��Ĭ�ϵ�λ��ʾ
        WCHAR szAvaSize[MAX_PATH];
        WCHAR szTotSize[MAX_PATH];
        ost::btoStrDL(lpBuffer.ullAvailPhys, szAvaSize);
        ost::btoStrDL(lpBuffer.ullTotalPhys, szTotSize);
        printf("%*ls / %*ls.\n", ost::NUM_WIDTH, szAvaSize, ost::NUM_WIDTH, szTotSize);
    }
    putchar('\n');
}

// ��ʾϵͳ��Ϣ
void ost::showSys() {
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));

    GetSystemInfo(&si);

    // ��ȡϵͳ�ܹ�
    auto &&sysType = ost::archList.count(si.wProcessorArchitecture) ?
                     ost::archList.at(si.wProcessorArchitecture) : "Unable";

    printf("[SYSTEM]:\n");
    printf("Process architecture: %s.\n", sysType.c_str());

    // �߼�����������
    printf("Number of logical processors: %ld.\nPage size: ", si.dwNumberOfProcessors);

    // ҳ���С
    WCHAR szPageSize[MAX_PATH];
    ost::btoStrDL(si.dwPageSize, szPageSize);
    printf("%ls.\n", szPageSize);

    // �ɷ����ڴ��ַ��Χ
    printf("Accessible memory address range: 0x%p - 0x%p.\n",
           si.lpMinimumApplicationAddress,
           si.lpMaximumApplicationAddress);
    putchar('\n');
}

// ��ʾ������Ϣ
void ost::showPerformance() {
    PERFORMANCE_INFORMATION pi;
    pi.cb = sizeof(PERFORMANCE_INFORMATION);
    GetPerformanceInfo(&pi, pi.cb);

    // ҳ���С
    printf("[PERFORMANCE]:\nPage size: ");
    WCHAR szPageSize[MAX_PATH];
    ost::btoStrDL(pi.PageSize, szPageSize);
    printf("%ls.\n", szPageSize);

    // ���ύ/����ҳ����
    printf("Currently committed pages amount:\n");
    printf("\tCurrent / Max: %llu / %llu.\n", pi.CommitTotal, pi.CommitLimit);

    // �����ʷҳ����
    printf("Max committed pages amount in history: %llu\n\n", pi.CommitPeak);

    // ����/��������ҳ����
    printf("Currently physical pages amount:\n");
    printf("\tAvailable / Max: %llu / %llu.\n", pi.PhysicalAvailable, pi.PhysicalTotal);

    // ϵͳ����ҳ����
    printf("System cache pages amount: %llu.\n\n", pi.SystemCache);

    // ��ҳ/δ��ҳ/ȫ���ڴ�ҳ��
    printf("Currently kernel pools:\n");
    printf("\tPaged/Nonpaged: %llu / %llu.\n    All: %llu.\n\n",
           pi.KernelPaged, pi.KernelNonpaged, pi.KernelTotal);

    printf("Currently program amount:\n");
    // ��ǰ�����
    printf("\tOpened handles: %lu\n", pi.HandleCount);
    // ��ǰ������
    printf("\tProcesses: %lu\n", pi.ProcessCount);
    // ��ǰ�߳���
    printf("\tThreads: %lu\n", pi.ThreadCount);

    putchar('\n');
}

// ��ʾ���н�����Ϣ
void ost::showEachProcess() {

    using namespace std;
    //������н��̵Ŀ���
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pointOfSnap;
    pointOfSnap.dwSize = sizeof(PROCESSENTRY32);
    printf("[PROCESS LIST]:\n");

    // ��ʾÿ�����̵� ID�����֣���������С����ҳ�ش�С
    printf("%-*s", ost::PID_SIZE, "ID");
    printf("\t%-*s", ost::PNAME_SIZE, "Name");
    printf("\t%-*s", ost::PWORKSET_SIZE, "WorkSet");
    printf("\t%-*s", ost::PWORKSET_SIZE, "PagePool");
    putchar('\n');

    // ��ȡ��һ�����̿���
    BOOL snapExist = Process32First(hSnapShot, &pointOfSnap);
    while (snapExist) {
        //ѭ����ȡ���н��̿���
        HANDLE pHandle = OpenProcess(
                PROCESS_ALL_ACCESS,
                FALSE,
                pointOfSnap.th32ProcessID);

        PROCESS_MEMORY_COUNTERS pMemCount;
        ZeroMemory(&pMemCount, sizeof(PROCESS_MEMORY_COUNTERS));

        if (GetProcessMemoryInfo(pHandle, &pMemCount, sizeof(PROCESS_MEMORY_COUNTERS)) == TRUE) {
            // ID
            printf("%-*lu", ost::PID_SIZE, pointOfSnap.th32ProcessID);
            // ������
            printf("\t%-*s", ost::PNAME_SIZE, pointOfSnap.szExeFile);
            // �������ͷ�ҳ�ش�С
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
        // ��ȡ��һ�����̿���
        snapExist = Process32Next(hSnapShot, &pointOfSnap);
    }

}

// ��ʾӲ����Ϣ
void ost::showHardwareInfo() {
    printf("[HARDWARE INFO]:\n");

    // CPU
    printf("[CPU]:\n");
    // ��ȡCPU��Ϣ
    int cpuInfo[4] = {-1};
    unsigned nExIds, i;
    char CPUBrandString[0x40];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));
    __cpuid(cpuInfo, 0);
    memcpy(CPUBrandString, cpuInfo + 1, sizeof(int));
    memcpy(CPUBrandString + 4, cpuInfo + 3, sizeof(int));
    memcpy(CPUBrandString + 8, cpuInfo + 2, sizeof(int));
    // CPU��Ӧ��
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
    // CPU������
    printf("\tCPU Type: %s\n", CPUBrandString);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    // CPU�߼�����������
    printf("\tNumber of logical processors: %ld.\n\n", sysInfo.dwNumberOfProcessors);

    // GPU
    printf("[GPU]:\n");
    for (int j = 0;; j++) {
        // ѭ����ʾGPU��Ϣ
        DISPLAY_DEVICE dd = {sizeof(dd), 0};
        BOOL f = EnumDisplayDevices(nullptr, j, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
        if (!f)
            break;
        // GPU��
        printf("\t%s\n", dd.DeviceName);
        // GPU����
        printf("\t%s\n", dd.DeviceString);
    }
    putchar('\n');
}

// ��ʾ����������Ϣ
void ost::processInfo(DWORD pid) {
    printf("[PROCESS INFO]: id: %lu\n", pid);
    printf("[FORMAT]:Region Address(Length) | Status | Protect | Type | Model\n");
    // �򿪽��̿���
    HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hp == nullptr) {
        ost::printError("Open process");
        return;
    }

    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);
    // ���������ڴ�ռ�
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));

    // ��ʼ�ڴ��ַ
    auto accAdd = si.lpMinimumApplicationAddress;

    // �����ڴ��ַ
    auto maxAdd = si.lpMaximumApplicationAddress;
    while (accAdd < maxAdd) {
        if (VirtualQueryEx(hp, accAdd,
                           &mbi,
                           sizeof(MEMORY_BASIC_INFORMATION)) == 0) {
            ost::printError("Get Virtual Memory");
            break;
        }
        LPVOID endAdd = reinterpret_cast<PBYTE>(accAdd) + mbi.RegionSize;
        // ��ʾ�����ֹ��ַ�ͳ���
        printf("%0*llX - %0*llX", ost::ADD_LEN, reinterpret_cast<ULONG_PTR>(accAdd),
               ost::ADD_LEN, reinterpret_cast<ULONG_PTR>(endAdd));
        if (ost::divByte.first) {
            printf("(%llu%cB)\t", mbi.RegionSize / ost::divByte.second, ost::divByte.first);
        } else {
            WCHAR szRegSize[MAX_PATH] = {0};
            ost::btoStrDL(mbi.RegionSize, szRegSize);
            printf("(%ls)\t", szRegSize);
        }
        // ��ʾ��״̬
        printf("%-*s\t", ost::SHORT_STR_LEN,
               ost::mbiStateMap.count(mbi.State) ?
               ost::mbiStateMap.at(mbi.State).c_str() :
               "Unknown");
        if (mbi.Protect == 0 && mbi.State != MEM_FREE) {
            mbi.Protect = PAGE_READONLY;
        }
        // ��ʾ�鱣������
        printf("%-*s ", ost::LONG_STR_LEN,
               ost::mbiProtectMap.count(mbi.Protect) ?
               ost::mbiProtectMap.at(mbi.Protect).c_str() :
               "Unknown");
        // ��ʾ������
        printf("%-*s", ost::SHORT_STR_LEN,
               ost::mbiTypeMap.count(mbi.Type) ?
               ost::mbiTypeMap.at(mbi.Type).c_str() :
               "Unknown");
        TCHAR szFilename[MAX_PATH];
        // ��ǰ�����Ѽ���ģ��
        if (GetModuleFileName(
                reinterpret_cast<HMODULE>(accAdd),
                szFilename,
                MAX_PATH) > 0) {
            // ��ȥǰ׺·��
            PathStripPath(szFilename);
            printf("\tModule: %s", szFilename);
        }
        putchar('\n');
        accAdd = endAdd;

    }
}
