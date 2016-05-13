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
        _set_op.insert('*');
    }

    ~Preprocessor() = default;

    void trim(std::string &reg) {
        for (auto it = reg.begin(); it != reg.end(); ++it) {
            if (*it == ' ') {
                it = reg.erase(it);
            }
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
        trim(reg);

        std::string tag = split(reg);

        if (tag[0] == ':') {
            tag = tag.substr(1);
        } else {
            _toDFA_tags.insert(tag);
        }

        printf("tag = %s\n", tag.data());
        printf("reg = %s\n", reg.data());

        std::string strContent;

        for (auto it = reg.begin(); it != reg.end(); ++it) {

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

        for (auto &tag : _toDFA_tags) {
            la -> add(buildDFA(tag, _regs[tag]));
        }

        return la;
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
};

#endif