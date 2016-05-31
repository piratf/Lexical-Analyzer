#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include "nfatable.hpp"
#include "dfalink.hpp"
#include "LexicalAnalyzer.hpp"
#include "pclock.hpp"
#include <string>
#include <map>

using namespace piratf;

extern const char OP_CAT;
extern const char OP_OR;
extern const char OP_STAR;
extern const char JUMP_EMP;

class Preprocessor {
  public:
    // 构造时插入运算符到集合
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

    /**
     * 去除字符串尾部的空格
     * @author piratf
     * @param  reg 字符串引用
     * @return     void
     */
    void inline trim_after(std::string &reg) {
        while (reg.back() == ' ') {
            reg.pop_back();
        }
    }

    /**
     * 分割字符串，得到表达式名称字符串，返回表达式内容字符串
     * @author piratf
     * @param  tag 原始表达式字符串
     * @return     表达式内容字符串
     */
    std::string split(std::string &tag) {
        auto it = tag.begin();

        // 找到第一个 = 符号
        for (; it != tag.end(); ++it) {
            if (*it == '=') {
                break;
            }
        }

        auto it2 = it + 1;

        // 处理 reg 前部的空格
        while (*it2 == ' ') {
            ++it2;
        }

        std::string reg(std::move(tag.substr(it2 - tag.begin(), tag.end() - it2).data()));
        it2 = it - 1;

        // 处理 tag 尾部的空格
        while (*it2 == ' ') {
            --it2;
        }

        tag.erase(it2 + 1, tag.end());
        return std::move(reg);
    }

    /**
     * 预处理正规式字符串
     * @author piratf
     * @param  tag 原始正规式字符串
     */
    void update(std::string &tag) {

        // 分割字符串，传去的 string 被切割剩下 tag，返回后半段的 reg 内容 string。
        std::string reg = split(tag);
        // 处理语法糖，去除分割后 reg 末尾的空格。
        trim_after(reg);

        // 处理 ':' 开头的辅助正规式语义
        if (tag[0] == ':') {
            // 去除这个符号
            tag = tag.substr(1);
        } else {
            // 添加到将要被处理成 DFA 的 vector 中。
            _toDFA_tags.push_back(tag);
        }

        printf("tag = %s\n", tag.data());
        printf("reg = %s\n", reg.data());

        // 存储被处理后的 reg 内容
        std::string strContent;

        for (auto it = reg.begin(); it != reg.end(); ++it) {

            // 转义字符，直接填到结果中
            if (*it == '\\') {
                ++it;
                strContent.push_back(*it);
                continue;
            }

            // 处理 [ ] 括号中的语法糖
            if (*it == '[') {
                // 语义不需要支持嵌套括号
                size_t right = reg.find(']', it - reg.begin());

                // 如果中括号不成对
                if (right == std::string::npos) {
                    error_shoot("error reg. missing ] ");
                }

                // 构造子字符串 得到 [ ] 括号中的内容
                std::string subreg(it + 1, reg.begin() + right);

                // 处理连续字符缩写语法糖，正则式引用的逻辑先保留，到语法树阶段再处理
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

            // 如果这个字符是独立出现的运算符，就在之前加上运算符转义
            if (_set_op.find(*it) != _set_op.end()) {
                strContent.push_back('\\');
            }

            strContent.push_back(*it);
        }

        // 处理完成后的 reg 连同 tag，添加到 vector 中
        _vecRegs.push_back(make_pair(tag, strContent));
        // 加入集合
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

    void buildLA(LexicalAnalyzer &la) {

        for (auto &var : _vecRegs) {
            _regTrees[var.first] = buildRegTree(var.second);
            // _regTrees[var.first] -> backOrderDisplay();
            // _regTrees[var.first] -> middleOrderDisplay();
        }

        Clock buildDFAClock;

        for (auto &tag : _toDFA_tags) {
            auto nfa = buildNFA(_regTrees[tag]);

            // // _regTrees[tag] -> backOrderDisplay();
            // // _regTrees[tag] -> middleOrderDisplay();
            buildDFAClock.start(tag.data());
            DFA *dfa = buildDFA(nfa);
            buildDFAClock.terminal();
            buildDFAClock.display_micro_seconds();
            dfa -> tag(tag);
            dfa -> minimize();

            la.add(dfa);

            if (tag == "operator") {
                la.separator(dfa);
            }

            // dfa -> display();
            // fflush(stdout);
            // printf("tag = %s\n", tag.data());
            // fflush(stdout);
        }
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
        // 当前结点的祖父节点
        std::shared_ptr<RegTree> gp = NULL;
        // 当前结点的父节点
        std::shared_ptr<RegTree> parent = NULL;
        // 当前结点
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

            // 子表达式树重用逻辑
            if (cur == ']' && *(it + 1) == '\\') {
                it += 2;
                // 找到结束括号
                size_t t = reg.rfind("\\[", reg.rend() - (it));
                // 取出内容
                std::string substr = reg.substr(t + 2, reg.rend() - it - t - 2);
                // 在保存树的 set 结构中找到对应树的根节点，赋值
                if (_regTrees.find(substr) == _regTrees.end()) {
                    error_shoot("can't find the sub reg tree.");
                    return std::shared_ptr<RegTree>(NULL);
                }
                p -> rson(_regTrees[substr]);
                p -> data(OP_CAT);
                it += substr.size() + 1;
            } else if (cur == '|' && *(it + 1) == '\\') {
                std::string substr = reg.substr(0, reg.rend() - it - 2);
                it = reg.rend() - 1;
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
                std::string substr = findChildReg(reg, reg.rend() - it - 2);
                it += substr.size() + 3;
                // 另右支为子表达式树，符号为默认
                p -> rson(buildRegTree(substr));
                p -> data(OP_CAT);
            } else if (cur == '*' && *(it + 1) == '\\') {
                it += 2;
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
                // ? 运算表示空跳转，空跳转设置为 0
                p -> rson(new RegTree(JUMP_EMPTY));
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
                root = r;
            } else {
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