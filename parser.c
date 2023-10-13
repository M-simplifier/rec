#include "rec.h"

typedef union NDUnion NDUnion;
typedef union NDExprUnion NDExprUnion;

union NDExprUnion {
    NDExprIdent ident;
    NDExprNum num;
    NDExprFc fc;
};

union NDUnion {
    NDProgram program;
    NDDefinition definition;
    NDArg arg;
    NDType type;
    NDBody body;
    NDStmt stmt;
    NDExprUnion expr;
};

static ND *newND(NDKind kind, TK *token);
static NDExpr *newNDExpr(NDExprKind exprKind, TK *token);
static TK *checkThenNext(TK *token, TKKind expectedKind,
                         char const *expectedString);
static bool check(TK *token, TKKind kind, char const *string);

static ND *newND(NDKind kind, TK *token) {
    ND *node = calloc(1, sizeof(NDUnion));
    node->kind = kind;
    node->token = token;
    return node;
}

static NDExpr *newNDExpr(NDExprKind exprKind, TK *token) {
    NDExpr *node = newND(NDKind_EXPR, token);
    node->exprKind = exprKind;
    return node;
}

static TK *checkThenNext(TK *token, TKKind expectedKind,
                         char const *expectedString) {
    if (check(token, expectedKind, expectedString)) {
        return token->next;
    }
    printf("%s", expectedString);
    error("checkThenNext");
}

static bool check(TK *token, TKKind kind, char const *string) {
    if (token->kind != kind) {
        return false;
    }
    char const *tokenString;
    switch (kind) {
    case TKKind_KEYWORD:
        tokenString = ((TKKeyword *)token)->keyword;
        break;
    case TKKind_PUNCT:
        tokenString = ((TKPunct *)token)->punct;
        break;
    default:
        error("check");
        break;
    }
    if (strcmp(tokenString, string) != 0) {
        return false;
    }
    return true;
}

static NDProgram *program(TK **rest, TK *token);
static NDDefinition *definition(TK **rest, TK *token);
static NDArg *args(TK **rest, TK *token);
static NDArg *arg(TK **rest, TK *token);
static NDType *type(TK **rest, TK *token);
static NDBody *body(TK **rest, TK *token);
static NDStmt *stmt(TK **rest, TK *token);
static NDExpr *expr(TK **rest, TK *token);
static NDExpr *unit(TK **rest, TK *token);
static NDExprFc *fc(TK **rest, TK *token);
static NDExpr *params(TK **rest, TK *token);
static NDExprIdent *ident(TK **rest, TK *token);
static NDExprNum *num(TK **rest, TK *token);

// program := definition*
static NDProgram *program(TK **rest, TK *token) {
    NDProgram *node = newND(NDKind_PROGRAM, token);
    NDDefinition head = {};
    NDDefinition *nd = &head;
    while (token->kind != TKKind_EOF) {
        nd = nd->next = definition(&token, token);
    }
    node->definitions = head.next;
    return node;
}

// definition := ident args type body
static NDDefinition *definition(TK **rest, TK *token) {
    NDDefinition *node = newND(NDKind_DEFINITION, token);
    node->ident = ident(&token, token);
    node->args = args(&token, token);
    node->type = type(&token, token);
    node->body = body(&token, token);
    *rest = token;
    return node;
}

// args := "(" (arg ("," arg)*)? ")"
static NDArg *args(TK **rest, TK *token) {
    token = checkThenNext(token, TKKind_PUNCT, "(");
    if (check(token, TKKind_PUNCT, ")")) {
        token = token->next;
        *rest = token;
        return NULL;
    }
    NDArg head = {};
    NDArg *nd = &head;
    while (true) {
        nd = nd->next = arg(&token, token);
        if (!check(token, TKKind_PUNCT, ",")) {
            break;
        }
        token = token->next;
    }
    token = checkThenNext(token, TKKind_PUNCT, ")");
    *rest = token;
    return head.next;
}

