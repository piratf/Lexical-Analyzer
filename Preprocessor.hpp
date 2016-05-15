#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include "dfa.hpp"
#include "LexicalAnalyzer.hpp"
#include <string>
#include <map>

class Preprocessor {
  public:
    Preprocessor() {
        _set_op.insert('|');
        _set_op.insert(')');
        _set_op.insert('(');
        _set_op.insert('*');
    }

    ~Preprocessor() = default;

    void trim(std::string &reg) {
        for (auto it = reg.begin(); it != reg.end(); ++it) {
            if (*it == ' ') {
                it = reg.erase(it);
            } else {
                break;
            }
        }

        while (reg.back() == ' ') {
            reg.pop_back();
        }
    }

    std::string split(std::string &reg) {
        auto it = reg.begin();

        for (; it != reg.end(); ++it) {
            if (*it == '=') {
                break;
            }
        }

        std::string tag = reg.substr(0, it - reg.begin());
        reg.erase(reg.begin(), it + 1);
        return tag;
    }

    bool standalone(const std::string &str, const std::string::iterator &it) {
        return (((it - 1 == str.begin()) || (*(it - 1) == '|'))
                && ((it + 1 == str.end()) || (*(it + 1) == '|'))) || ((*(it - 1) == '(') && (*(it + 1) == ')'));
    }

    std::map<std::string, std::string> &regs() {
        return _regs;
    }

    void update(std::string &reg) {

        std::string tag = split(reg);
        trim(tag);
        trim(reg);

        if (tag[0] == ':') {
            tag = tag.substr(1);
        } else {
            _toDFA_tags.insert(tag);
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
                printf("subreg = %s\n", subreg.data());

                // 处理连续字符缩写语法糖
                if (subreg.size() == 3 && subreg[1] == '-') {
                    char startchar = subreg[0], endchar = subreg[2];
                    subreg.clear();

                    for (char i = startchar; i < endchar; ++i) {
                        subreg.push_back(i);
                        subreg.push_back('\\');
                        subreg.push_back('|');
                    }

                    subreg.push_back(endchar);
                    strContent.append(subreg);
                } else if (_regs.find(subreg) != _regs.end()) {
                    // 处理子 reg 语法糖
                    strContent.append(_regs[subreg]);
                }

                it = reg.begin() + right;
                continue;
            }

            if (_set_op.find(*it) != _set_op.end() && !standalone(reg, it)) {
                strContent.push_back('\\');
            }

            strContent.push_back(*it);
        }

        _regs[tag] = strContent;
    }

    void display() {
        puts("-------- preprosessor display ----------");
        printf("size = %d\n", _regs.size());

        for (auto &var : _regs) {
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

    LexicalAnalyzer *buildLA() {
        LexicalAnalyzer *la = new LexicalAnalyzer();

        for (auto &var : _regs) {
            _regTrees[var.first] = buildRegTree(var.second);
        }

        for (auto &tag : _toDFA_tags) {
            NFA *nfa = buildNFA(_regTrees[tag]);
            DFA *dfa = buildDFA(nfa);
            dfa -> tag(tag);
            dfa -> minimize();
            la -> add(dfa);
        }

        return la;
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
                fflush(stdout);
                std::string substr = reg.substr(0, reg.rend() - it - 1);
                // printf("substr = %s\n", substr.data());
                fflush(stdout);
                it = reg.rend() - 1;
                // printf("it = %c\n", *it);
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
                std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
                it += substr.size();
                // 另右支为子表达式树，符号为默认
                p -> rson(buildRegTree(substr));
                p -> data(OP_CAT);
            } else if (cur == '*' && *(it + 1) == '\\') {
                it += 3;
                size_t t = reg.rfind("\\(", reg.rend() - (it));
                std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
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

    void error_shoot(const char *message) {
        printf("from preprosessor: error** -> %s\n", message);
    }

  private:
    // tag : reg
    std::map<std::string, std::string> _regs;
    // tag need to be build to dfa
    std::set<std::string> _toDFA_tags;
    std::set<char> _set_op;
    std::map<std::string, RegTree *> _regTrees;
};

#endif