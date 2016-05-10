#include "regtree.hpp"
#include "nfa.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>

class DFA {
  public:
    DFA() = default;
    ~DFA() = default;


};

DFA *buildDFA(NFA *nfa) {
    int cnt = 0;
    std::set<char> schar = nfa -> getCharacters(cnt);
    schar.erase(0);
    std::map<char, int> titleMap;

    std::vector<set::vector<unsigned int> > vecData(cnt, std::<vector> );


    printf("cnt of char: %d\n", schar.size());

    cnt = 0;

    for (char var : schar) {
        titleMap[var] = cnt;
        ++cnt;
    }

    cnt = 0;
    std::set<NFANode *> sstart;
    std::set<NFANode *> scur;
    sstart.insert(nfa -> head());
    nfa -> getEplisonClosure(sstart);
    // 所有状态
    std::map<std::set<NFANode *>, unsigned int> sdstates;
    // 所有未标记状态
    std::queue<std::set<NFANode *>> qunflag;

    sdstates[sstart] = cnt;
    qunflag.push(sstart);

    while (!qunflag.empty()) {
        // 标记 X
        scur = qunflag.front();
        printf("at start, scur size = %d\n", scur.size());
        qunflag.pop();

        // for 每一个输入字符 a
        for (char ch : schar) {
            printf("ch = %c\n", ch);
            std::set<NFANode *> su = scur;
            nfa -> getRouteClosure(ch, su);
            nfa -> getEplisonClosure(su);
            printf("in loop, scur size = %d\n", su.size());

            if (sdstates.find(su) == sdstates.end()) {
                ++cnt;
                sdstates[su] = cnt;
                qunflag.push(su);
            }

            sdstates[cur] = titleMap[ch];
        }
    }

    printf("sdstates count = %d\n", sdstates.size());

    for (auto var : sdstates) {
        printf("first size = %d, id = %d\n", var.first.size(), var.second);
    }
    return NULL;
}

NFA *inputNFA() {
    ifstream input("input.txt");
    string reg;
    input >> reg;
    input.close();
    RegTree *root = buildRegTree(reg);
    root -> backOrderDisplay();
    root -> middleOrderDisplay();
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
    nfa -> display();
    fflush(stdout);
    // test(nfa);

    buildDFA(nfa);
    delete nfa;
    return 0;
}