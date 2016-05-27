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

Preprocessor preprocess(const char *filePath) {

    auto begin = std::chrono::high_resolution_clock::now();
    Preprocessor ppr;
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

    return std::move(ppr);
}

void test(const char *regFile, const char *codeFile) {
    Preprocessor ppr = std::move(preprocess(regFile));

    auto begin = std::chrono::high_resolution_clock::now();
    LexicalAnalyzer la = ppr.buildLA();
    auto end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " micro seconds" << std::endl;
    printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

    begin = std::chrono::high_resolution_clock::now();
    la.parse(codeFile);
    // la.parse("test.txt");
    end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " micro seconds" << std::endl;
    printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

int main(int argc, char *argv[]) {
    freopen("output.txt", "w", stdout);

    printf("%s %s\n", argv[1], argv[2]);

    test(argv[1], argv[2]);

    return 0;
}