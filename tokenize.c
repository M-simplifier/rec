#include "rec.h"

typedef union TKUnion TKUnion;

union TKUnion {
    TKIdent ident;
    TKNum num;
    TKPunct punct;
};

static TK *newTK(TKKind kind, char const *location) {
    TK *token = calloc(1, sizeof(TKUnion));
    token->kind = kind;
    token->location = location;
    return token;
}

static bool isKeyword(char const *string) {
    char *keywords[] = {"i32", "return"};
    for (int i = 0; i < LENGTH_OF(keywords); i++) {
        if (strcmp(string, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

TK *tokenize(char const *string) {
    char const *p = string;
    TK head = {};
    TK *tk = &head;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (isdigit(*p)) {
            TKNum *token = newTK(TKKind_NUM, p);
            long num = strtol(p, &p, 10);
            token->num = num;
            tk = tk->next = token;
            continue;
        }

        if (ispunct(*p)) {
            TKPunct *token = newTK(TKKind_PUNCT, p);
            token->punct = takeSubString(p, 1);
            p++;
            tk = tk->next = token;
            continue;
        }

        if (isalnum(*p)) {
            char const *q = p;
            while (isalnum(*p)) {
                p++;
            }
            int len = p - q;
            char const *keywordOrIdent = takeSubString(q, len);
            if (isKeyword(keywordOrIdent)) {
                TKKeyword *token = newTK(TKKind_KEYWORD, q);
                token->keyword = keywordOrIdent;
                tk = tk->next = token;
            } else {
                TKIdent *token = newTK(TKKind_IDENT, q);
                token->ident = keywordOrIdent;
                tk = tk->next = token;
            }
            continue;
        }

        error("tokenizer");
    }

    TKEof *token = newTK(TKKind_EOF, p);
    tk = tk->next = token;

    return head.next;
}