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

    std::vector<pair<char, NFANode *>> &children() {
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
        _tail(next) {
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

            if (cur == _tail) {
                continue;
            }

            for (auto &var : cur ->children()) {

                if (sdel.find(var.second) != sdel.end()) {
                    continue;
                } else {
                    qnfa.push(var.second);
                    sdel.insert(var.second);
                }
            }
        }

        for (auto var : sdel) {
            delete var;
        }
    }

    void uion(NFA *other) {
        printf("-----------uion--------------\n");
        std::queue<NFANode *> qnfa;
        std::set<NFANode *>svisit;
        svisit.insert(_head);
        qnfa.push(_head);
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            if (cur == _tail) {
                continue;
            }

            if (!cur -> children().empty()) {
                for (auto &var : cur -> children()) {
                    if (var.second == _tail) {
                        // printf("secode !\n");
                        var.second = other -> head();
                    }
                }
            }

            for (auto &var : cur ->children()) {
                // if (var.first) {
                //     printf("%c\n", var.first);
                // } else {
                //     printf("|e|\n");
                // }

                if (!var.second -> children().empty()) {
                    for (auto &temp : var.second -> children()) {
                        if (temp.second == _tail) {
                            // printf("secode !\n");
                            temp.second = other -> head();
                        }
                    }
                }

                if (var.second == _tail) {
                    // printf("secode = tail = %p\n", static_cast<void *>(var.second));
                } else {
                    if (svisit.find(var.second) != svisit.end()) {
                        continue;
                    } else {
                        svisit.insert(var.second);
                        qnfa.push(var.second);
                    }
                }

            }
        }

        // _tail = other -> head();
        _tail = other -> tail();

        printf("tail = %p\n", static_cast<void *>(_tail));
        printf("-----------uion end----------\n");
    }

    NFANode *head() {
        return _head;
    }

    NFANode *tail() {
        return _tail;
    }

    void tail(NFANode *const tail) {
        _tail = tail;
    }

    void getEplisonClosure(std::set<NFANode *> &input) {

        for (auto &node : input) {
            printf("eplison node: %p\n", static_cast<void *>(node));
            getEplisonClosure(node, input);
        }

    }

    void getEplisonClosure(NFANode *head, std::set<NFANode *> &ret) {
        // ret.erase(head);
        for (auto &var : head -> children()) {
            if (!var.first && ret.find(var.second) == ret.end()) {
                ret.insert(var.second);
                getEplisonClosure(var.second, ret);
            }
        }

        printf("end ---\n");
    }

    void getRouteClosure(NFANode *head, char ch, std::set<NFANode *> &ret) {
        // ret.erase(head);
        printf("%c\n", ch);

        for (auto &var : head -> children()) {
            if (var.first == ch && ret.find(var.second) == ret.end()) {
                printf("var.first: %c\n", var.first);
                ret.insert(var.second);
            }
        }

        printf("end ---\n");
    }

    void calculate(char ch, std::set<NFANode *> &input) {
        std::set<NFANode *> ret;
        int i = 0;

        for (auto &node : input) {
            printf("i = %d\n", i++);
            printf("calculate node: %p\n", static_cast<void *>(node));
            getRouteClosure(node, ch, ret);
        }

        input.swap(ret);
    }

    bool calculate(const char *input) {
        const char *pch = input;
        std::set<NFANode *> cur;
        cur.insert(_head);
        getEplisonClosure(cur);
        printf("eplison.size() = %d\n", cur.size());

        while (*pch) {
            calculate(*pch, cur);
            printf("cur.size() = %d\n", cur.size());
            getEplisonClosure(cur);
            printf("eplison.size() = %d\n", cur.size());
            ++pch;
        }

        if (cur.find(_tail) != cur.end()) {
            return true;
        } else {
            return false;
        }
    }

    void display() {
        std::queue<NFANode *> qnfa;
        std::set<NFANode *> svisit;
        qnfa.push(_head);
        svisit.insert(_head);
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            printf("---\n");

            if (cur == _tail) {
                printf("end.\n");
                continue;
            }

            printf("num = %d\n", cur -> children().size());
            printf("cur = %p\n", static_cast<void *>(cur));
            fflush(stdout);

            for (auto &var : cur ->children()) {

                if (var.first) {
                    printf("%c ", var.first);
                } else {
                    printf("|e| ");
                }

                printf("next: %p\n", static_cast<void *>(var.second));
                fflush(stdout);

                if (svisit.find(var.second) != svisit.end()) {
                    continue;
                } else {
                    qnfa.push(var.second);
                    svisit.insert(var.second);
                }
            }

            putchar(10);
        }

        printf("tail = %p\n", static_cast<void *>(_tail));
        printf("-------display end-------\n");
        fflush(stdout);
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
        case '*':
            printf("case star.\n");
            NFANode *head, *tail;
            NFA *ret;
            NFANode *p;

            head = left -> head();
            tail = left -> tail();

            // printf("%d\n", tail -> children().size());

            tail -> add(0, head);
            p = new NFANode();
            tail -> add(0, p);
            left -> tail(p);

            // printf("%d\n", tail -> children().size());


            ret = new NFA(0, new NFANode());
            ret -> uion(left);
            ret -> head() -> add(0, left -> tail());
            ret -> tail(left -> tail());
            // NFANode *p;
            // p = new NFANode();
            // printf("%d\n", tail -> children().size());
            return ret;
            fflush(stdout);
            break;

        case '.':
            printf("case cat.\n");
            left -> uion(right);
            // left -> display();
            break;

        case '|':
            p = new NFANode();
            left -> uion(new NFA(0, p));
            right -> uion(new NFA(0, p));
            // left -> display();
            // right -> display();
            // printf("left -> tail %p\n", static_cast<void *>(left -> tail()));
            // printf("right -> tail %p\n", static_cast<void *>(right -> tail()));
            ret = new NFA(0, new NFANode());
            ret -> uion(left);
            // printf("ret -> tail %p\n", static_cast<void *>(ret -> tail()));
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
    test(nfa);
    delete nfa;
    return 0;
}