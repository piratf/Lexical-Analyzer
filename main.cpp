#include "LexicalAnalyzer.hpp"
#include "Preprocessor.hpp"
#include <cstdio>
#include <fstream>
#include <chrono>
using namespace std;

// static const int N = 1024;   

// bool test(LexicalAnalyzer *la) {
//     ifstream input("test.txt");
//     char str[N] = {};
//     input.getline(str, N);
//     string code(str);
//     printf("input = %s\n", code.data());

//     printf("%s\n", la -> calculate(code.data()).data());
//     return true;
// }

// bool test(NFA *nfa) {
//     ifstream input("test.txt");
//     string code;
//     input >> code;
//     printf("input = %s\n", code.data());

//     if (nfa -> calculate(code.data())) {
//         printf("yes.\n");
//         return true;
//     } else {
//         printf("no.\n");
//         return false;
//     }
// }

// bool test(DFA *dfa) {
//     ifstream input("test.txt");
//     string code;
//     input >> code;
//     // code = "// 123";
//     cout << "input = " << code << endl;

//     if (dfa -> calculate(code.data())) {
//         printf("yes.\n");
//         return true;
//     } else {
//         printf("no.\n");
//         return false;
//     }
// }

void preprocess() {

    auto begin = std::chrono::high_resolution_clock::now();
    Preprocessor ppr;
    ifstream input("input.txt");
    char str[N] = {};

    while (!input.eof()) {
        input.getline(str, N, '\n');

        if (str[0]) {
            std::string reg(str, strlen(str));
            if (*reg.rbegin() == 13 || *reg.rbegin() == '#') {
                reg.pop_back();
            }
            // printf("rrrr = %c\n", *reg.rbegin());
            ppr.update(reg);
        }
    }

    input.close();

    ppr.display();
    auto end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " micro seconds" << std::endl;
    printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    // fflush(stdout);

    begin = std::chrono::high_resolution_clock::now();
    LexicalAnalyzer la = ppr.buildLA();
    end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " micro seconds" << std::endl;
    printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

    begin = std::chrono::high_resolution_clock::now();
    la.parse("main.cpp");
    // la.parse("test.txt");
    end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " micro seconds" << std::endl;
    printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

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
    // RegTree *root = ppr.buildRegTree(reg);
    // root -> backOrderDisplay();
    // fflush(stdout);
    // root -> middleOrderDisplay();
    // NFA *nfa = buildNFA(root);
    // DFA *dfa = buildDFA(nfa);
    // fflush(stdout);

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