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

#include "cmdline.h"

namespace ost {
    // 程序功能表
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
    // 程序显示单位(B,KB,MB,GB)
    std::pair<char, unsigned long> divByte;
}

int main(int argc, char **argv) {
    // 默认单位，前项若为0则为自动
    ost::divByte = std::make_pair(0, 1);
    // 命令行参数处理
    cmdline::parser par;

    std::ios::sync_with_stdio();

    par.set_program_name("OStask");
    // 增加参数类型
    par.add<unsigned long>("inquire", 'i', "Inquire the selected process info.",
                           false, ost::PID_MIN, cmdline::range<unsigned long>(ost::PID_MIN, ost::PID_MAX));
    par.add<unsigned int>("loop", 'l', "loop this program from [1-65535] second.",
                          false, ost::MIN_TIME, cmdline::range(ost::MIN_TIME, ost::MAX_TIME));
    par.add<int>("type", 'y', "Set the show byte type[0=B,1=KB,2=MB,3=GB],Auto decide if not use this.",
                 false, ost::DEFAULT_BTYPE, cmdline::range(0, 3));

    par.add("help", '?', "show help message.");
    par.add("copyright", 'c', "show copyright and license information.");
    par.add("all", 'a', "show all info.");

    for (auto &&[arg, arf]: ost::funcMap) {
        par.add(arg, arf.shortName, arf.desc);
    }
    // 若有help或无参，显示帮助信息
    if (argc <= 1 || !par.parse(argc, argv) || par.exist("help")) {
        std::cout << par.error() << par.usage();
        return 0;
    }
    // 显示开源协议
    if (par.exist("copyright")) {
        std::cout << "[LICENSE]:\n"
                     "\n"
                     "Copyright 2022 Gzh0821/Gaozih\n"
                     "\n"
                     "Licensed under the Apache License, Version 2.0 (the \"License\");\n"
                     "you may not use this file except in compliance with the License.\n"
                     "You may obtain a copy of the License at\n"
                     "\n"
                     "http://www.apache.org/licenses/LICENSE-2.0\n"
                     "\n"
                     "Unless required by applicable law or agreed to in writing, software\n"
                     "distributed under the License is distributed on an \"AS IS\" BASIS,\n"
                     "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
                     "See the License for the specific language governing permissions and\n"
                     "limitations under the License.\n"
                     "\n";
        return 0;
    }
    // 设置程序使用的单位
    if (par.exist("type")) {
        int type = par.get<int>("type");
        ost::divByte.first = ost::BTYPE_NAME[type];
        for (int i = 0; i < type; ++i)
            ost::divByte.second *= ost::DIV;
    }

    // 根据参数调用函数表
    auto doFunc = [&]() {
        for (auto &&[arg, arf]: ost::funcMap)
            if (par.exist("all") || par.exist(arg))
                arf.func();
    };

    // 查询单个进程
    if (par.exist("inquire")) {
        auto pid = par.get<unsigned long>("inquire");
        ost::processInfo(pid);
        return 0;
    }
    // 循环执行程序
    if (par.exist("loop")) {
        auto loopCount = par.get<unsigned int>("loop");
        while (loopCount--) {
            system("cls");
            doFunc();
            printf("[LEFT TIME]:%d", loopCount);
            Sleep(1000);
        }
    } else
        // 默认执行
        doFunc();
    return 0;
}