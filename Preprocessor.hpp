#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include <string>
#include <map>

class Preprocessor {
  public:
    Preprocessor() = default;
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
        return ((it - 1 == str.begin()) || (*(it - 1) == '|'))
               && ((it + 1 == str.end()) || (*(it + 1) == '|'));
    }

    std::map<std::string, std::string>& regs() {
        return _regs;
    }

    void update(std::string &reg) {
        trim(reg);
        std::string tag = split(reg);
        printf("tag = %s\n", tag.data());
        printf("reg = %s\n", reg.data());

        std::string strContent;

        for (auto it = reg.begin(); it != reg.end(); ++it) {

            if (*it == '[' && !standalone(reg, it)) {
                size_t right = reg.find(']', it - reg.begin());

                if (right == std::string::npos) {
                    error_shoot("error reg.");
                }

                std::string subreg(it + 1, reg.begin() + right);
                printf("subreg = %s\n", subreg.data());

                if (subreg.size() == 3 && subreg[1] == '-') {
                    char startchar = subreg[0], endchar = subreg[2];
                    subreg.clear();

                    for (char i = startchar; i < endchar; ++i) {
                        subreg.push_back(i);
                        subreg.push_back('|');
                    }

                    subreg.push_back(endchar);
                    strContent.append(subreg);
                } else if (_regs.find(subreg) != _regs.end()) {
                    strContent.append(_regs[subreg]);
                } else {
                    strContent.append(subreg);
                }

                it = reg.begin() + right;
                continue;
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

        puts("-------- end display -------------------");
    }

    void error_shoot(const char *message) {
        printf("from preprosessor: error** -> %s\n", message);
    }

  private:
    std::map<std::string, std::string> _regs;
};

#endif