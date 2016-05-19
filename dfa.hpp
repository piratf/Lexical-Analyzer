#ifndef DFA_H_
#define DFA_H_

#include "nfa.hpp"
#include <cstdio>
#include <memory.h>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

const size_t CHAR_CNT = 259;

class DFA {
  public:
    // DFA() = default;

    DFA(int *titleHash,
        std::vector<std::vector<unsigned int> > &&vecData,
        std::set<unsigned int> &&sendState)
        : _titleHash(titleHash),
          _vecData(std::move(vecData)),
          _sendState(std::move(sendState)) {

    }

    unsigned int move(unsigned int state, char title) {
        if (_titleHash[static_cast<size_t>(title)] == -1) {
            return -1;
        }
        if (state >= _vecData.size() || state < 0) {
            return state;
        }

        return _vecData[state][_titleHash[static_cast<size_t>(title)]];
    }

    bool calculate(const char *str) {
        unsigned int s = 0;

        const char *p = str;

        while (*p) {
            // printf("*p = %c\n", *p);
            s = move(s, *p);
            // printf("s = %d\n", s);
            // fflush(stdout);

            if (s == static_cast<unsigned int>(-1)) {
                return false;
            }
            
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
        puts("the title chars are: ");
        putchar(' ');
        putchar(' ');
        putchar(' ');

        for (size_t i = 0; i < CHAR_CNT; ++i) {
            if (~_titleHash[i]) {
                printf("%2c ", i);
            }
        }

        putchar(10);

        printf("the data matrix is:\n");

        int rowid = 0;

        for (auto &row : _vecData) {

            printf("%-2d: ", rowid++);

            for (unsigned int var : row) {
                printf("%-2d ", var);
            }

            putchar(10);
        }

        printf("the end state set:\n");

        for (unsigned int var : _sendState) {
            printf("%-2d ", var);
        }

        putchar(10);
        puts("----------- end display ------------");

    }

    void minimize() {
        std::set<unsigned int> sstart;
        decltype(sstart) send(_sendState.begin(), _sendState.end());

        for (unsigned int i = 0; i < _vecData.size(); ++i) {
            if (send.find(i) == send.end()) {
                sstart.insert(i);
            }
        }

        std::set<decltype(sstart)> sdivide;
        decltype(sdivide) stemp;
        stemp.insert(sstart);
        stemp.insert(send);
        char currentChar = 0;

        // display();

        while (stemp != sdivide) {
            sdivide.swap(stemp);
            stemp.clear();

            // puts("==========================");
            // printf("sdivide\n");

            // for (auto &d : sdivide) {
            //     for (unsigned int data : d) {
            //         printf("%d ", data);
            //     }

            //     putchar(10);
            // }

            bool endOfMinimize = true;

            // 遍历当前划分中的每一个集合
            for (const auto &divide : sdivide) {

                endOfMinimize = true;

                // puts("new divide ========> ");

                if (divide.size() == 1) {
                    stemp.insert(divide);
                    continue;
                }

                decltype(sstart) subset;

                // 遍历 _title 中每一个可能的输入字符
                for (unsigned int i = 0; i < _vecData[0].size(); ++i) {
                    currentChar = i;
                    // printf("currentChar = %d\n", currentChar);
                    subset.clear();

                    // 遍历当前所有状态，填充 subset 集合
                    for (unsigned int state : divide) {
                        subset.insert(_vecData[state][currentChar]);
                    }

                    bool needChange = true;

                    // printf("subset: => ");

                    // for (auto &var : subset) {
                    //     printf("%d ", var);
                    // }

                    // putchar(10);

                    // 判断结果是不是在同一个划分内
                    for (auto d : sdivide) {
                        if (std::includes(d.begin(), d.end(), subset.begin(), subset.end())) {
                            needChange = false;
                            break;
                        }
                    }

                    // 如果不是
                    if (needChange) {
                        // printf("need change.\n");

                        // 获得和所有 divide 的交集
                        for (auto d : sdivide) {
                            decltype(subset) st;
                            std::set_intersection(subset.begin(), subset.end(), d.begin(), d.end(), std::inserter(st, st.begin()));

                            // 得到一个不为空的交集，添加所有单步结果在此交集内的元素到新的划分中
                            if (!st.empty()) {

                                if (d == divide) {
                                    decltype(subset) temp;

                                    // traverse current divide
                                    for (unsigned int state : divide) {

                                        if (st.find(_vecData[state][currentChar]) != st.end()) {
                                            temp.insert(state);
                                        }
                                    }

                                    stemp.insert(temp);
                                } else {
                                    for (unsigned int state : divide) {

                                        if (st.find(_vecData[state][currentChar]) != st.end()) {
                                            decltype(subset) temp;
                                            temp.insert(state);
                                            stemp.insert(temp);
                                        }
                                    }
                                }

                            }
                        }

                        endOfMinimize = false;
                        // puts("stemp: ");

                        // for (auto &d : stemp) {
                        //     for (unsigned int data : d) {
                        //         printf("%d ", data);
                        //     }

                        //     putchar(10);
                        // }

                        break;
                    }
                }

                if (endOfMinimize) {
                    stemp.insert(divide);
                }
            }


        }

        // puts("after minimize");

        // for (const auto &s : sdivide) {
        //     for (unsigned int var : s) {
        //         printf("%d ", var);
        //     }

        //     putchar(10);
        // }

        // puts("=========================");
        // fflush(stdout);


        // puts("modify data matrix.");

        // display();

        // decltype(_vecData) vecNew;
        // vecNew.clear();
        int id = -1;

        for (const auto &divid : sdivide) {
            ++id;

            auto it = divid.begin();
            unsigned int nodeID = *it;

            // for (auto &row : _vecData) {
            //     for (auto &var : row) {
            //         if (var == nodeID) {
            //             if (_sendState.find(var) != _sendState.end()) {
            //                 _sendState.erase(var);
            //                 _sendState.insert(id);
            //             }

            //             // var = id;
            //         }
            //     }
            // }

            ++it;

            if (it != divid.end()) {
                for (; it != divid.end(); ++it) {
                    if (_sendState.find(*it) != _sendState.end()) {
                        _vecData.erase(_vecData.begin() + *it);
                        _sendState.erase(*it);
                        _sendState.insert(nodeID);
                    }

                    for (auto &row : _vecData) {
                        for (auto &var : row) {
                            if (var == *it) {
                                var = nodeID;
                            }
                        }
                    }

                }
            }

            // printf("id = %d\n", id);
            // printf("nodeID = %d\n", nodeID);
            // vecNew.push_back(_vecData[nodeID]);
        }

        // display();

        // printf("\n");

        // for (auto &row : vecNew) {
        //     for (auto &var : row) {
        //         printf("%d ", var);
        //     }

        //     putchar(10);
        // }

        // printf("start to remove died node\n");
        // fflush(stdout);

        // 记录死亡的节点

        bool diedFlag = true;

        for (auto it = _vecData.begin(); it != _vecData.end(); ++it) {
            unsigned int index = it - _vecData.begin();
            diedFlag = true;

            for (auto &var : *it) {
                if (var != index) {
                    diedFlag = false;
                    break;
                }
            }

            if (diedFlag) {
                // printf("died: %d\n", index);
                it = _vecData.erase(it);
            }
        }

    }

    std::string tag() {
        return _tag;
    }

    void tag(const std::string &tag) {
        _tag = tag;
    }

    ~DFA() = default;

  private:
    int *_titleHash;
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
    int *titleHash = new int[CHAR_CNT];
    memset(titleHash, -1, sizeof(int) * CHAR_CNT);
    // 存放 dfa data
    std::vector<std::vector<unsigned int> > vecData;

    // printf("cnt of char: %d\n", schar.size());

    int cnt = 0;

    for (char var : schar) {
        titleHash[static_cast<size_t>(var)] = cnt;
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

            vecData[curID][titleHash[static_cast<size_t>(ch)]] = sdstates[su];
        }
    }


    // minimize end state

    // for (auto &endstate : sendState) {
    //     for (unsigned int &var : vecData[endstate]) {
    //         sendState.insert(var);
    //     }
    // }

    return new DFA(titleHash, std::move(vecData), std::move(sendState));
}

#endif
