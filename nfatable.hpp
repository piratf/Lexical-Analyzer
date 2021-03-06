#ifndef NFATABLE_H_
#define NFATABLE_H_

#include "regtree.hpp"
#include <vector>
#include <array>
#include <set>
#include <queue>
#include <cstring>

class NFATable {
  public:
    NFATable() = default;
    ~NFATable() = default;

    NFATable(char tag) {
        addNode();
        addNode();
        _table[0].push_back(std::make_pair(tag, 1));
    }

    std::vector<std::vector<std::pair<char, size_t> > > table() {
        return _table;
    }

    const std::vector<std::vector<std::pair<char, size_t> > > table() const {
        return _table;
    }

    void addNode() {
        _table.push_back(std::vector<std::pair<char, size_t> >());
    }

    size_t index_tail() {
        return _table.size() - 1;
    }

    size_t index_head() {
        return 0;
    }

    /**
     * 添加一个 nfa 到当前 nfa 中，合并两个表，另外一个 nfa 的内容会被删除
     * @author piratf
     * @param  other 另一个 nfa
     * @return       另一个 nfa 的 head 位置
     */
    size_t add(NFATable &other) {
        size_t base_size = _table.size();

        for (auto &row : other.table()) {
            // 处理行号
            for (auto &pair : row) {
                pair.second += base_size;
            }

            _table.push_back(std::move(row));
        }

        return base_size;
    }

    void update() {
        std::queue<size_t> que;
        // hash visit
        std::unique_ptr<bool[]> visit(new bool[_table.size()]);
        size_t cur = 0;

        _schar.clear();

        memset(visit.get(), 0, _table.size() * sizeof(visit[0]));

        que.push(0);
        visit[0] = true;

        while (!que.empty()) {
            cur = que.front();
            que.pop();
            auto &row = _table[cur];

            for (auto &pair : row) {
                if (_schar.find(pair.first) == _schar.end()) {
                    _schar.insert(pair.first);
                }

                if (!visit[pair.second]) {
                    que.push(pair.second);
                    visit[pair.second] = true;
                }
            }
        }

        // 去除空跳转字符
        _schar.erase(0);
    }

    void display() {
        puts("============= nfa display ===========");
        fflush(stdout);
        printf("size of table = %d\n", _table.size());

        for (auto &row : _table) {
            for (auto &var : row) {
                if (var.first) {
                    printf("%c:%d ", var.first, var.second);
                } else {
                    printf("|E|:%d ", var.second);
                }
            }

            putchar('\n');
        }

        puts("=====================================");
        fflush(stdout);
    }

    std::set<char> schar() {
        return _schar;
    }

    const std::set<char> &schar() const {
        return _schar;
    }

    /**
     * smove 函数
     * @author piratf
     * @param  ch    跳转字符
     * @param  input 输入集合
     */
    void getRouteClosure(char ch, std::set<size_t> &input) {
        std::set<size_t> ret;

        for (auto &node : input) {
            for (auto &var : _table[node]) {
                if (var.first == ch && ret.find(var.second) == ret.end()) {
                    ret.insert(var.second);
                }
            }
        }

        // 交换集合，原来的输入状态被抛弃
        input.swap(ret);
    }

    /**
     * 递归函数，得到空闭包
     * @author piratf
     * @param  index_head 当前函数内起始状态的下标
     * @param  ret        状态集合
     */
    void _getEplisonClosure(size_t index_head, std::set<size_t> &ret) {
        // 遍历邻接表由起始状态所关联的所有状态
        for (auto &var : _table[index_head]) {
            // 如果是空跳转且不在 ret 集合内
            if (!var.first && ret.find(var.second) == ret.end()) {
                ret.insert(var.second);
                // 空闭包会无限递归，直到没有空跳转为止
                _getEplisonClosure(var.second, ret);
            }
        }
    }

    /**
     * 得到空闭包
     * @author piratf
     * @param  input 输入一个状态组成的集合
     */
    void getEplisonClosure(std::set<size_t> &input) {
        // 遍历输入集合内的状态
        for (auto &node : input) {
            _getEplisonClosure(node, input);
        }
    }

    void uion(NFATable &other) {
        _table.pop_back();
        add(other);
    }

    void line(NFATable &lhs, NFATable &rhs) {
        // add head node
        addNode();
        size_t headID = index_tail();
        _table[headID].push_back(std::make_pair(0, add(lhs)));
        size_t lhs_tail = index_tail();
        _table[headID].push_back(std::make_pair(0, add(rhs)));
        size_t rhs_tail = index_tail();
        // add tail node
        addNode();
        _table[lhs_tail].push_back(std::make_pair(0, index_tail()));
        _table[rhs_tail].push_back(std::make_pair(0, index_tail()));
    }

    void star(NFATable &other) {
        addNode();
        size_t headID = index_tail();
        size_t other_head = add(other);
        _table[headID].push_back(std::make_pair(0, other_head));
        size_t other_tail = index_tail();
        _table[other_tail].push_back(std::make_pair(0, other_head));
        addNode();
        _table[other_tail].push_back(std::make_pair(0, index_tail()));
        _table[headID].push_back(std::make_pair(0, index_tail()));
    }

  private:
    // 邻接表
    std::vector<std::vector<std::pair<char, size_t> > > _table;
    // 记录 nfa 的字符集
    std::set<char> _schar;
};

/**
 * 构造 NFA
 * @author piratf
 * @param  root 语法树的根节点
 * @return      NFA 对象
 */
NFATable _buildNFA(std::shared_ptr<RegTree> root) {
    // 空 NFA
    if (root == nullptr) {
        return NFATable();
    }

    char tag = 0;
    NFATable left, right, nfa;

    // 单字符跳转
    if (root -> leaf()) {
        tag = root -> data();
        return NFATable(tag);
    }

    // 后序遍历
    if (root -> lson()) {
        left = _buildNFA(root -> lson());
    }

    if (root -> rson()) {
        right = _buildNFA(root -> rson());
    }

    switch (root -> data()) {
        case '*':
            nfa = NFATable();
            nfa.star(left);
            return nfa;
            break;

        case '.':
            left.uion(right);
            break;

        case '|':
            nfa = NFATable();
            nfa.line(left, right);
            return nfa;

        default:
            break;
    }

    return left;
}

NFATable buildNFA(std::shared_ptr<RegTree> root) {
    return _buildNFA(root);
}

#endif
