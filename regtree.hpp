#ifndef REGTREE_H_
#define REGTREE_H_

#include <string>

using std::ifstream;
using std::string;

static const int N = 1024;

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
                _backOrderDisplay(root -> _lson);
            }

            if (root -> _rson) {
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
                _preOrderDisplay(root -> _lson);
            }

            if (root -> _rson) {
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
                _middleOrderDisplay(root -> _lson);
            }

            printf("op = %c\n", root -> _ope);

            if (root -> _rson) {
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

void print_error(const char *message) {
    printf("error**: \n");
}

#endif