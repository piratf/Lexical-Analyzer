#include <cstdio>
#include <iostream>
using namespace std;

struct Node {
    int data;
    char ch;

    Node(): data(-233), ch('a') {}
};


void readNode(unsigned char *node) {
    Node *pn = reinterpret_cast<Node *>(node);
    printf("data = %d\n", pn -> data);
}

void sendNode(Node *node) {
    readNode(reinterpret_cast<unsigned char *>(node));
}

int main() {
    Node *node = new Node();
    sendNode(node);
    return 0;
}