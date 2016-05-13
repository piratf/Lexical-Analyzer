#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

#include "dfa.hpp"
#include <fstream>
#include <cstring>
#include <set>

// static const int N = 1024;
static const int M = N + 7;

std::set<char> g_terminal_set;

class LexicalAnalyzer {
  public:
    LexicalAnalyzer() {
        init();
    }

    ~LexicalAnalyzer() = default;

    void init() {
        g_terminal_set.insert(' ');
        g_terminal_set.insert('\n');
        g_terminal_set.insert('\r');
    }

    void add(DFA *dfa) {
        _vecRM.push_back(dfa);
    }

    void parse(const char *filePath) {
        std::ifstream ifile(filePath);

        line_num = 0;
        char *buf = new char[M];
        char *temp = new char[M];
        std::vector<std::string> vecTagList;

        // while (!ifile.eof()) {
        // read data to buffer
        memset(buf, 0, sizeof(char) * M);
        ifile.read(buf, N);

        char *head = buf, *tail = buf;
        printf("%s\n", buf);
        std::string tag;
        char *output = new char[M];
        memset(output, 0, sizeof(char) * M);

        // printf("%s\n", buf);
        while (*tail) {

            ++tail;
#ifdef DEBUG
            printf("tail = %c\n", *tail);
            fflush(stdout);
            printf("output = %s\n", output);
            fflush(stdout);
#endif

            memset(temp, 0, sizeof(char) * M);
            strncpy(temp, head, tail -  head);
            temp[strlen(temp)] = 0;

#ifdef DEBUG
            printf("n = %d\n", tail - head);
            printf("temp = %s\n", temp);
            fflush(stdout);
#endif

            std::string cur = calculate(temp);

            if (cur.empty()) {
#ifdef DEBUG
                printf("failed, temp = %s\n", temp);
                fflush(stdout);
#endif

                // 匹配失败
                if (!tag.empty()) {
#ifdef DEBUG
                    printf(" -----------> ");
#endif
                    printf("( %s : %-s ) \n\tline %d\n", tag.data(), output, line_num);

                    head = tail - (strlen(temp) - strlen(output));

                    while (
                        g_terminal_set.find(*head)
                        != g_terminal_set.end()) {

                        if (*head == '\n') {
                            ++line_num;
                        }

                        ++head;
                    }


#ifdef DEBUG
                    printf("head = %c\n", *head);
#endif
                    tail = head;

                    // correction
                    // ...
                    // end of correction

                    vecTagList.push_back(tag);
                    tag.clear();
                }

            } else {
#ifdef DEBUG
                printf("success, tag = %s, temp = %s\n", cur.data(), temp);
#endif
                // 匹配成功
                tag = cur;
                strcpy(output, temp);
#ifdef DEBUG
                printf("output = %s\n", output);
                fflush(stdout);
#endif
            }

        }

        if (tail - head) {
            err_shot("Unreachable sequences!");
        }

#ifdef DEBUG
        printf("head -> tail %d\n", tail - head);
#endif

        // }

        ifile.close();
    }

    std::string calculate(const char *input) {
        std::string tag;

        for (auto *var : _vecRM) {
            if (var -> calculate(input)) {
                tag = var -> tag();
                break;
            }
        }
        
        return tag;
    }

    void err_shot(const char *message) {
        printf("error** : %s line %d\n", message, line_num);
        fflush(stdout);
    }

  private:
    std::vector<DFA *> _vecRM;
    unsigned int line_num;
};

#endif
