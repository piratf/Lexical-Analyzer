#include "regtree.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <queue>

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

    const std::vector<pair<char, NFANode *>> &next() {
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

    ~NFANode() {
        for (auto &var : _vecNext) {
            var.second -> ~NFANode();
            delete var.second;
        }
    }

  private:
    bool _end;
    std::vector<pair<char, NFANode *>> _vecNext;
    static unsigned int cnt;
};

class NFA {
  public:
    NFA() {
        _head = new NFANode();
    };

    NFA(NFANode *head): _head(head) {}

    ~NFA() {
        delete _head;
    }

    void display() {
        std::queue<NFANode *> qnfa;
        qnfa.push(_head);
        NFANode *cur = NULL;
        while (!qnfa.empty()) {
            cur = qnfa.front();
            qnfa.pop();
            if (cur -> end()) {
                printf("end.\n");
                continue;
            }
            for (auto &var : cur ->next()) {
                printf("%c ", var.first);
                qnfa.push(var.second);
            }
            putchar(10);
        }
    }

  private:
    NFANode *_head;
};

NFANode *_buildNFA(RegTree *root) {
    if (!root) {
        return NULL;
    }

    char first = 0;
    char tag = 0;
    NFANode *next = NULL;
    NFANode *cur = NULL;

    if (root -> lson()) {
        if (root -> lson() -> leaf()) {
            first = root -> lson() -> data();
        } else {
            next = _buildNFA(root -> lson());
        }
    }

    if (root -> rson()) {
        if (root -> rson() -> leaf()) {
            tag = root -> rson() -> data();
        } else {
            next = _buildNFA(root -> rson());
        }
    }

    if (first) {
        NFANode *end = new NFANode();
        next = new NFANode(tag, end);
        cur = new NFANode(first, next);
        return cur;
    }

    cur = new NFANode(tag, next);
    // printf("op = %c\n", root -> data());

    return cur;
}

NFA *buildNFA(RegTree *root) {
    NFANode *nfa = _buildNFA(root);

    return new NFA(nfa);
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
    NFA *nfa = inputNFA();
    nfa -> display();

    return 0;
}