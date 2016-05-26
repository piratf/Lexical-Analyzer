#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

#include "dfalink.hpp"
#include <fstream>
#include <cstring>
#include <set>

// static const int N = 1024;
static const int BUFFER_SIZE = N + 7;

std::set<char> g_terminal_set;

void trim(char *temp) {
    int i = strlen(temp);

    while (temp[i - 1] == ' ') {
        temp[i - 1] = temp[i];
        --i;
    }
}

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
        puts("============================");
        printf("=> start parse.\n");
        std::ifstream ifile(filePath);

        line_num = 1;
        char *buf = new char[BUFFER_SIZE];
        char *temp = new char[BUFFER_SIZE];
        std::vector<std::string> vecTagList;

        // init buffer
        memset(buf, 0, sizeof(char) * BUFFER_SIZE);
        ifile.read(buf, N);

        char *head = buf, *tail = buf;
        printf("Initial content: \n%s\n", buf);
        fflush(stdout);
        std::string tag;
        char *output = new char[BUFFER_SIZE];
        memset(output, 0, sizeof(char) * BUFFER_SIZE);

        // printf("%s\n", buf);
        while (*tail) {

            ++tail;

#ifdef DEBUG
            printf("tail = %c\n", *tail);
            fflush(stdout);
            printf("output = %s\n", output);
            fflush(stdout);
#endif

            memset(temp, 0, sizeof(char) * BUFFER_SIZE);

            if (tag == "comment") {
                strncpy(temp, head, tail -  head);
                temp[tail - head] = 0;
            } else {
                int i = 0;

                for (char *p = head; p != tail; ++p) {
                    if (*p == '\n') {
                        temp[i] = ' ';
                    } else {
                        temp[i] = *p;
                    }

                    ++i;
                }

                temp[i] = 0;
            }

            // for signle line comment

            // for multiline comment
            if (temp[0] == '/' && temp[1] == '*') {

                while (!(*tail == '*' && *(tail + 1) == '/') || !(*tail)) {
                    if (*tail == '\n') {
                        ++line_num;
                    }

                    ++tail;
                }

                strncpy(temp, head, tail -  head + 2);
                result_print("comment", temp);
                head = tail + 2;

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
                // complete a matching
                tag.clear();
                continue;
            }

#ifdef DEBUG
            printf("n = %d\n", tail - head);
            printf("temp = %s\n", temp);
            fflush(stdout);
#endif

            // trim space in temp
            // trim(temp);

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
                    result_print(tag.data(), output);
                    fflush(stdout);

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

        if (!tag.empty()) {
#ifdef DEBUG
            printf(" -----------> ");
#endif
            result_print(tag.data(), output);

            head = tail - (strlen(temp) - strlen(output));

            while (
                g_terminal_set.find(*head)
                != g_terminal_set.end()) {

                if (*head == '\n') {
                    ++line_num;
                }

                ++head;
            }
        }

        if (tail - head) {
            err_shot("Unreachable sequences!");
        } else {
            printf("Analysis success!\n");
        }

#ifdef DEBUG
        printf("head -> tail %d\n", tail - head);
#endif

        // }

        ifile.close();
    }

    void inline result_print(const char *tag, const char *content) {
        printf("line %d: ( %10.10s : %-s )\n", line_num, tag, content);
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
