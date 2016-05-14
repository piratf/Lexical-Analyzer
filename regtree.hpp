#ifndef REGTREE_H_
#define REGTREE_H_

#ifdef DEBUG
    #include <cstdio>
#endif
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

void print_error(const char *message) {
    printf("error**: \n");
}

RegTree *buildRegTree(const string &reg) {
    RegTree *root = new RegTree();
    RegTree *gp = NULL;
    RegTree *parent = NULL;
    RegTree *p = root;

    for (auto it = reg.rbegin(); it != reg.rend(); ++it) {
        char cur = *it;
        // printf("cur = %c\n", cur);
        fflush(stdout);

        if (cur == '\\') {
            continue;
        }

        if (cur == '|' && *(it + 1) == '\\') {
            it += 2;
            size_t t = reg.rfind('|', reg.rend() - (it));

            // printf("it = %c\n", *it);
            fflush(stdout);
            std::string substr = reg.substr(t + 1, reg.rend() - it - t - 1);
            // printf("substr = %s\n", substr.data());
            fflush(stdout);
            it += substr.size() - 1;
            // 右支为子表达式树，和父节点的右支相对应，因此修改父节点的运算符
            RegTree *r = buildRegTree(substr);
            RegTree *n = root;
            root = new RegTree('|');
            root -> rson(r);
            root -> lson(n);

            if (!gp) {
                gp = root;
            }
        } else if (cur == ')' && *(it + 1) == '\\') {
            it += 2;
            size_t t = reg.rfind("\\(", reg.rend() - (it));
            std::string substr = reg.substr(t + 1, reg.rend() - it - t - 1);
            it += substr.size();
            // 另右支为子表达式树，符号为默认
            p -> rson(buildRegTree(substr));
            p -> data(OP_CAT);
        } else if (cur == '*' && *(it + 1) == '\\') {
            it += 3;
            size_t t = reg.rfind("\\(", reg.rend() - (it));
            std::string substr = reg.substr(t + 1, reg.rend() - it - t - 1);
            it += substr.size();
            // 需要在右支上建立新的 星号运算树
            RegTree *star = new RegTree(OP_STAR);
            star -> rson(new RegTree('*'));
            star -> lson(buildRegTree(substr));
            p -> rson(star);
            p -> data(OP_CAT);
        } else {
            // 内容放到右节点
            p -> rson(new RegTree(cur));
            // 设置运算符
            p -> data(OP_CAT);
        }

        // 左节点向下拓展
        if (p -> data()) {
            p -> lson(new RegTree(0));
            gp = parent;
            parent = p;
            p = p -> lson();
        }
    }

    // 运行到 reg 结尾时，需要将 parent 的 rson 挂到 gp 的 lson 上
    if (!p -> data()) {
        RegTree *r = parent -> rson();

        if (!gp) {
            delete root;
            root = r;
        } else {
            delete gp -> lson();
            gp -> lson(r);
        }
    }

    return root;
}

#endif