// arg := ident type
static NDArg *arg(TK **rest, TK *token) {
    NDArg *node = newND(NDKind_ARG, token);
    node->ident = ident(&token, token);
    node->type = type(&token, token);
    *rest = token;
    return node;
}

// type := "i32"
static NDType *type(TK **rest, TK *token) {
    NDType *node = newND(NDKind_TYPE, token);
    token = checkThenNext(token, TKKind_KEYWORD, "i32");
    *rest = token;
    return node;
}

// body := "{" stmt* "}"
static NDBody *body(TK **rest, TK *token) {
    NDBody *node = newND(NDKind_BODY, token);
    token = checkThenNext(token, TKKind_PUNCT, "{");
    NDStmt head = {};
    NDStmt *nd = &head;
    while (!check(token, TKKind_PUNCT, "}")) {
        nd = nd->next = stmt(&token, token);
    }
    token = token->next;
    *rest = token;
    node->stmt = head.next;
    return node;
}

// stmt := "return" expr ";"
static NDStmt *stmt(TK **rest, TK *token) {
    NDStmt *node = newND(NDKind_STMT, token);
    token = checkThenNext(token, TKKind_KEYWORD, "return");
    node->expr = expr(&token, token);
    token = checkThenNext(token, TKKind_PUNCT, ";");
    *rest = token;
    return node;
}

// expr := unit (("+"|"-"|"*"|"/"|"%") expr)?
static NDExpr *expr(TK **rest, TK *token) {
    NDExpr *lhs = unit(&token, token);
    if (check(token, TKKind_PUNCT, "+") | check(token, TKKind_PUNCT, "-") |
        check(token, TKKind_PUNCT, "*") | check(token, TKKind_PUNCT, "/") |
        check(token, TKKind_PUNCT, "%")) {
        NDExprBinary *node = newNDExpr(NDExprKind_BINARY, token);
        node->ope = ((TKPunct *)token)->punct;
        token = token->next;
        node->lhs = lhs;
        node->rhs = expr(&token, token);
        *rest = token;
        return node;
    }
    *rest = token;
    return lhs;
}

// unit := num
//       | ident
//       | fc
static NDExpr *unit(TK **rest, TK *token) {
    if (token->kind == TKKind_NUM) {
        NDExprNum *node = num(&token, token);
        *rest = token;
        return node;
    }
    if (!check(token->next, TKKind_PUNCT, "(")) {
        NDExprIdent *node = ident(&token, token);
        *rest = token;
        return node;
    }
    NDExprFc *node = fc(&token, token);
    *rest = token;
    return node;
}

// fc := ident params
static NDExprFc *fc(TK **rest, TK *token) {
    NDExprFc *node = newNDExpr(NDExprKind_FC, token);
    node->ident = ident(&token, token);
    node->params = params(&token, token);
    *rest = token;
    return node;
}

// params := "(" (expr (, expr)*)? ")"
static NDExpr *params(TK **rest, TK *token) {
    token = checkThenNext(token, TKKind_PUNCT, "(");
    if (check(token, TKKind_PUNCT, ")")) {
        token = token->next;
        *rest = token;
        return NULL;
    }
    NDExpr head = {};
    NDExpr *nd = &head;
    while (true) {
        nd = nd->next = expr(&token, token);
        if (!check(token, TKKind_PUNCT, ",")) {
            break;
        }
        token = token->next;
    }
    token = checkThenNext(token, TKKind_PUNCT, ")");
    *rest = token;
    return head.next;
}

// ident
static NDExprIdent *ident(TK **rest, TK *token) {
    NDExprIdent *node = newNDExpr(NDExprKind_IDENT, token);
    // this data depends on TK
    node->ident = ((TKIdent *)token)->ident;
    token = token->next;
    *rest = token;
    return node;
}

// num
static NDExprNum *num(TK **rest, TK *token) {
    NDExprNum *node = newNDExpr(NDExprKind_NUM, token);
    node->num = ((TKNum *)token)->num;
    token = token->next;
    *rest = token;
    return node;
}

NDProgram *parse(TK *token) {
    NDProgram *ast = program(&token, token);
    return ast;
}
