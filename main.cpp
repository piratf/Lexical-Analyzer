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

    DFA(const std::map<char, unsigned int> &title,
        const std::vector<std::vector<unsigned int> > &vecData,
        const std::set<unsigned int> &sendState)
        : _title(title), _vecData(vecData), _sendState(sendState) {

    }

    DFA(std::map<char, unsigned int> &&title,
        std::vector<std::vector<unsigned int> > &&vecData,
        std::set<unsigned int> &&sendState)
        : _title(title), _vecData(vecData), _sendState(sendState) {

    }

    void display() {
        puts("--------- start display ------------");
        printf("the title chars are: \n");
        for (auto &var : _title) {
            printf("%c ", var.first);
        }
        putchar(10);

        printf("the data matrix is:\n");
        for (auto &row : _vecData) {
            for (unsigned int var : row) {
                printf("%d ", var);
            }
            putchar(10);
        }

        printf("the end state set:\n");
        for (unsigned int var : _sendState) {
            printf("%d ", var);
        }
        putchar(10);
        puts("----------- end display ------------");

    }

    ~DFA() = default;

  private:
    std::map<char, unsigned int> _title;
    std::vector<std::vector<unsigned int> > _vecData;
    std::set<unsigned int> _sendState;
};

DFA *buildDFA(NFA *nfa) {
    // 更新 nfa 状态
    nfa -> update();
    std::set<char> schar = nfa -> schar();
    unsigned int chCnt = schar.size();
    // 存放 dfa char 和对应的 index
    std::map<char, unsigned int> titleMap;
    // 存放 dfa data
    std::vector<std::vector<unsigned int> > vecData;

    // printf("cnt of char: %d\n", schar.size());

    int cnt = 0;

    for (char var : schar) {
        titleMap[var] = cnt;
        ++cnt;
    }

    cnt = 0;
    // 初始集合
    std::set<NFANode *> sstart;
    // 当前遍历的集合
    std::set<NFANode *> scur;
    // 存储终态集合
    std::set<unsigned int> sendState;
    sstart.insert(nfa -> head());
    // 空闭包
    nfa -> getEplisonClosure(sstart);
    // 所有状态
    std::map<std::set<NFANode *>, unsigned int> sdstates;
    // 所有未标记状态
    std::queue<std::set<NFANode *>> qunflag;
    // 未标记状态对应的 ID
    std::queue<unsigned int> qid;

    sdstates[sstart] = cnt;
    qid.push(cnt);
    qunflag.push(sstart);
    vecData.push_back(std::vector<unsigned int>(chCnt));

    int curID = 0;

    while (!qunflag.empty()) {
        // 标记 X
        scur = qunflag.front();
        curID = qid.front();
        // printf("at start, scur size = %d\n", scur.size());
        qunflag.pop();
        qid.pop();

        // for 每一个输入字符 a
        for (char ch : schar) {
            std::set<NFANode *> su = scur;
            nfa -> getRouteClosure(ch, su);
            nfa -> getEplisonClosure(su);

            if (sdstates.find(su) == sdstates.end()) {

                ++cnt;
                sdstates[su] = cnt;
                qunflag.push(su);
                qid.push(cnt);
                vecData.push_back(std::vector<unsigned int>(chCnt));

                if (su.find(nfa -> tail()) != su.end()) {
                    sendState.insert(cnt);
                }
            }

            vecData[curID][titleMap[ch]] = sdstates[su];
        }
    }

    DFA *dfa = new DFA(std::move(titleMap), std::move(vecData), std::move(sendState));

    return dfa;
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

    DFA *dfa = buildDFA(nfa);
    dfa -> display();
    delete nfa;
    delete dfa;
    return 0;
}