#include "LexicalAnalyzer.hpp"
#include "Preprocessor.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <ctime>
#include <windows.h>

// static const int N = 1024;

void trim(std::string &reg) {
    for (auto it = reg.begin(); it != reg.end(); ++it) {
        if (*it == ' ') {
            it = reg.erase(it);
        }
    }
}

std::string split(std::string &reg) {
    auto it = reg.begin();

    for (; it != reg.end(); ++it) {
        if (*it == '=') {
            break;
        }
    }

    std::string tag = reg.substr(0, it - reg.begin());
    reg.erase(reg.begin(), it + 1);
    return tag;
}

bool test(LexicalAnalyzer *la) {
    ifstream input("test.txt");
    char str[N] = {};
    input.getline(str, N);
    string code(str);
    printf("input = %s\n", code.data());

    printf("%s\n", la -> calculate(code.data()).data());
    return true;
}

bool test(NFA *nfa) {
    ifstream input("test.txt");
    string code;
    input >> code;
    printf("input = %s\n", code.data());

    if (nfa -> calculate(code.data())) {
        printf("yes.\n");
        return true;
    } else {
        printf("no.\n");
        return false;
    }
}

bool test(DFA *dfa) {
    ifstream input("test.txt");
    string code;
    input >> code;
    // code = "// 123";
    printf("input = %s\n", code.data());

    if (dfa -> calculate(code.data())) {
        printf("yes.\n");
        return true;
    } else {
        printf("no.\n");
        return false;
    }
}

void preprocess() {

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks

    double elapsedTime;
    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&t1);
    Preprocessor ppr;
    ifstream input("input.txt");
    char *str = new char[N];

    while (!input.eof()) {
        input.getline(str, N);

        if (str[0] != '\n') {
            std::string reg(str);
            ppr.update(reg);
        }
    }

    input.close();
    QueryPerformanceCounter(&t2);
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

    ppr.display();
    // fflush(stdout);
    printf("preprocess time = %lf\n", elapsedTime);

    QueryPerformanceCounter(&t1);
    LexicalAnalyzer *la = ppr.buildLA();
    QueryPerformanceCounter(&t2);
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("la build time = %lf\n", elapsedTime);

    QueryPerformanceCounter(&t1);
    la -> parse("code.txt");
    QueryPerformanceCounter(&t2);
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("parse time = %lf\n", elapsedTime);

    // test(la);

    // std::string literal("literal");
    // std::string id("id");
    // std::string strchar("char");
    //
    // std::string strtest("test");

    // auto &regs = ppr.regs();
    // std::string reg = regs[strtest];

    // printf("reg = %s\n", reg.data());
    // fflush(stdout);
    // RegTree *root = buildRegTree(reg);
    // root -> backOrderDisplay();
    // root -> middleOrderDisplay();
    // fflush(stdout);

    // DFA *dfa = buildDFA(strtest, reg);
    // dfa -> minimize();
    // dfa -> display();
    // fflush(stdout);

    // test(dfa);

    // la -> add(dfa);

}

int main() {
    freopen("output.txt", "w", stdout);
    // NFA *nfa = inputNFA();
    // nfa -> display();
    // test(nfa);

    // DFA *dfa = inputDFA();
    // dfa -> display();
    // dfa -> minimize();
    // test(dfa);
    // dfa -> display();
    // fflush(stdout);
    // delete nfa;
    // delete dfa;

    preprocess();
    return 0;
}