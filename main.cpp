#include "LexicalAnalyzer.hpp"
#include "Preprocessor.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>

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

NFA *inputNFA() {
    ifstream input("raw.txt");
    char *str = new char[N];
    input.getline(str, N);
    std::string reg(str);
    trim(reg);
    printf("%s\n", reg.data());
    std::string tag = split(reg);
    printf("tag = %s\n", tag.data());
    printf("reg = %s\n", reg.data());

    input.close();
    RegTree *root = buildRegTree(reg);
    root -> backOrderDisplay();
    root -> middleOrderDisplay();
    fflush(stdout);
    input.close();
    // return NULL;
    return buildNFA(root);
}

DFA *inputDFA() {
    ifstream input("raw.txt");
    char *str = new char[N];
    input.getline(str, N);
    std::string reg(str);
    trim(reg);
    printf("%s\n", reg.data());
    std::string tag = split(reg);
    printf("tag = %s\n", tag.data());
    printf("reg = %s\n", reg.data());

    input.close();
    RegTree *root = buildRegTree(reg);
    // root -> backOrderDisplay();
    // fflush(stdout);
    // return NULL;
    NFA *nfa = buildNFA(root);
    DFA *dfa = buildDFA(nfa);
    dfa ->tag(tag);
    input.close();
    return dfa;
}

LexicalAnalyzer *inputLA() {
    ifstream input("input.txt");
    char *str = new char[N];
    LexicalAnalyzer *la = new LexicalAnalyzer();
    la -> init();

    while (!input.eof()) {
        input.getline(str, N);
        std::string reg(str);
        trim(reg);
        printf("%s\n", reg.data());
        std::string tag = split(reg);
        printf("tag = %s\n", tag.data());
        printf("reg = %s\n", reg.data());
        fflush(stdout);
        RegTree *root = buildRegTree(reg);
        root -> backOrderDisplay();
        root -> middleOrderDisplay();
        fflush(stdout);
        NFA *nfa = buildNFA(root);
        DFA *dfa = buildDFA(nfa);
        dfa -> tag(tag);
        la -> add(dfa);
    }

    input.close();
    return la;
}

bool test(LexicalAnalyzer *la) {
    ifstream input("test.txt");
    string code;
    input >> code;
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
    Preprocessor ppr;
    ifstream input("input.txt");
    char *str = new char[N];

    while (!input.eof()) {
        input.getline(str, N);

        if (strlen(str)) {
            std::string reg(str);
            ppr.update(reg);
        }
    }

    input.close();

    ppr.display();
    fflush(stdout);

    // LexicalAnalyzer *la = ppr.buildLA();
    // test(la);

    // std::string literal("literal");
    // std::string id("id");
    // std::string strchar("char");
    // 
    std::string strtest("longcomment");

    auto &regs = ppr.regs();
    std::string reg = regs[strtest];
    printf("reg = %s\n", reg.data());
    fflush(stdout);
    RegTree *root = buildRegTree(reg);
    root -> backOrderDisplay(); 
    fflush(stdout);

    // DFA *dfa = buildDFA(strtest, reg);
    // dfa -> display();    
    // // dfa -> minimize();
    // fflush(stdout);
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