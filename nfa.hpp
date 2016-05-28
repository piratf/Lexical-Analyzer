#ifndef NFA_H_
#define NFA_H_

#include "regtree.hpp"
#include <cstdio>
#include <vector>
#include <queue>
#include <set>

using std::pair;

class NFANode {
  public:
    NFANode(): _end(true) {}

    NFANode(char tag, std::shared_ptr<NFANode> next, bool end = false): _end(end) {
        _vecNext.push_back(std::make_pair(tag, next));
    }

    // NFANode(NFANode *next, bool end = false): _end(end) {
    //     _vecNext.push_back(std::make_pair(0, std::shared_ptr<NFANode>(next)));
    // }

    // void add(char tag, NFANode *next) {
    //     _vecNext.push_back(std::make_pair(tag, std::shared_ptr<NFANode>(next)));
    // }

    void add(const char tag, const std::shared_ptr<NFANode> &next) {
        _vecNext.push_back(std::make_pair(tag, next));
    }

    std::vector<pair<char, std::shared_ptr<NFANode>>> &children() {
        return _vecNext;
    }

    const std::vector<pair<char, std::shared_ptr<NFANode>>> &children() const {
        return _vecNext;
    }

    bool end() {
        return _end;
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
    std::vector<pair<char, std::shared_ptr<NFANode> >> _vecNext;
    static unsigned int cnt;
};

class NFA {
  public:
    explicit NFA(char tag, std::shared_ptr<NFANode> next) {
        _head = std::make_shared<NFANode>(NFANode(tag, next));
        _tail = next;
    }

    ~NFA() {
        // delete _head;
        std::queue<NFANode *> qnfa;
        std::set<NFANode *> sdel;
        sdel.insert(head());
        qnfa.push(head());
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            if (cur == _tail.get()) {
                continue;
            }

            for (auto &var : cur ->children()) {

                if (sdel.find(var.second.get()) != sdel.end()) {
                    continue;
                } else {
                    qnfa.push(var.second.get());
                    sdel.insert(var.second.get());
                }
            }
        }

        for (auto var : sdel) {
            delete var;
        }
    }

    void uion(std::shared_ptr<NFA> other) {
        std::queue<NFANode *> qnfa;
        std::set<NFANode *>svisit;
        svisit.insert(head());
        qnfa.push(head());
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            if (cur == tail()) {
                continue;
            }

            if (!cur -> children().empty()) {
                for (auto &var : cur -> children()) {
                    if (!var.second -> children().empty()) {
                        for (auto &temp : var.second -> children()) {
                            if (temp.second == _tail) {
                                // printf("secode !\n");
                                temp.second = other -> shared_head();
                            }
                        }
                    }

                    if (var.second == _tail) {
                        // printf("secode !\n");
                        var.second = other -> shared_head();
                    } else if (svisit.find(var.second.get()) == svisit.end()) {
                        svisit.insert(var.second.get());
                        qnfa.push(var.second.get());
                    }
                }
            }
        }

        // printf("tail = %p\n", static_cast<void *>(_tail));
        // fflush(stdout);

        // if (_tail) {
        //     delete _tail;
        // }

