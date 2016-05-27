#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include "LexicalAnalyzer.hpp"
#include <string>
#include <map>

class Preprocessor {
  public:
    Preprocessor() {
        _set_op.insert('|');
        _set_op.insert(')');
        _set_op.insert('(');
        _set_op.insert('[');
        _set_op.insert(']');
        _set_op.insert('*');
        _set_op.insert('?');
    }

    ~Preprocessor() = default;

    void inline trim(std::string &reg) {
        while (reg.back() == ' ') {
            reg.pop_back();
        }
    }

    std::string split(std::string &tag) {
        auto it = tag.begin();

        for (; it != tag.end(); ++it) {
            if (*it == '=') {
                break;
            }
        }

        auto it2 = it + 1;

        while (*it2 == ' ') {
            ++it2;
        }

        // printf("reg2 = %s\n", tag.substr(it2 - tag.begin(), tag.end() - it2).data());

        std::string reg(std::move(tag.substr(it2 - tag.begin(), tag.end() - it2).data()));
        it2 = it - 1;

        while (*it2 == ' ') {
            --it2;
        }

        tag.erase(it2 + 1, tag.end());
        return std::move(reg);
    }

    bool standalone(const std::string &str, const std::string::iterator &it) {
        return (((it - 1 == str.begin()) || (*(it - 1) == '|'))
                && ((it + 1 == str.end()) || (*(it + 1) == '|'))) || ((*(it - 1) == '(') && (*(it + 1) == ')'));
    }

    void update(std::string &tag) {

        std::string reg = split(tag);
        trim(tag);
        trim(reg);

        if (tag[0] == ':') {
            tag = tag.substr(1);
        } else {
            _toDFA_tags.push_back(tag);
        }

        printf("tag = %s\n", tag.data());
        printf("reg = %s\n", reg.data());

        std::string strContent;

        for (auto it = reg.begin(); it != reg.end(); ++it) {

            if (*it == '\\') {
                ++it;
                strContent.push_back(*it);
                continue;
            }

            // 处理 [ ] 括号中的语法糖
            if (*it == '[' && !standalone(reg, it)) {
                size_t right = reg.find(']', it - reg.begin());

                // 如果中括号不成对
                if (right == std::string::npos) {
                    error_shoot("error reg. missing ] ");
                }

                // 构造子字符串 得到括号中的内容
                std::string subreg(it + 1, reg.begin() + right);

                // 处理连续字符缩写语法糖
                if (subreg.size() == 3 && subreg[1] == '-') {
                    char startchar = subreg[0], endchar = subreg[2];
                    subreg.clear();

                    for (char i = startchar; i < endchar; ++i) {
                        subreg.push_back(i);
                        subreg.append("\\|");
                    }

                    subreg.push_back(endchar);
                    strContent.append(subreg);
                    it = reg.begin() + right;
                    continue;
                }

            }

            if (!standalone(reg, it) && _set_op.find(*it) != _set_op.end()) {
                strContent.push_back('\\');
            }

            strContent.push_back(*it);
        }

        _vecRegs.push_back(make_pair(tag, strContent));
        _regs[tag] = strContent;
    }

    std::map<std::string, std::string> &regs() {
        return _regs;
    }

    void display() {
        puts("-------- preprosessor display ----------");
        printf("size = %d\n", _vecRegs.size());

        for (auto &var : _vecRegs) {
            printf("tag = %s\n", var.first.data());
            printf("reg = %s\n", var.second.data());
        }

        puts("----- to dfa -----");

        for (const std::basic_string<char> &var : _toDFA_tags) {
            printf("%s ", var.data());
        }

        putchar(10);

        puts("-------- end display -------------------");
    }

    LexicalAnalyzer buildLA() {
        LexicalAnalyzer la;

        for (auto &var : _vecRegs) {
            _regTrees[var.first] = buildRegTree(var.second);
        }

        for (auto &tag : _toDFA_tags) {
            NFA *nfa = buildNFA(_regTrees[tag]);
            // _regTrees[tag] -> backOrderDisplay();
            // _regTrees[tag] -> middleOrderDisplay();
            DFA *dfa = buildDFA(nfa);
            dfa -> tag(tag);
            // dfa -> display();
            // fflush(stdout);
            dfa -> minimize();
            // printf("tag = %s\n", tag.data());
            // fflush(stdout);
            la.add(dfa);

            if (tag == "operator") {
                la.separator(dfa);
            }

        }

        return la;
    }

