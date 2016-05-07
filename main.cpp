#include "regtree.hpp"
#include <cstdio>
#include <typeinfo>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <iostream>

using std::cout;
using std::endl;

using std::pair;

class NFANode {
  public:
    NFANode(): _end(true) {}

    NFANode(char tag, NFANode *next, bool end = false): _end(end) {
        _vecNext.push_back(std::move(std::make_pair(tag, next)));
    }

    NFANode(NFANode *next, bool end = false): _end(end) {
        _vecNext.push_back(std::make_pair(0, next));
    }

    // explicit NFANode(bool end): _end(end) {}

    void add(char tag, NFANode *next) {
        _vecNext.push_back(std::make_pair(0, next));
    }

    const std::vector<pair<char, NFANode *>> &children() {
        return _vecNext;
    }

    bool end() {
        return _end;
    }

    NFANode *&next() {
        return _vecNext[0].second;
    }

    void display() {
        printf("cur: ");

        if (_end) {
            printf("end.\n");
            return;
        }

        for (auto &var : _vecNext) {
            printf("%c ", var.first);
            var.second -> display();
        }
    }

    ~NFANode() = default;

  private:
    bool _end;
    std::vector<pair<char, NFANode *>> _vecNext;
    static unsigned int cnt;
};

class NFA {
  public:
    explicit NFA(char tag, NFANode *next): _head(new NFANode(tag, next)),
        _tail(_head) {
    }

    ~NFA() {
        // delete _head;
        std::queue<NFANode *> qnfa;
        std::set<NFANode *> sdel;
        sdel.insert(_head);
        qnfa.push(_head);
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            if (cur == _tail -> next()) {
                continue;
            }

            for (auto &var : cur ->children()) {
                qnfa.push(var.second);

                if (sdel.find(var.second) == sdel.end()) {
                    sdel.insert(var.second);
                }
            }
        }

        for (auto var : sdel) {
            delete var;
        }
    }

    void uion(NFA *other) {
        _tail -> next() = other -> head();
        _tail = other -> tail();
    }

    NFANode *head() {
        return _head;
    }

    NFANode *tail() {
        return _tail;
    }

    void display() {
        std::queue<NFANode *> qnfa;
        qnfa.push(_head);
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            printf("---\n");

            if (cur == _tail -> next()) {
                printf("end.\n");
                continue;
            }

            for (auto &var : cur ->children()) {
                if (var.first) {
                    printf("%c ", var.first);
                } else {
                    printf("|e| ");
                }

                qnfa.push(var.second);
            }

            putchar(10);
        }

        printf("-------display end-------\n");
    }

  private:
    NFANode *_head;
    NFANode *_tail;
};

NFA *_buildNFA(RegTree *root) {
    if (!root) {
        return NULL;
    }

    char tag = 0;
    NFA *left = NULL;
    NFA *right = NULL;

    if (root -> leaf()) {
        tag = root -> data();
        NFANode *next = new NFANode();
        NFA *nfa = new NFA(tag, next);
        return nfa;
    }

    if (root -> lson()) {
        left = _buildNFA(root -> lson());
    }

    if (root -> rson()) {
        right = _buildNFA(root -> rson());
    }

    switch (root -> data()) {
        case '.':
            left -> uion(right);
            break;

        case '|':
            NFANode *p;
            p = new NFANode();
            left -> uion(new NFA(0, p));
            right -> uion(new NFA(0, p));
            NFA *ret;
            ret = new NFA(0, new NFANode());
            ret -> uion(left);
            ret -> head() -> add(0, right -> head());
            return ret;
            break;

        default:
            break;
    }

    return left;
}

NFA *buildNFA(RegTree *root) {
    NFA *nfa = _buildNFA(root);
    return nfa;
}

NFA *inputNFA() {
    ifstream input("input.txt");
    string reg;
    input >> reg;
    input.close();
    RegTree *root = buildRegTree(reg);
    root -> backOrderDisplay();
    return buildNFA(root);
}

int main() {
    freopen("output.txt", "w", stdout);
    NFA *nfa = inputNFA();
    printf("ret = %p\n", static_cast<void *>(nfa));
    fflush(stdout);
    // nfa -> display();
    delete nfa;
    return 0;
}