#ifndef REGTREE_H_
#define REGTREE_H_

#ifdef DEBUG
    #include <cstdio>
#endif
#include <string>

using std::ifstream;
using std::string;

const int N = 1024;

enum Operator {
    EMPTY = 0,
    CAT = '.',
    OR = '|',
    STAR = '*',
};

const char OP_CAT = '.';
const char OP_OR = '|';
const char OP_STAR = '*';
const char OP_EMPTY = 0;

class RegTree {
  public:
    RegTree(): _leaf(false), _ope(OP_EMPTY), _lson(NULL), _rson(NULL) {}

    RegTree(char data)
        : _leaf(true),
          _ope(data),
          _lson(NULL),
          _rson(NULL) {

    }


    RegTree(char ope, RegTree *lson, RegTree *rson)
        : _leaf(false),
          _ope(ope),
          _lson(lson),
          _rson(rson) {

    }

    RegTree *lson() {
        return _lson;
    }

    RegTree *lson(RegTree *const data) {
        _leaf = false;
        _lson = data;
        return _lson;
    }

    RegTree *rson() {
        return _rson;
    }

    RegTree *rson( RegTree *const &data) {
#ifdef DEBUG
        printf("modify rson with lvalue\n");
#endif
        _leaf = false;
        _rson = data;
        return _rson;
    }

    RegTree *rson( RegTree * const &&data) {
        _leaf = false;
        _rson = std::move(data);
        return _rson;
    }

    char data(const char data) {
        _ope = data;
        return _ope;
    }

    char data() {
        return _ope;
    }

    bool leaf() const {
        return _leaf;
    }

    bool leaf() {
        return _leaf;
    }

    void _backOrderDisplay(const RegTree *root) {
        // printf("root = %p\n", static_cast<void *>(const_cast<RegTree *>(root)));

        if (!root) {
            return;
        }

        if (root -> _leaf) {
            printf("data = %c\n", root -> _ope);
        } else {
            if (root -> _lson) {
#ifdef DEBUG
                printf("to left\n");
#endif
                _backOrderDisplay(root -> _lson);
            }

            if (root -> _rson) {
#ifdef DEBUG
                printf("to right\n");
#endif
                _backOrderDisplay(root -> _rson);
            }

            printf("op = %c\n", root -> _ope);
        }
    }

    void backOrderDisplay() {
        backOrderDisplay(this);
    }

    void backOrderDisplay(RegTree *other) {
        _backOrderDisplay(other);
        printf("------------------------\n");
    }

    void _preOrderDisplay(const RegTree *root) {
        // printf("root = %p\n", static_cast<void *>(const_cast<RegTree *>(root)));

        if (!root) {
            return;
        }

        if (root -> _leaf) {
            printf("data = %c\n", root -> _ope);
        } else {
            printf("op = %c\n", root -> _ope);

            if (root -> _lson) {
#ifdef DEBUG
                printf("to left\n");
#endif
                _preOrderDisplay(root -> _lson);
            }

            if (root -> _rson) {
#ifdef DEBUG
                printf("to right\n");
#endif
                _preOrderDisplay(root -> _rson);
            }
        }
    }

    void preOrderDisplay() {
        preOrderDisplay(this);
    }

    void preOrderDisplay(RegTree *other) {
        _preOrderDisplay(other);
        printf("------------------------\n");
    }

    void _middleOrderDisplay(const RegTree *root) {
        // printf("root = %p\n", static_cast<void *>(const_cast<RegTree *>(root)));

        if (!root) {
            return;
        }

        if (root -> _leaf) {
            printf("data = %c\n", root -> _ope);
        } else {
            if (root -> _lson) {
#ifdef DEBUG
                printf("to left\n");
#endif
                _middleOrderDisplay(root -> _lson);
            }

            printf("op = %c\n", root -> _ope);

            if (root -> _rson) {
#ifdef DEBUG
                printf("to right\n");
#endif
                _middleOrderDisplay(root -> _rson);
            }
        }
    }

    void middleOrderDisplay() {
        middleOrderDisplay(this);
    }

    void middleOrderDisplay(RegTree *other) {
        _middleOrderDisplay(other);
        printf("------------------------\n");
    }

    ~RegTree() = default;

    // private:
    bool _leaf;
    char _ope;
    RegTree *_lson;
    RegTree *_rson;
};

RegTree *buildRegTree(const string &reg) {
#ifdef DEBUG
    printf("reg = %s\n", reg.data());
#endif
    RegTree *root = new RegTree();
    RegTree *parent = NULL;
    RegTree *gp = NULL;
    RegTree *p = root;

    for (auto it = reg.rbegin(); it != reg.rend(); ++it) {
        char cur = *it;
#ifdef DEBUG
        printf("cur = %c\n", cur);
#endif

        if (*it == '(') {
            printf("error**: missing symmetric (");
            return NULL;
        }

        if (*it == ')') {

            size_t t = reg.rfind('(', reg.rend() - (it + 1) - 1);

            if (t == string::npos) {
                printf("error**: missing symmetric (");
                return NULL;
            } else {
                t += 1;
                string temp = reg.substr(t, reg.rend() - (it + 1) - t);
                it = reg.rend() - t;

                if (parent && !parent -> leaf()) {
                    parent -> data(OP_CAT);
                }

                RegTree *r = buildRegTree(temp);
                p -> rson(r);
                p -> lson(new RegTree());
                gp = parent;
                parent = p;

                p = p -> lson();
            }

            continue;
        }

        if (cur == '|') {

#ifdef DEBUG
            printf("||||||||\n");
#endif

            if (!p -> leaf() && !p -> lson() && !p -> rson()) {
                char data = parent -> rson() -> data();
                delete parent;
                parent = new RegTree(data);
            }

            size_t t = reg.rfind('|', reg.rend() - (it + 2));

            if (t == string::npos) {
#ifdef DEBUG
                printf("this is the last |\n");
#endif
                p = root;
                root = new RegTree('|');
                root -> rson(p);
                root -> lson(buildRegTree(reg.substr(0, reg.rend() - (it + 1))));
#ifdef DEBUG
                printf("t == string::npos: \n");
                printf("the reg = %s\n", reg.data());
                root -> backOrderDisplay();
#endif
                return root;
            } else {
#ifdef DEBUG
                printf("this is not the last |\n");
#endif
                t += 1;
                p = root;
                root = new RegTree('|');
                root -> rson(p);
                root -> lson(buildRegTree(reg.substr(t, reg.rend() - (it + 1) - t)));
                it = reg.rend() - t - 1;
            }

            continue;
        }


        if (parent && !parent -> leaf()) {

#ifdef DEBUG
                printf("add star to parent\n");
#endif
            if (cur == '*') {
                parent -> data(OP_STAR);
            } else {

#ifdef DEBUG
                printf("add cat to parent\n");
#endif
                parent -> data(OP_CAT);

            }
        }

        p -> rson(new RegTree(cur));
        p -> lson(new RegTree());
        gp = parent;
        parent = p;

        p = p -> lson();
    }

    if (!p -> leaf() && !p -> lson() && !p -> rson()) {
        RegTree *r = parent -> rson();

        if (gp) {
#ifdef DEBUG
            printf("modify gp.\n");
#endif
            delete gp -> lson();
            gp -> lson(r);
        } else {
#ifdef DEBUG
            printf("modify root.\n");
#endif
            delete root;
            root = r;
        }
    }

#ifdef DEBUG
    printf("before return: \n");
    root -> backOrderDisplay();
#endif
    return root;
}

#endif