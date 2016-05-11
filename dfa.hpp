#ifndef DFA_H_
#define DFA_H_

#include "regtree.hpp"
#include "nfa.hpp"
#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

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
        : _title(std::move(title)),
          _vecData(std::move(vecData)),
          _sendState(std::move(sendState)) {

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

    void minimize() {
        std::set<unsigned int> sstart;
        std::set<unsigned int> send = _sendState;

        for (unsigned int i = 0; i < _vecData.size(); ++i) {
            if (send.find(i) == send.end()) {
                sstart.insert(i);
            }
        }

        std::set<std::set<unsigned int> > sdivide;
        std::set<std::set<unsigned int> > stemp;
        printf("%d %d\n", sstart.size(), send.size());
        stemp.insert(sstart);
        stemp.insert(send);
        char titleID = 0;

        while (stemp.size() != sdivide.size()) {
            sdivide.swap(stemp);
            stemp.clear();

            for (auto &s : sdivide) {
                for (unsigned int state : s) {
                    printf("%d ", state);
                }

                putchar(10);
            }

            for (std::set<unsigned int> divide : sdivide) {

                if (divide.size() == 1) {
                    continue;
                }

                printf("divide size() %d\n", divide.size());

                std::set<unsigned int> subset;

                // 遍历每一个可能的输入字符
                for (auto &var : _title) {
                    titleID = var.second;
                    printf("id = %d\n", titleID);

                    subset.clear();

                    // 遍历当前所有状态
                    for (unsigned int state : divide) {
                        printf("state %d\n", state);
                        printf("data = %d\n", _vecData[state][titleID]);

                        subset.insert(_vecData[state][titleID]);
                    }

                    bool flag = false;

                    for (auto &divide : sdivide) {
                        if (!std::includes(divide.begin(), divide.end(), subset. begin(), subset.end())) {
                            flag = true;
                        }
                    }

                    // 如果不属于同一个组
                    if (flag) {
                        unsigned int id = static_cast<unsigned int>(-1);

                        for (auto &divide : sdivide) {
                            std::set<unsigned int> su;
                            std::set<unsigned int> si;

                            std::set_union(divide.begin(), divide.end(), subset.begin(), subset.end(), std::back_inserter(su));
                            std::set_intersection(su.begin(), su.end(), subset.begin(), subset.end(), std::back_inserter(si));

                            if (si.size() == 1) {
                                id = *si.begin();
                                break;
                            }
                        }

                        if (id == static_cast<unsigned int>(-1)) {
                            printf("error: cannot minimize.\n");
                        }

                        subset.clear();
                        // 遍历当前所有状态
                        for (unsigned int state : divide) {
                            if (_vecData[state][titleID] == id) {
                                divide.erase(state);
                                subset.insert(state);
                            }
                        }

                        stemp.insert(divide);
                        stemp.insert(subset);
                        break;
                    }
                }
            }
        }

        printf("%d\n", stemp.size());
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

#endif
