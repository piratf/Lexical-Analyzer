RegTree *buildRegTree(const string &reg) {
#ifdef DEBUG
    printf("reg = %s\n", reg.data());
#endif
    RegTree *root = new RegTree();
    RegTree *parent = NULL;
    RegTree *gp = NULL;
    RegTree *p = root;

    for (auto it = reg.rbegin(); it != reg.rend(); ++it) {
        char cur = *it;

        // 对于转义字符直接跳过
        if (cur == '\\') {
            continue;
        }

#ifdef DEBUG
        printf("cur = %c\n", cur);
#endif

        // 如果是 or 符号
        if (cur == '|' && *(it + 1) == '\\') {

#ifdef DEBUG
            printf("||||||||\n");
#endif

            if (!p -> leaf() && !p -> lson() && !p -> rson()) {
                char data = parent -> rson() -> data();
                delete parent;
                parent = new RegTree(data);
            }

            size_t t = reg.rfind('|', reg.rend() - (it + 2));

            if (t == string::npos) {
#ifdef DEBUG
                printf("this is the last |\n");
#endif
                p = root;
                root = new RegTree('|');
                root -> rson(p);
                root -> lson(buildRegTree(reg.substr(0, reg.rend() - (it + 1))));
#ifdef DEBUG
                printf("t == string::npos: \n");
                printf("the reg = %s\n", reg.data());
                root -> backOrderDisplay();
#endif
                return root;
            } else {
#ifdef DEBUG
                printf("this is not the last |\n");
#endif
                t += 1;
                p = root;
                root = new RegTree('|');
                root -> rson(p);
                root -> lson(buildRegTree(reg.substr(t, reg.rend() - (it + 1) - t)));
                it = reg.rend() - t - 1;
            }

            continue;

        }

        if (cur == ')' && *(it + 1) == '\\') {
            ++it;
            fflush(stdout);

            size_t t = reg.rfind('(', reg.rend() - (it + 1) - 1);

            if (t == string::npos) {
                printf("error**: missing symmetric (");
                return NULL;
            } else {
                t += 1;
                // string temp = reg.substr(t, reg.rend() - (it + 1) - t);

                p -> rson(buildRegTree(reg.substr(t, reg.rend() - (it + 1) - t)));
                it = reg.rend() - t;
            }
        } else if (cur == '*' && *(it + 1) == '\\') {
        } else {
            p -> rson(new RegTree(cur));
        }

        p -> lson(new RegTree());
        gp = parent;
        parent = p;

        if (cur == '*') {
            p -> data(OP_STAR);
        } else {
            p -> data(OP_CAT);
        }

        p = p -> lson();
    }

    if (!p -> leaf() && !p -> lson() && !p -> rson()) {
        RegTree *r = parent -> rson();

        if (gp) {
#ifdef DEBUG
            printf("modify gp.\n");
#endif
            delete gp -> lson();
            gp -> lson(r);
        } else {
#ifdef DEBUG
            printf("modify root.\n");
#endif
            delete root;
            root = r;
        }
    }