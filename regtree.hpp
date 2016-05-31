#ifndef REGTREE_H_
#define REGTREE_H_

#include <memory>
#include <string>

using std::ifstream;
using std::string;

static const int N = 1024;

const char OP_CAT = '.';
const char OP_OR = '|';
const char OP_STAR = '*';
const char JUMP_EMPTY = 0;

class RegTree {
  public:
    // 默认构造情况下，该节点为空跳转，默认是叶子结点
    RegTree(): _leaf(true), _ope(JUMP_EMPTY), _lson(NULL), _rson(NULL) {}

    explicit RegTree(char data)
        : _leaf(true),
          _ope(data),
          _lson(NULL),
          _rson(NULL) {

    }

    RegTree(char ope, std::shared_ptr<RegTree> lson, std::shared_ptr<RegTree> rson)
        : _leaf(false),
          _ope(ope),
          _lson(lson),
          _rson(rson) {

    }

    std::shared_ptr<RegTree> lson() {
        return _lson;
    }

    std::shared_ptr<RegTree> lson(RegTree *const &data) {
        _leaf = false;
        _lson.reset(data);
        return _lson;
    }

    std::shared_ptr<RegTree> lson(std::shared_ptr<RegTree> const data) {
        _leaf = false;
        _lson = data;
        return _lson;
    }

    std::shared_ptr<RegTree> rson() {
        return _rson;
    }

    std::shared_ptr<RegTree> rson(RegTree *const &data) {
        _leaf = false;
        _rson.reset(data);
        return _rson;
    }

    std::shared_ptr<RegTree> rson(std::shared_ptr<RegTree> const &data) {
        _leaf = false;
        _rson = data;
        return _rson;
    }

    std::shared_ptr<RegTree> rson(std::shared_ptr<RegTree>  const &&data) {
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

    void _backOrderDisplay(RegTree *root) {

        if (!root) {
            return;
        }

        if (root -> _leaf) {
            printf("data = %c\n", root -> _ope);
        } else {
            if (root -> _lson) {
                _backOrderDisplay(root -> _lson.get());
            }

            if (root -> _rson) {
                _backOrderDisplay(root -> _rson.get());
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

    void _preOrderDisplay(RegTree *root) {

        if (!root) {
            return;
        }

        if (root -> _leaf) {
            printf("data = %c\n", root -> _ope);
        } else {
            printf("op = %c\n", root -> _ope);

            if (root -> _lson) {
                _preOrderDisplay(root -> _lson.get());
            }

            if (root -> _rson) {
                _preOrderDisplay(root -> _rson.get());
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

    void _middleOrderDisplay(RegTree *root) {

        if (!root) {
            return;
        }

        if (root -> _leaf) {
            printf("data = %c\n", root -> _ope);
        } else {
            if (root -> _lson) {
                _middleOrderDisplay(root -> _lson.get());
            }

            printf("op = %c\n", root -> _ope);

            if (root -> _rson) {
                _middleOrderDisplay(root -> _rson.get());
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

  private:
    bool _leaf;
    char _ope;
    std::shared_ptr<RegTree> _lson;
    std::shared_ptr<RegTree> _rson;
};

#endif