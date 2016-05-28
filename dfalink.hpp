#ifndef DFA_H_
#define DFA_H_

#include "nfatable.hpp"
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <list>
#include <array>

using iterator_array = int **;
const size_t CHAR_CNT = 259;

class DFA {
  public:
    // DFA() = default;

    DFA(std::array<int, CHAR_CNT> &char_hash,
        size_t char_count,
        std::vector<iterator_array> &&vec_list_index,
        std::list<iterator_array> &&listData,
        std::set<iterator_array> &&sendState)
        : _char_hash(char_hash),
          _char_count(char_count),
          _vec_list_index(vec_list_index),
          _listData(std::move(listData)),
          _sendState(std::move(sendState)) {

    }

    DFA(std::array<int, CHAR_CNT> &char_hash,
        size_t char_count,
        std::vector<iterator_array> &vec_list_index,
        std::list<iterator_array> &listData,
        std::set<iterator_array> &sendState)
        : _char_hash(char_hash),
          _char_count(char_count),
          _vec_list_index(vec_list_index),
          _sendState(sendState) {
        _listData = listData;
    }

    ~DFA() {
        for (auto fuck : _listData) {
            delete[] fuck;
        }
    }

    iterator_array move(iterator_array state, char title) {
        if (_char_hash[static_cast<size_t>(title)] == -1) {
            return NULL;
        }

        return reinterpret_cast<iterator_array>
               (state[_char_hash[static_cast<size_t>(title)]]);
    }

