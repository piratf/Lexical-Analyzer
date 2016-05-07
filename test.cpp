#include <cstdio>
#include <string>
#include <utility>
#include <fstream>
using namespace std;

int main() {
    ifstream input("input.txt");
    string reg;
    input >> reg;
    for (auto it = reg.rbegin(); it != reg.rend(); ++it) {
        printf("%c", *it);
        if (*it == ')') {
            size_t t = reg.rfind('(', reg.rend() - (it + 1) - 1);
            if (t == string::npos) {
                printf("error**: missing symmetric (");
                return 0;
            } else {
                t += 1;
                printf("%s", reg.substr(t, reg.rend() - (it + 1) - t).data());
                it = reg.rend() - t - 1;
            }
        }

        // if (*it == '|') {
        //     size_t t = reg.rfind('|', reg.rend() - (it + 2));

        //     if (t == string::npos) {
        //         // printf("t = %d\n", t);
        //         // printf("%s", reg.substr(0, reg.rend() - (it + 1)).data());
        //         break;
        //     } else {
        //         // printf("%d\n", t);
        //         t += 1;
        //         // printf("%s", reg.substr(t, reg.rend() - (it + 1) - t).data());
        //         it = reg.rend() - t - 1;
        //         printf("*it = %c\n", *(it));
        //     }
        // }
    }
    // printf("%d\n", reg.rfind('|'));;
    input.close();
    return 0;
}