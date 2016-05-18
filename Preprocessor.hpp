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
        _set_op.insert('[');
        _set_op.insert(']');
        _set_op.insert('*');
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

    LexicalAnalyzer *buildLA() {
        LexicalAnalyzer *la = new LexicalAnalyzer();

        for (auto &var : _vecRegs) {
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

            // 子表达式树重用
            if (cur == ']' && *(it + 1) == '\\') {
                it += 2;
                size_t t = reg.rfind("\\[", reg.rend() - (it));
                std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
                // printf("substr = %s\n", substr.data());
                // fflush(stdout);
                parent = p;
                // _regTrees[substr] -> middleOrderDisplay();
                p -> rson(_regTrees[substr]);
                p -> data(0);
                it += substr.size() + 1;
            } else if (cur == '|' && *(it + 1) == '\\') {
                fflush(stdout);
                std::string substr = reg.substr(0, reg.rend() - it - 1);
                // printf("substr = %s\n", substr.data());
                // fflush(stdout);
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
                parent = p;
                p -> rson(new RegTree(cur));
                // 设置运算符
                p -> data(0);
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
    // vec tag : reg
    std::vector<std::pair<std::string, std::string> > _vecRegs;
    // tag need to be build to dfa
    std::set<std::string> _toDFA_tags;
    std::set<char> _set_op;
    std::map<std::string, RegTree *> _regTrees;
};

#endif