    bool calculate(const char *str) {
        iterator_array s = *_listData.begin();

        const char *p = str;

        while (*p) {
            // printf("*p = %c\n", *p);
            // fflush(stdout);
            s = move(s, *p);
            // printf("s = %d\n", s);

            if (!s) {
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
            if (~_char_hash[i]) {
                printf("%2c ", i);
            }
        }

        putchar(10);

        printf("the data matrix is:\n");

        for (auto &row : _listData) {

            printf("%-8p: ", reinterpret_cast<void *>(row));

            for (unsigned int i = 0; i < _char_count; ++i) {
                printf("%-8p ", static_cast<void *>(row[i]));
            }

            putchar(10);
        }

        printf("the end state set:\n");

        for (auto var : _sendState) {
            printf("%-8p ", static_cast<void *>(var));
        }

        putchar(10);
        puts("----------- end display ------------");

    }

    void minimize() {
        decltype(_sendState) sstart;
        decltype(sstart) send(_sendState.begin(), _sendState.end());

        for (auto var : _listData) {
            if (send.find(var) == send.end()) {
                sstart.insert(var);
            }
        }

        std::set<decltype(sstart)> sdivide;
        decltype(sdivide) stemp;
        stemp.insert(sstart);
        stemp.insert(send);
        unsigned int currentChar = 0;

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
                for (unsigned int i = 0; i < _char_count; ++i) {
                    currentChar = i;
                    // printf("currentChar = %d\n", currentChar);
                    subset.clear();

                    // 遍历当前所有状态，填充 subset 集合
                    for (auto state : divide) {
                        subset.insert(
                            reinterpret_cast<int **>
                            (state[currentChar]));
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
                                    for (auto state : divide) {

                                        if (st.find(reinterpret_cast<iterator_array>(state[currentChar]))
                                                != st.end()) {
                                            temp.insert(state);
                                        }
                                    }

                                    stemp.insert(temp);
                                } else {
                                    for (auto state : divide) {

                                        if (st.find(reinterpret_cast<iterator_array>(state[currentChar])) != st.end()) {
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

        //     for (auto var : s) {
        //         printf("%-8p ", reinterpret_cast<void *>(var));
        //     }

        //     putchar(10);
        // }

        // puts("=========================");
        // fflush(stdout);


        // puts("modify data matrix.");
        // fflush(stdout);
        // display();

        // decltype(_vecData) vecNew;
        // vecNew.clear();

        for (const auto &divid : sdivide) {

            auto it = divid.begin();
            auto nodeID = *it;

            ++it;

            if (it != divid.end()) {
                // start from the second one of the divid
                for (; it != divid.end(); ++it) {

                    if (_sendState.find(*it) != _sendState.end()) {
                        _sendState.erase(*it);
                        _sendState.insert(nodeID);
                    }

                    // find in data matrix;
                    for (auto it_data = _listData.begin(); it_data != _listData
                            .end(); ++it_data) {

                        if (*it == *(it_data)) {
                            delete [] *(it_data);
                            it_data = _listData.erase(it_data);

                            if (it_data == _listData.end()) {
                                break;
                            }
                        }

                        for (unsigned int i = 0; i < _char_count; ++i) {

                            if ((*it_data)[i] == reinterpret_cast<int *>(*it)) {
                                (*it_data)[i] = reinterpret_cast<int *>(nodeID);
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
        // fflush(stdout);

        // printf("\n");

        // for (auto &row : vecNew) {
        //     for (auto &var : row) {
        //         printf("%d ", var);
        //     }

        //     putchar(10);
        // }

        // // printf("start to remove died node\n");
        // // fflush(stdout);

        // 记录死亡的节点

        // bool diedFlag = true;

        // for (auto it = _listData.begin(); it != _listData.end(); ++it) {
        //     auto index = *it;
        //     diedFlag = true;

        //     for (unsigned int i = 0; i < _char_count; ++i) {
        //         if (index[i] != reinterpret_cast<int *>(index)) {
        //             diedFlag = false;
        //             break;
        //         }
        //     }

        //     if (diedFlag) {
        //         // printf("died: %d\n", index);
        //         delete *(it);
        //         it = _listData.erase(it);
        //     }
        // }
    }

    std::string tag() {
        return _tag;
    }

    void tag(const std::string &tag) {
        _tag = tag;
    }

  private:
    std::array<int, CHAR_CNT> _char_hash;
    unsigned int _char_count = 0;
    std::vector<iterator_array> _vec_list_index;
    // std::vector<std::vector<unsigned int> > _vecData;
    std::list<int **> _listData;
    std::set<int **> _sendState;
    std::string _tag = "default tag of dfa.";
};

DFA *buildDFA(NFATable &nfa) {
    nfa.update();
    auto chCnt = nfa.schar().size();
    std::array<int, CHAR_CNT> char_hash;
    // the invaild of char hash is -1
    memset(char_hash.data(), -1, char_hash.max_size() * sizeof(char_hash[0]));

    std::list<iterator_array> listData;
    // 统计字符情况
    size_t cnt = 0;

    for (char var : nfa.schar()) {
        char_hash[static_cast<size_t>(var)] = cnt;
        ++cnt;
    }

    // 初始集合
    std::set<size_t> sstart;
    // 当前遍历的集合
    std::set<size_t> scur;
    // 存储终态集合
    std::set<iterator_array> sendState;
    sstart.insert(nfa.index_head());
    // 空闭包
    nfa.getEplisonClosure(sstart);
    // 所有状态
    std::map<std::set<size_t>, size_t> sdstates;
    // 所有未标记状态
    std::queue<std::set<size_t>> qunflag;
    // 未标记状态对应的 ID
    std::queue<size_t> qid;

    cnt = 0;
    sdstates[sstart] = cnt;
    qid.push(cnt);
    qunflag.push(sstart);

    // 记录 list 下标对应的指针
    std::vector<iterator_array> list_index;
    iterator_array arr = new int *[chCnt];
    listData.push_back(arr);
    list_index.push_back(arr);

    int curID = 0;

    while (!qunflag.empty()) {
        // 标记 X
        scur = qunflag.front();
        curID = qid.front();
        // printf("at start, scur size = %d\n", scur.size());
        qunflag.pop();
        qid.pop();

        // for 每一个输入字符 a
        for (char ch : nfa.schar()) {
            std::set<size_t> su = scur;
            nfa.getRouteClosure(ch, su);
            nfa.getEplisonClosure(su);

            if (sdstates.find(su) == sdstates.end()) {
                ++cnt;
                sdstates[su] = cnt;
                qunflag.push(su);
                qid.push(cnt);
                iterator_array arr = new int *[chCnt];
                listData.push_back(arr);
                list_index.push_back(arr);

                if (su.find(nfa.index_tail()) != su.end()) {
                    sendState.insert(arr);
                }
            }

            auto it = listData.begin();

            for (int i = 0; i < curID; ++i) {
                ++it;
            }

            unsigned int a = sdstates[su];
            (*(it))[char_hash[static_cast<size_t>(ch)]] =
                reinterpret_cast<int *>(list_index[a]);
        }
    }

    return new DFA(char_hash, chCnt, list_index, listData, sendState);
}

#endif