        _tail = other -> shared_tail();
    }

    std::shared_ptr<NFANode> & shared_head() {
        return _head;
    }

    std::shared_ptr<NFANode> & shared_tail() {
        return _tail;
    }

    void shared_head(std::shared_ptr<NFANode> &other) {
        _head = other;
    }

    void shared_tail(std::shared_ptr<NFANode> &other) {
        _tail = other;
    }

    NFANode *head() {
        return _head.get();
    }

    NFANode *tail() {
        return _tail.get();
    }

    // void tail(NFANode *const tail) {
    //     _tail.reset(tail);
    // }

    void getEplisonClosure(std::set<NFANode *> &input) {

        for (auto &node : input) {
            getEplisonClosure(node, input);
        }

    }

    // 得到空闭包
    void getEplisonClosure(NFANode *head, std::set<NFANode *> &ret) {
        for (auto &var : head -> children()) {
            if (!var.first && ret.find(var.second.get()) == ret.end()) {
                ret.insert(var.second.get());
                getEplisonClosure(var.second.get(), ret);
            }
        }

    }

    // 得到对指定字符的闭包
    void _getRouteClosure(const NFANode *head, char ch, std::set<NFANode *> &ret) {
        for (auto &var : head -> children()) {
            if (var.first == ch && ret.find(var.second.get()) == ret.end()) {
                ret.insert(var.second.get());
            }
        }

    }

    // 得到对指定字符的闭包
    void getRouteClosure(char ch, std::set<NFANode *> &input) {
        std::set<NFANode *> ret;

        for (const NFANode * const &node : input) {
            _getRouteClosure(node, ch, ret);
        }

        input.swap(ret);
    }

    bool calculate(const char *input) {
        const char *pch = input;
        std::set<NFANode *> cur;
        cur.insert(_head.get());
        getEplisonClosure(cur);

        while (*pch) {
            getRouteClosure(*pch, cur);
            getEplisonClosure(cur);
            ++pch;
        }

        if (cur.find(_tail.get()) != cur.end()) {
            return true;
        } else {
            return false;
        }
    }

    void update() {
        std::queue<NFANode *> qnfa;
        std::set<NFANode *> svisit;
        _schar.clear();

        qnfa.push(head());
        svisit.insert(head());

        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            if (cur == tail()) {
                continue;
            }

            for (auto &var : cur ->children()) {

                if (_schar.find(var.first) == _schar.end()) {
                    _schar.insert(var.first);
                }

                if (svisit.find(var.second.get()) != svisit.end()) {
                    continue;
                } else {
                    qnfa.push(var.second.get());
                    svisit.insert(var.second.get());
                }
            }
        }

        _nodeCount = svisit.size();
        _schar.erase(0);
    }

    std::set<char> schar() {
        return _schar;
    }

    unsigned int nodeCount() {
        return _nodeCount;
    }

    void display() {
        std::queue<NFANode *> qnfa;
        std::set<NFANode *> svisit;
        qnfa.push(head());
        svisit.insert(head());
        NFANode *cur = NULL;

        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();

            printf("---\n");

            if (cur == tail()) {
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

                printf("next: %p\n", static_cast<void *>(var.second.get()));
                fflush(stdout);

                if (svisit.find(var.second.get()) != svisit.end()) {
                    continue;
                } else {
                    qnfa.push(var.second.get());
                    svisit.insert(var.second.get());
                }
            }

            putchar(10);
        }

        printf("tail = %p\n", static_cast<void *>(tail()));
        printf("-------display end-------\n");
        fflush(stdout);
    }

  private:
    std::shared_ptr<NFANode> _head;
    std::shared_ptr<NFANode> _tail;
    unsigned int _nodeCount;
    std::set<char> _schar;
};

std::shared_ptr<NFA> _buildNFA(std::shared_ptr<RegTree> root) {
    if (!root) {
        return NULL;
    }

    char tag = 0;
    std::shared_ptr<NFA> left;
    std::shared_ptr<NFA> right;

    if (root -> leaf()) {
        tag = root -> data();
        return std::make_shared<NFA>(NFA(tag, std::make_shared<NFANode>(NFANode())));
    }

    if (root -> lson()) {
        left = _buildNFA(root -> lson());
    }

    if (root -> rson()) {
        right = _buildNFA(root -> rson());
    }

    std::shared_ptr<NFANode> head, tail, p;
    std::shared_ptr<NFA> ret;

    switch (root -> data()) {
        case '*':
            // printf("case star.\n");
            head = left -> shared_head();
            tail = left -> shared_tail();

            // printf("%d\n", tail -> children().size());

            tail -> add(0, head);
            p = std::make_shared<NFANode>(NFANode());
            tail -> add(0, p);
            left -> shared_tail(p);

            // printf("%d\n", tail -> children().size());


            ret = std::make_shared<NFA>(NFA(0, left -> shared_head()));
            // ret -> uion(left);
            ret -> head() -> add(0, left -> shared_tail());
            ret -> shared_tail(left -> shared_tail());
            // NFANode *p;
            // p = new NFANode();
            // printf("%d\n", tail -> children().size());
            // fflush(stdout);
            return ret;
            break;

        case '.':
            // printf("case cat.\n");
            left -> uion(right);
            // left -> display();
            break;

        case '|':
            p = std::make_shared<NFANode>(NFANode());
            left -> uion(std::make_shared<NFA>(NFA(0, p)));
            right -> uion(std::make_shared<NFA>(NFA(0, p)));
            // left -> display();
            // right -> display();
            // printf("left -> tail %p\n", static_cast<void *>(left -> tail()));
            // printf("right -> tail %p\n", static_cast<void *>(right -> tail()));
            ret = std::make_shared<NFA>(NFA(0, left -> shared_head()));
            ret -> shared_tail(left -> shared_tail());
            // ret -> uion(left);
            // printf("ret -> tail %p\n", static_cast<void *>(ret -> tail()));
            ret -> head() -> add(0, right -> shared_head());
            return ret;
            break;

        default:
            break;
    }

    return left;
}

std::shared_ptr<NFA> buildNFA(std::shared_ptr<RegTree> root) {
    std::shared_ptr<NFA> nfa = _buildNFA(root);
    return nfa;
}

#endif
