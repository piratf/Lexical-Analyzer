#include "regtree.hpp"
#include "nfa.hpp"
#include "dfa.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>

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
    ifstream input("input.txt");
    const int N = 1024;
    char *str = new char[N];
    input.getline(str, 1000);
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
    // return NULL;
    return buildNFA(root);
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

int main() {
    freopen("output.txt", "w", stdout);
    NFA *nfa = inputNFA();
    nfa -> display();
    // test(nfa);

    DFA *dfa = buildDFA(nfa);
    dfa -> display();
    // dfa -> minimize();
    test(dfa);
    // dfa -> display();
    // fflush(stdout);
    // delete nfa;
    // delete dfa;
    return 0;
}