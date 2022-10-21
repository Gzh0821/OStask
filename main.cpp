// Copyright (c) 2022 Gzh0821/Gaozih
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "cmdline.h"

namespace ost {
    const std::unordered_map<WORD, std::string> ARCH_LIST = {{9,      "x64"},
                                                             {5,      "ARM"},
                                                             {12,     "ARM64"},
                                                             {6,      "Intel Itanium"},
                                                             {0,      "x86"},
                                                             {0xffff, "Unknown"}};

    const std::map<std::string, ost::ArguFunc> funcMap = {
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
    std::pair<char, unsigned long> divByte;
}

int main(int argc, char **argv) {
    ost::divByte = std::make_pair(0, 1);
    cmdline::parser par;
    std::ios::sync_with_stdio();

    par.set_program_name("OStask");
    par.add<unsigned long>("inquire", 'i', "Inquire the selected process info.",
                           false, ost::PID_MIN, cmdline::range<unsigned long>(ost::PID_MIN, ost::PID_MAX));
    par.add("help", '?', "show help message.");
    par.add("all", 'a', "show all info.");
    par.add<unsigned int>("loop", 'l', "loop this program from [1-65535] second.",
                          false, ost::MIN_TIME, cmdline::range(ost::MIN_TIME, ost::MAX_TIME));
    par.add<int>("type", 'y', "Set the show byte type[0=B,1=KB,2=MB,3=GB],Auto decide if not use this.",
                 false, ost::DEFAULT_BTYPE, cmdline::range(0, 3));
    for (auto &&[arg, arf]: ost::funcMap) {
        par.add(arg, arf.shortName, arf.desc);
    }
    if (argc <= 1 || !par.parse(argc, argv) || par.exist("help")) {
        std::cout << par.error() << par.usage();
        return 0;
    }
    if (par.exist("type")) {
        int type = par.get<int>("type");
        ost::divByte.first = ost::BTYPE_NAME[type];
        for (int i = 0; i < type; ++i)
            ost::divByte.second *= ost::DIV;
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