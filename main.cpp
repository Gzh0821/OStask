#include "cmdline.h"

namespace ost {
    const std::unordered_map<WORD, std::string> ARCH_LIST = {{9,      "x64"},
                                                             {5,      "ARM"},
                                                             {12,     "ARM64"},
                                                             {6,      "Intel Itanium"},
                                                             {0,      "x86"},
                                                             {0xffff, "Unknown"}};
    std::pair<char, int> divByte;
    std::map<std::string, ost::ArguFunc> funcMap = {
            {"perf",
                    ArguFunc('p', "show system performance value info.", &ost::showPerformance)},
            {"sys",
                    ArguFunc('s', "show system memory info.", &ost::showSys)},
            {"total",
                    ArguFunc('t', "show total memory usage.", &ost::showTotal)},
            {"each",
                    ArguFunc('e', "show each process info.", &ost::showEachProcess)},
            {"hardware",
                    ArguFunc('w', "show PC hardware information.", &ost::showHardwareInfo)}
    };
}

int main(int argc, char **argv) {
    ost::divByte = std::make_pair('K', ost::DIV);
    cmdline::parser par;
    std::ios::sync_with_stdio();

    par.set_program_name("OStask");
    par.add<unsigned long>("inquire", 'i', "Inquire the selected process info.",
                           false, ost::PID_MIN, cmdline::range<unsigned long>(ost::PID_MIN, ost::PID_MAX));
    par.add("help", '?', "show help message.");
    par.add("all", 'a', "show all info.");
    par.add<unsigned int>("loop", 'l', "loop this program from [1-65535] second.",
                          false, ost::MIN_TIME, cmdline::range(ost::MIN_TIME, ost::MAX_TIME));

    for (auto &&[arg, arf]: ost::funcMap) {
        par.add(arg, arf.shortName, arf.desc);
    }

    if (argc <= 1 || !par.parse(argc, argv) || par.exist("help")) {
        std::cout << par.error() << par.usage();
        return 0;
    }

    auto doFunc = [&]() {
        for (auto &&[arg, arf]: ost::funcMap)
            if (par.exist("all") || par.exist(arg))
                arf.func();
    };

    if (par.exist("inquire")) {
        auto pid = par.get<unsigned long>("inquire");
        ost::processInfo(pid);
        return 0;
    }
    if (par.exist("loop")) {
        auto loopCount = par.get<unsigned int>("loop");
        while (loopCount--) {
            system("cls");
            doFunc();
            printf("[LEFT TIME]:%d", loopCount);
            Sleep(1000);
        }
    } else
        doFunc();
    return 0;
}