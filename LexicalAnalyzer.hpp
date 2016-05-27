#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

#include "dfalink.hpp"
#include <cstring>
#include <set>

// static const int N = 1024;
static const int BUFFER_SIZE = N + 7;

std::set<char> g_terminal_set;

bool continuity(char *temp) {
    while (*temp) {
        if (*temp == ' ' || *temp == '\n') {
            return false;
        }

        ++temp;
    }

    return true;
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

    void head_skip(char *&head) {
        while (
            g_terminal_set.find(*head)
            != g_terminal_set.end()) {

            if (*head == '\n') {
                ++line_num;
            }

            ++head;
        }
    }

    bool judgeDifferent(char *temp, char *output) {
        int len_temp = strlen(temp);
        int len_output = strlen(output);

        if (len_temp > len_output) {
            return false;
        }

        char *p = temp + len_output;

        while (*p) {
            if (g_terminal_set.find(*p) == g_terminal_set.end()) {
                return false;
            }

            ++p;
        }

        return true;
    }

    void parse(const char *filePath) {
        puts("===============================");
        printf("=> start parse.\n");
        // std::ifstream ifile(filePath);
        std::FILE *f = std::fopen(filePath, "r");

        // mark if the current temp str failed.
        bool failFlag = false;
        line_num = 1;
        unsigned int    error_cnt = 0;
        std::unique_ptr<char[]> _buf(new char[BUFFER_SIZE]);
        std::unique_ptr<char[]> _temp(new char[BUFFER_SIZE]);
        std::unique_ptr<char[]> _output(new char[BUFFER_SIZE]);

        char * buf = _buf.get();
        char * temp = _temp.get();
        char * output = _output.get();
        char *tail = NULL;
        char *head = NULL;

        // init buffer
        memset(buf, 0, sizeof(char) * BUFFER_SIZE);
        std::string tag;

        // ifile.read(buf, N);
        size_t read_cnt = N;
        size_t remain_size = 0;
        bool comment_state = false;
        bool long_comment_state = false;

        while (read_cnt >= N) {
            read_cnt = std::fread(buf + remain_size, sizeof buf[0], N, f);
            printf("read cnt = %d\n", read_cnt);
            printf("buf[0] = %c\n", buf[0]);
            buf[read_cnt] = 0;
            printf("Current content: \n%s\n", buf);
            // ifile.close();

            head = buf;
            tail = buf;
            // fflush(stdout);
            memset(output, 0, sizeof(char) * BUFFER_SIZE);

            // skip to my lou
            head_skip(head);

            tail = head;

            while (*tail) {

                if (g_terminal_set.find(*(tail - 1))
                        != g_terminal_set.end() && failFlag) {
                    printf("----------> %s\n", temp);
                    fflush(stdout);
                    err_shot("Unreachable sequences.");
                    ++error_cnt;

                    head = tail;

                    head_skip(head);

                    tail = head;
                }

                if (comment_state) {
                    while (*tail && *tail != '\n') {
                        ++head;
                        ++tail;
                    }

                    head_skip(head);
                    tail = head;
                    // printf("comment head.\n");
                    // fflush(stdout);
                    comment_state = false;
                }

                if (long_comment_state) {
                    while (*tail && *tail != '/' && *(tail - 1) == '*') {
                        ++head;
                        ++tail;
                    }

                    head_skip(head);
                    tail = head;
                    long_comment_state = false;
                }

                ++tail;

#ifdef DEBUG
                printf("tail = %c\n", *tail);
                fflush(stdout);
                printf("output = %s\n", output);
                fflush(stdout);
#endif

                memset(temp, 0, sizeof(char) * BUFFER_SIZE);

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

                // for signle line comment
                if (temp[0] == '/' && temp[1] == '/') {
                    comment_state = true;
                    // printf("comment sattle.\n");

                    while (*tail && *tail != '\n') {
                        ++tail;
                    }

                    if (*tail) {
                        // printf("comment funish.\n");
                        comment_state = false;
                    } else {
                        break;
                    }

                    ++line_num;

                    strncpy(temp, head, tail - head);
                    result_print("comment", temp);
                    head = tail + 1;

                    head_skip(head);

#ifdef DEBUG
                    printf("head = %c\n", *head);
#endif
                    tail = head;
                    // complete a matching
                    tag.clear();
                    continue;
                }

                // for multiline comment
                if (temp[0] == '/' && temp[1] == '*') {
                    long_comment_state = true;

                    while (!(*tail == '*' && *(tail + 1) == '/') || !(*tail)) {
                        if (*tail == '\n') {
                            ++line_num;
                        }

                        ++tail;
                    }

                    if (*tail) {
                        long_comment_state = false;
                    } else {
                        break;
                    }

                    strncpy(temp, head, tail -  head + 2);
                    result_print("comment", temp);
                    head = tail + 2;

                    head_skip(head);

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

                std::string cur = calculate(temp);

                // matching failed
                if (cur.empty()) {
                    failFlag = true;
#ifdef DEBUG
                    printf("failed, temp = %s\n", temp);
                    fflush(stdout);
#endif

                    // has a success before
                    if (!tag.empty()) {
                        if (tag == "temp") {
                            continue;
                        }

                        // operator could be the separator
                        if (tag == "operator" ||
                                ((!continuity(temp) || _separator -> calculate(temp + strlen(temp) - 1))
                                 && strlen(output) == strlen(temp) - 1)) {

                            failFlag = false;
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

                            tag.clear();
                        } else {
                            continue;
                        }
                    }

                } else {
                    failFlag = false;
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

            remain_size = tail - head;

            // printf("comment ? %d\n", comment_state);
            if (remain_size && !comment_state && !long_comment_state) {
                // printf("copy !\n");
                // fflush(stdout);
                while (!*head && remain_size) {
                    ++head;
                    --remain_size;
                }
                strncpy(buf, head, remain_size);
            } else {
                remain_size = 0;
                memset(buf, 0, sizeof(buf[0]) * N);
            }
        }

        if (!tag.empty() && strcmp(tag.data(), "temp") != 0 && (strcmp(temp, "operator") == 0 || judgeDifferent(temp, output)) ) {
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

        puts("-------------------------------");

        if (tail - head) {
            err_shot("Unreachable sequences!");
            printf("Parse end, %d error found.\n", error_cnt + 1);
        } else {
            if (error_cnt) {
                printf("Parse end, %d error found.\n", error_cnt);
            } else {
                printf("Parse success!\n");
            }
        }

        puts("===============================");

#ifdef DEBUG
        printf("head -> tail %d\n", tail - head);
#endif
    }

    void inline result_print(const char *tag, const char *content) {
        printf("line %-4d: ( %10.10s : %-s )\n", line_num, tag, content);
        fflush(stdout);
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

    void separator(DFA *other) {
        _separator = other;
    }

  private:
    std::vector<DFA *> _vecRM;
    DFA *_separator;
    unsigned int line_num;
};

#endif
