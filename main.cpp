#include "lexical_analyzer.hpp"
#include "preprocessor.hpp"
#include "pclock.hpp"
#include <cstdio>
#include <fstream>
#include <chrono>
using namespace piratf;
using namespace std;

/**
 * 预处理正规式文件
 * @author piratf
 * @param  filePath 正规式文件路径
 * @param  ppr      被更新的预处理对象
 */
void preprocess(const char *filePath, Preprocessor &ppr) {

    ifstream input(filePath);
    char str[N] = {};

    while (!input.eof()) {
        input.getline(str, N, '\n');

        // 处理正规式注释
        if (str[0] && str[0] != '#') {
            std::string reg(str, strlen(str));

            // 处理 CRLF 换行
            if (*reg.rbegin() == 13) {
                reg.pop_back();
            }

            ppr.update(reg);
        }
    }

    input.close();

    ppr.display();
}

void test(Preprocessor &ppr, LexicalAnalyzer &la) {
    preprocess("input.txt", ppr);
    ppr.buildLA(la);
    la.parse("main.cpp");
}

int main(int argc, char *argv[]) {
    freopen("output.txt", "w", stdout);
    Preprocessor ppr;
    LexicalAnalyzer la;
    Clock clock;

    printf("argc = %d\n", argc);

    if (argc > 1) {
        clock.start("preprocess.");
        preprocess(argv[1], ppr);
        clock.terminal("preprocess end.");
        clock.start("build LA.");
        ppr.buildLA(la);
        clock.terminal("build LA end.");
    } else {
        test(ppr, la);
    }

    if (argc > 2) {
        clock.start("parse.");
        la.parse(argv[2]);
        clock.terminal("parse end.");
    }

    return 0;
}