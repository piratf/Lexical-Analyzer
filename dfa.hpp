#ifndef DFA_H_
#define DFA_H_

#include "nfa.hpp"
#include <cstdio>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

class DFA {
  public:
    // DFA() = default;

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

    unsigned int move(unsigned int state, char title) {
        return _vecData[state][_title[title]];
    }

    bool calculate(const char *str) {
        unsigned int s = 0;

        const char *p = str;

        while (*p) {
            printf("p = %c\n", *p);
            s = move(s, *p);
            printf("s = %d\n", s);
            ++p;
        }

        if (_sendState.find(s) != _sendState.end()) {
            return true;
        } else {
            return false;
        }
    }

    void display() {
        puts("--------- start display ------------");
        printf("my tag is: %s\n", _tag.data());
        printf("the title chars are: \n");

        for (auto &var : _title) {
            printf("%c ", var.first);
        }

        putchar(10);

        printf("the data matrix is:\n");

        int rowid = 0;

        for (auto &row : _vecData) {

            printf("%d: ", rowid++);

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
        stemp.insert(sstart);
        stemp.insert(send);
        char titleID = 0;

        while (stemp.size() != sdivide.size()) {
            sdivide.swap(stemp);
            stemp.clear();

            // printf("sdivide\n");

            // for (auto &d : sdivide) {
            //     for (unsigned int data : d) {
            //         printf("%d ", data);
            //     }

            //     putchar(10);
            // }

            bool endOfMinimize = true;

            // 遍历当前划分中的每一个集合
            for (std::set<unsigned int> divide : sdivide) {

                if (divide.size() == 1) {
                    stemp.insert(divide);
                    continue;
                }

                std::set<unsigned int> subset;

                // 遍历 _title 中每一个可能的输入字符
                for (auto &var : _title) {
                    titleID = var.second;
                    subset.clear();

                    // 遍历当前所有状态，填充 subset 集合
                    for (unsigned int state : divide) {
                        subset.insert(_vecData[state][titleID]);
                    }

                    bool needChange = true;

                    // 判断结果是不是在同一个划分内
                    for (auto d : sdivide) {
                        if (std::includes(d.begin(), d.end(), subset.begin(), subset.end())) {
                            needChange = false;
                            break;
                        }
                    }

                    // 如果不是
                    if (needChange) {
                        // 获得和所有 divide 的交集
                        for (auto d : sdivide) {
                            std::set<unsigned int> st;
                            std::set_intersection(subset.begin(), subset.end(), d.begin(), d.end(), std::inserter(st, st.begin()));

                            // 得到一个不为空的交集，添加所有单步结果在此交集内的元素到新的划分中
                            if (!st.empty()) {
                                std::set<unsigned int> temp;

                                for (unsigned int state : divide) {
                                    if (st.find(_vecData[state][titleID]) != st.end()) {
                                        temp.insert(state);
                                    }
                                }

                                stemp.insert(temp);
                            }
                        }

                        endOfMinimize = false;
                        break;
                    }

                }

                if (endOfMinimize) {
                    stemp.insert(divide);
                }
            }
        }

        puts("after minimize");

        for (auto &s : sdivide) {
            for (unsigned int var : s) {
                printf("%d ", var);
            }

            putchar(10);
        }

        puts("=========================");

        fflush(stdout);

        // modify data matrix
        for (auto &s : sdivide) {
            if (s.size() > 1) {
                unsigned int maxOfSet = *s.rbegin();
                auto it = s.begin();
                unsigned int tag = *it;

                // update end state
                std::vector<unsigned int> modify;
                std::vector<unsigned int> vecRemove;

                for (auto endit = _sendState.begin();
                        endit != _sendState.end(); ++endit) {
                    if (s.find(*endit) != s.end()) {
                        vecRemove.push_back(*endit);
                        continue;
                    }

                    if (*endit > maxOfSet) {
                        modify.push_back(*endit);
                        continue;
                    }

                }


                for (auto &var : modify) {
                    _sendState.erase(var);
                    _sendState.insert(tag);
                }

                for (auto &var : modify) {
                    _sendState.erase(var);
                    _sendState.insert(var - 1);
                }

                for (auto &row : _vecData) {
                    for (unsigned int &var : row) {
                        if (s.find(var) != s.end()) {
                            var = tag;
                        } else if (var > maxOfSet) {
                            --var;
                        }
                    }
                }

                ++it;

                for (; it != s.end(); ++it) {
                    // printf("it = %d\n", *it);
                    _vecData.erase(_vecData.begin() + *it);
                    _sendState.erase(*it);
                }
            }
        }

        printf("start to remove died node\n");
        fflush(stdout);

        // std::vector<unsigned int> vecDied;
        // bool diedFlag = true;

        // for (auto it = _vecData.begin(); it != _vecData.end(); ++it) {
        //     unsigned int index = it - _vecData.begin();
        //     diedFlag = true;

        //     for (auto &var : *it) {
        //         if (var != index) {
        //             diedFlag = false;
        //             break;
        //         }
        //     }

        //     if (diedFlag) {
        //         printf("died: %d\n", index);
        //         vecDied.push_back(index);
        //     }
        // }

        // while (!vecDied.empty()) {
        //     for (auto &died : vecDied) {
        //         _vecData.erase(_vecData.begin() + died);

        //         for (auto it = _vecData.begin(); it != _vecData.end(); ++it) {
        //             for (unsigned int &var : *it) {
        //                 if (var == died) {

        //                     if (_sendState.find(died) != _sendState.end()) {
        //                         _sendState.erase(died);
        //                         _sendState.insert(it - _vecData.begin());
        //                     }

        //                     var = it - _vecData.begin();
        //                 } else if (var > died) {
        //                     --var;
        //                 }
        //             }
        //         }
        //     }

        //     display();

        //     vecDied.clear();

        //     for (auto it = _vecData.begin(); it != _vecData.end(); ++it) {
        //         unsigned int index = it - _vecData.begin();
        //         diedFlag = true;

        //         for (auto &var : *it) {
        //             if (var != index) {
        //                 diedFlag = false;
        //                 break;
        //             }
        //         }

        //         if (diedFlag) {
        //             printf("died: %d\n", index);
        //             vecDied.push_back(index);
        //         }
        //     }
        // }
    }

    std::string tag() {
        return _tag;
    }

    void tag(const std::string &tag) {
        _tag = tag;
    }

    ~DFA() = default;

  private:
    std::map<char, unsigned int> _title;
    std::vector<std::vector<unsigned int> > _vecData;
    std::set<unsigned int> _sendState;
    std::string _tag = "default tag of dfa.";
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


    // minimize end state

    // for (auto &endstate : sendState) {
    //     for (unsigned int &var : vecData[endstate]) {
    //         sendState.insert(var);
    //     }
    // }

    DFA *dfa = new DFA(std::move(titleMap), std::move(vecData), std::move(sendState));

    return dfa;
}

DFA *buildDFA(const std::basic_string<char> &tag, std::basic_string<char> &reg) {
    RegTree *root = buildRegTree(reg);
    // root -> backOrderDisplay();
    // root -> middleOrderDisplay();
    fflush(stdout);
    NFA *nfa = buildNFA(root);
    DFA *dfa = buildDFA(nfa);
    dfa -> tag(tag);
    return dfa;
}

#endif
