#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// utils.c
//

#define LENGTH_OF(array) (sizeof(array) / sizeof((array)[0]))

char *readFile(char const *filename);
char *takeSubString(char const *string, int len);
char *addExtension(char const *filename, char const *extension);
void error(char const *message);

//
// tokenize.c
//

typedef enum TKKind TKKind;
typedef struct TK TK;
typedef struct TKKeyword TKKeyword;
typedef struct TKIdent TKIdent;
typedef struct TKNum TKNum;
typedef struct TKPunct TKPunct;
typedef struct TKEof TKEof;

enum TKKind {
    TKKind_KEYWORD,
    TKKind_IDENT,
    TKKind_NUM,
    TKKind_PUNCT,
    TKKind_EOF,
};

struct TK {
    TKKind kind;
    char const *location;
    TK *next;
};

struct TKKeyword {
    TKKind kind;
    char *location;
    TK *next;

    char *keyword;
};

struct TKIdent {
    TKKind kind;
    char *location;
    TK *next;

    char *ident;
};

struct TKNum {
    TKKind kind;
    char *location;
    TK *next;

    int num;
};

struct TKPunct {
    TKKind kind;
    char *location;
    TK *next;

    char *punct;
};

struct TKEof {
    TKKind kind;
    char *location;
    TK *next;
};

TK *tokenize(char const *string);

//
// parse.c
//

typedef enum NDKind NDKind;
typedef struct ND ND;
typedef struct NDProgram NDProgram;
typedef struct NDDefinition NDDefinition;
typedef struct NDArg NDArg;
typedef struct NDType NDType;
typedef struct NDBody NDBody;
typedef struct NDStmt NDStmt;
typedef enum NDExprKind NDExprKind;
typedef struct NDExpr NDExpr;
typedef struct NDExprBinary NDExprBinary;
typedef struct NDExprIdent NDExprIdent;
typedef struct NDExprNum NDExprNum;
typedef struct NDExprFc NDExprFc;

enum NDKind {
    NDKind_PROGRAM,
    NDKind_DEFINITION,
    NDKind_ARG,
    NDKind_TYPE,
    NDKind_BODY,
    NDKind_STMT,
    NDKind_EXPR,
};

struct ND {
    NDKind kind;
    TK *token;
};

struct NDProgram {
    NDKind kind;
    TK *token;

    NDDefinition *definitions;
};

struct NDDefinition {
    NDKind kind;
    TK *token;

    NDExprIdent *ident;
    NDArg *args;
    NDType *type;
    NDBody *body;
    NDDefinition *next;
};

struct NDArg {
    NDKind kind;
    TK *token;

    NDExprIdent *ident;
    NDType *type;
    NDArg *next;
};

struct NDType {
    NDKind kind;
    TK *token;
};

struct NDBody {
    NDKind kind;
    TK *token;

    NDStmt *stmt;
};

struct NDStmt {
    NDKind kind;
    TK *token;

    NDExpr *expr;
    NDStmt *next;
};

enum NDExprKind {
    NDExprKind_BINARY,
    NDExprKind_IDENT,
    NDExprKind_NUM,
    NDExprKind_FC,
};

struct NDExpr {
    NDKind kind;
    TK *token;

    NDExprKind exprKind;
    NDExpr *next;
};

struct NDExprBinary {
    NDKind kind;
    TK *token;

    NDExprKind exprKind;
    NDExpr *next;

    NDExpr *lhs;
    NDExpr *rhs;
    char const *ope;
};

struct NDExprIdent {
    NDKind kind;
    TK *token;

    NDExprKind exprKind;
    NDExpr *next;

    char *ident;
};

struct NDExprNum {
    NDKind kind;
    TK *token;

    NDExprKind exprKind;
    NDExpr *next;

    int num;
};

struct NDExprFc {
    NDKind kind;
    TK *token;

    NDExprKind exprKind;
    NDExpr *next;

    NDExprIdent *ident;
    NDExpr *params;
};

NDProgram *parse(TK *token);

//
// codegen.c
//

void codegen(NDProgram *program, char const *filename);