    std::string findChildReg(const std::string &reg, size_t pos) {
        size_t it = pos;
        int cnt = 0;

        while (it >= 0 && it < reg.size()) {
            // printf(" it = %c\n", reg[it]);

            if (reg[it] == ')' && reg[it - 1] == '\\') {
                ++cnt;
            }

            if (reg[it] == '(' && reg[it - 1] == '\\') {
                if (cnt) {
                    --cnt;
                } else {
                    break;
                }
            }

            --it;
        }

        return std::string(reg.data() + it + 1, pos - it - 1);
    }

    std::shared_ptr<RegTree> buildRegTree(const string &reg) {
        std::shared_ptr<RegTree> root(new RegTree());
        std::shared_ptr<RegTree> gp = NULL;
        std::shared_ptr<RegTree> parent = NULL;
        std::shared_ptr<RegTree> p = root;

        // printf("reg = %s\n", reg.data());
        // printf("rbegin = %c\n", *reg.rbegin());

        for (auto it = reg.rbegin(); it != reg.rend(); ++it) {
            char cur = *it;
            // printf("cur = %c\n", cur);
            // fflush(stdout);

            if (cur == '\\' && (it + 1 != reg.rend() &&  *(it + 1) != '\\')) {
                continue;
            }

            // 子表达式树重用
            if (cur == ']' && *(it + 1) == '\\') {
                it += 2;
                size_t t = reg.rfind("\\[", reg.rend() - (it));
                std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
                // printf("substr = %s\n", substr.data());
                // fflush(stdout);
                // _regTrees[substr] -> middleOrderDisplay();
                p -> rson(_regTrees[substr]);
                p -> data(OP_CAT);
                it += substr.size() + 1;
            } else if (cur == '|' && *(it + 1) == '\\') {
                // fflush(stdout);
                std::string substr = reg.substr(0, reg.rend() - it - 2);
                // printf("substr = %s\n", substr.data());
                // fflush(stdout);
                it = reg.rend() - 1;
                // printf("it = %c\n", *it);
                // 右支为子表达式树，和父节点的右支相对应，因此修改父节点的运算符
                std::shared_ptr<RegTree> r(buildRegTree(substr));
                std::shared_ptr<RegTree> n = root;
                root.reset(new RegTree('|'));
                root -> rson(r);
                root -> lson(n);

                if (!gp) {
                    gp = root;
                }
            } else if (cur == ')' && *(it + 1) == '\\') {
                // size_t t = reg.rfind("\\(", reg.rend() - (it));
                // std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
                std::string substr = findChildReg(reg, reg.rend() - it - 2);
                it += substr.size() + 3;
                // 另右支为子表达式树，符号为默认
                p -> rson(buildRegTree(substr));
                p -> data(OP_CAT);
            } else if (cur == '*' && *(it + 1) == '\\') {
                it += 2;
                // size_t t = reg.rfind("\\(", reg.rend() - (it));
                // std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
                std::string substr = findChildReg(reg, reg.rend() - it - 2);

                it += substr.size() + 3;
                // 需要在右支上建立新的 星号运算树
                std::shared_ptr<RegTree> star(new RegTree(OP_STAR));
                star -> rson(new RegTree('*'));
                star -> lson(buildRegTree(substr));
                p -> rson(star);
                p -> data(OP_CAT);
            } else if (cur == '?' && *(it + 1) == '\\') {
                ++it;
                // set to zero, empty jump
                p -> rson(new RegTree(0));
                // 设置运算符
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
            std::shared_ptr<RegTree> r = parent -> rson();

            if (!gp) {
                // delete root;
                root = r;
            } else {
                // delete gp -> lson();
                gp -> lson(r);
            }
        }

        return root;
    }

    void error_shoot(const char *message) {
        printf("from preprosessor: error** -> %s\n", message);
    }

  private:
    // vec tag : reg
    std::vector<std::pair<std::string, std::string> > _vecRegs;
    std::map<std::string, std::string> _regs;
    // tag need to be build to dfa
    std::vector<std::string> _toDFA_tags;
    std::set<char> _set_op;
    std::map<std::string, std::shared_ptr<RegTree> > _regTrees;
};

#endif