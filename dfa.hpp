#ifndef DFA_H_
#define DFA_H_

#include "nfatable.hpp"
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

    DFA(std::array<int, CHAR_CNT> &char_hash,
        std::vector<std::vector<size_t> > &vec_data,
        std::set<size_t> &set_end_state)
        : _char_hash(char_hash),
          _vec_data(std::move(vec_data)),
          _set_end_state(std::move(set_end_state)) {

    }

    size_t move(size_t state, char title) {
        if (_char_hash[static_cast<size_t>(title)] == -1) {
            return -1;
        }

        if (state >= _vec_data.size() || state < 0) {
            return state;
        }

        return _vec_data[state][_char_hash[static_cast<size_t>(title)]];
    }

    bool calculate(const char *str) {
        size_t s = 0;

        const char *p = str;

        while (*p) {
            s = move(s, *p);

            if (s == static_cast<size_t>(-1)) {
                return false;
            }

            ++p;
        }

        if (_set_end_state.find(s) != _set_end_state.end()) {
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

        int rowid = 0;

        for (auto &row : _vec_data) {

            printf("%-2d: ", rowid++);

            for (size_t var : row) {
                printf("%-2d ", var);
            }

            putchar(10);
        }

        printf("the end state set:\n");

        for (size_t var : _set_end_state) {
            printf("%-2d ", var);
        }

        putchar(10);
        puts("----------- end display ------------");

    }

    void minimize() {

    }

    std::string tag() {
        return _tag;
    }

    void tag(const std::string &tag) {
        _tag = tag;
    }

    ~DFA() = default;

  private:
    std::array<int, CHAR_CNT> _char_hash;
    std::vector<std::vector<size_t> > _vec_data;
    std::set<size_t> _set_end_state;
    std::string _tag = "default tag of dfa.";
};

DFA* buildDFA(NFATable &nfa) {
    nfa.update();
    auto chCnt = nfa.schar().size();
    std::array<int, CHAR_CNT> char_hash;
    memset(char_hash.data(), -1, char_hash.max_size() * sizeof(char_hash[0]));

    size_t cnt = 0;

    for (char var : nfa.schar()) {
        char_hash[static_cast<size_t>(var)] = cnt;
        ++cnt;
    }

    std::set<size_t> sstart;
    std::set<size_t> scur;
    std::set<size_t> set_end_state;

    // get the initial eplison closure
    sstart.insert(nfa.index_head());
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

    std::vector<std::vector<size_t> > vec_data;
    vec_data.push_back(std::vector<size_t>(chCnt));

    int curID = 0;

    while (!qunflag.empty()) {
        // 标记 X
        scur = qunflag.front();
        curID = qid.front();
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
                vec_data.push_back(std::vector<size_t>(chCnt));

                if (su.find(nfa.index_tail()) != su.end()) {
                    set_end_state.insert(cnt);
                }
            }

            vec_data[curID][char_hash[static_cast<size_t>(ch)]] = sdstates[su];
        }
    }

    return new DFA(char_hash, vec_data, set_end_state);
}

#endif
