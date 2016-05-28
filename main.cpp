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
//         printf("yes.123123123\n");
//         return true;
//     } else {
//         printf("no.\n");
//         return false;
//     }
// }

void preprocess(const char *filePath, Preprocessor &ppr) {

    auto begin = std::chrono::high_resolution_clock::now();
    ifstream input(filePath);
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
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    freopen("output.txt", "w", stdout);
    Preprocessor ppr;
    LexicalAnalyzer la;

    printf("argc = %d\n", argc);

    if (argc > 1) {
        preprocess(argv[1], ppr);
        auto begin = std::chrono::high_resolution_clock::now();
        ppr.buildLA(la);
        auto end = std::chrono::high_resolution_clock::now();
        printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    }

    if (argc > 2) {
        auto begin = std::chrono::high_resolution_clock::now();
        la.parse(argv[2]);
        la.parse("test.txt");
        auto end = std::chrono::high_resolution_clock::now();
        printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    }

    return 0;
}