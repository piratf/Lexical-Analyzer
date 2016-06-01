#ifndef DFA_H_
#define DFA_H_

#include "pclock.hpp"

using namespace piratf;

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
        std::list<iterator_array> &&list_data,
        std::set<iterator_array> &&s_end_state)
        : _char_hash(char_hash),
          _char_count(char_count),
          _list_data(std::move(list_data)),
          _s_end_state(std::move(s_end_state)) {

    }

    DFA(std::array<int, CHAR_CNT> &char_hash,
        size_t char_count,
        std::list<iterator_array> &list_data,
        std::set<iterator_array> &s_end_state)
        : _char_hash(char_hash),
          _char_count(char_count),
          _list_data(std::move(list_data)),
          _s_end_state(s_end_state) {
    }

    ~DFA() {
        for (auto fuck : _list_data) {
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
        iterator_array s = *_list_data.begin();

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

        if (_s_end_state.find(s) != _s_end_state.end()) {
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

        for (auto &row : _list_data) {

            printf("%-8p: ", reinterpret_cast<void *>(row));

            for (unsigned int i = 0; i < _char_count; ++i) {
                printf("%-8p ", static_cast<void *>(row[i]));
            }

            putchar(10);
        }

        printf("the end state set:\n");

        for (auto var : _s_end_state) {
            printf("%-8p ", static_cast<void *>(var));
        }

        putchar(10);
        puts("----------- end display ------------");

    }

    /**
     * DFA 最小化
     * @author piratf
     */
    void minimize() {
        // 类型推导
        decltype(_s_end_state) sstart;
        // 使用终态集合初始化 send
        decltype(sstart) send(_s_end_state.begin(), _s_end_state.end());

        // 所有内容分成两个集合
        for (auto var : _list_data) {
            if (send.find(var) == send.end()) {
                sstart.insert(var);
            }
        }

        std::set<decltype(sstart)> sdivide;
        decltype(sdivide) stemp;
        stemp.insert(sstart);
        stemp.insert(send);
        unsigned int currentChar = 0;


        while (stemp != sdivide) {
            sdivide.swap(stemp);
            stemp.clear();

            // 判断当前集合是否有变化
            bool endOfMinimize = true;

            // 遍历当前划分中的每一个集合
            for (const auto &divide : sdivide) {

                endOfMinimize = true;
                // 如果当前集合没有变化，就直接添加到 stemp 中

                if (divide.size() == 1) {
                    stemp.insert(divide);
                    continue;
                }
                // 如果当前集合没有变化，就直接添加到 stemp 中

                decltype(sstart) subset;

                // 遍历 _title 中每一个可能的输入字符
                for (unsigned int i = 0; i < _char_count; ++i) {
                    currentChar = i;
                    subset.clear();

                    // 遍历当前所有状态，填充 subset 集合
                    for (auto state : divide) {
                        subset.insert(
                            reinterpret_cast<int **>
                            (state[currentChar]));
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
                            decltype(subset) st;
                            std::set_intersection(subset.begin(), subset.end(), d.begin(), d.end(), std::inserter(st, st.begin()));

                            // 得到一个不为空的交集，添加所有单步结果在此交集内的元素到新的划分中
                            if (!st.empty()) {

                                // 如果集合和外层处理的是同一个集合，那么拆分出来的项可以汇总作为单独的新集合
                                if (d == divide) {
                                    decltype(subset) temp;

                                    for (auto state : divide) {

                                        if (st.find(reinterpret_cast<iterator_array>(state[currentChar]))
                                                != st.end()) {
                                            temp.insert(state);
                                        }
                                    }

                                    stemp.insert(temp);
                                } else {
                                    // 否则，每一个拆分出来的项都要单独作为一个集合
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
                    
                        break;
                    }
                }

                // 如果当前集合没有变化，就直接添加到 stemp 中
                if (endOfMinimize) {
                    stemp.insert(divide);
                }
            }
        }

        // 得到所有集合的状态后，开始处理数据，删除多余行
        for (const auto &divide : sdivide) {

            auto it = divide.begin();
            // 集合的第一个内容作为保留内容，其他的会被删掉
            auto nodeID = *it;

            ++it;

            // 这个判断表示如果 divide 集合中不止一个元素，就继续处理
            if (it != divide.end()) {
                // start from the second one of the divide
                for (; it != divide.end(); ++it) {

                    if (_s_end_state.find(*it) != _s_end_state.end()) {
                        _s_end_state.erase(*it);
                        _s_end_state.insert(nodeID);
                    }

                    // search in data matrix;
                    for (auto it_data = _list_data.begin(); it_data != _list_data
                            .end(); ++it_data) {

                        // 删除对应行
                        if (*it == *(it_data)) {
                            delete [] *(it_data);
                            it_data = _list_data.erase(it_data);

                            if (it_data == _list_data.end()) {
                                break;
                            }
                        }

                        // 如果不是对应行，查看是否有集合内的下标，处理下标
                        for (unsigned int i = 0; i < _char_count; ++i) {

                            if ((*it_data)[i] == reinterpret_cast<int *>(*it)) {
                                (*it_data)[i] = reinterpret_cast<int *>(nodeID);
                            }
                        }
                    }
                }
            }
        }
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
    std::list<int **> _list_data;
    std::set<int **> _s_end_state;
    std::string _tag = "default tag of dfa.";
};

/**
 * 构造 dfa
 * @author piratf
 * @param  nfa nfa 对象
 * @return     DFA 对象的指针
 */
DFA *buildDFA(NFATable &nfa) {
    // update 函数，记录 nfa 除空跳转外的其他字符集
    nfa.update();
    auto char_cnt = nfa.schar().size();
    // 长度固定的 hash 表
    std::array<int, CHAR_CNT> char_hash;
    // 不被包含的字符位置为 -1
    memset(char_hash.data(), -1, char_hash.max_size() * sizeof(char_hash[0]));

    // 统计字符情况
    size_t cnt = 0;

    for (char var : nfa.schar()) {
        // 对每一个字符填不同的值
        char_hash[static_cast<size_t>(var)] = cnt;
        ++cnt;
    }

    // 初始集合
    std::set<size_t> sstart;
    // 当前遍历的集合
    std::set<size_t> scur;
    // 存储终态集合
    std::set<iterator_array> s_end_state;
    sstart.insert(nfa.index_head());
    // 空闭包
    nfa.getEplisonClosure(sstart);
    // 所有状态
    std::map<std::set<size_t>, size_t> sdstates;
    // 所有未标记状态
    std::queue<std::set<size_t>> qunflag;
    // 未标记状态对应的 ID
    std::queue<size_t> qid;

    // 第一个状态集的 ID 为 0
    cnt = 0;
    // 第一个未标记的状态
    sdstates[sstart] = cnt;
    qid.push(cnt);
    qunflag.push(sstart);

    // list 数据容器
    std::list<iterator_array> list_data;
    // 记录 list 下标对应的指针
    std::vector<iterator_array> list_index;
    // 第一个空行
    iterator_array arr = new int *[char_cnt];
    list_data.push_back(arr);
    list_index.push_back(arr);

    int curID = 0;

    while (!qunflag.empty()) {
        // 标记当前状态
        scur = qunflag.front();
        // 当前状态 ID
        curID = qid.front();
        qunflag.pop();
        qid.pop();

        // for 每一个输入字符
        for (char ch : nfa.schar()) {
            // 拷贝状态，准备做 smove + 空闭包 处理
            std::set<size_t> su = scur;
            nfa.getRouteClosure(ch, su);
            nfa.getEplisonClosure(su);

            // 如果没在集合中找到这个状态集
            if (sdstates.find(su) == sdstates.end()) {
                // 标记新的 ID
                ++cnt;
                sdstates[su] = cnt;
                qunflag.push(su);
                qid.push(cnt);
                // 新的行
                iterator_array arr = new int *[char_cnt];
                list_data.push_back(arr);
                list_index.push_back(arr);

                // 如果包含终态，那么将这一行的地址加入到 dfa 的终态集合中
                if (su.find(nfa.index_tail()) != su.end()) {
                    s_end_state.insert(arr);
                }
            }

            auto it = list_data.begin();

            // 移动迭代器到 cur ID 对应的行上
            for (int i = 0; i < curID; ++i) {
                ++it;
            }

            // 建立跳转
            // 先获得目标行的 ID
            size_t a = sdstates[su];
            // 填入 a 对应行的指针
            // list_index 在这里使用，可以根据 ID 在 O(1) 复杂度内取得对应指针，而 list 需要 O(n) 复杂度，所以多建了这个 list_index
            (*(it))[char_hash[static_cast<size_t>(ch)]] =
                reinterpret_cast<int *>(list_index[a]);
        }
    }

    return new DFA(char_hash, char_cnt, list_data, s_end_state);
}

#endif
