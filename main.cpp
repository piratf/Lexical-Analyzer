#include "regtree.hpp"
#include "nfa.hpp"
#include "dfa.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>

NFA *inputNFA() {
    ifstream input("input.txt");
    string reg;
    input >> reg;
    input.close();
    RegTree *root = buildRegTree(reg);
    // root -> backOrderDisplay();
    // root -> middleOrderDisplay();
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

int main() {
    freopen("output.txt", "w", stdout);
    NFA *nfa = inputNFA();
    // nfa -> display();
    // test(nfa);

    DFA *dfa = buildDFA(nfa);
    dfa -> display();
    dfa -> minimize();
    dfa -> display();
    fflush(stdout);
    delete nfa;
    delete dfa;
    return 0;
}