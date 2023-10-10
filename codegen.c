#include "rec.h"

static FILE *fp;

static void programgen(NDProgram *program);
static void definitiongen(NDDefinition *definition);
static void arggen(NDArg *arg);
static void typegen(NDType *type);
static void bodygen(NDBody *body);
static void stmtgen(NDStmt *stmt);
static void exprgen(NDExpr *expr);
static void numgen(NDExprNum *num);
static void identgen(NDExprIdent *ident);
static void fcgen(NDExprFc *fc);

static void programgen(NDProgram *program) {
    NDDefinition *definiton = program->definitions;
    while (definiton) {
        definitiongen(definiton);
        definiton = definiton->next;
    }
}

static void definitiongen(NDDefinition *definition) {
    typegen(definition->type);
    fprintf(fp, " ");
    identgen(definition->ident);
    fprintf(fp, "(");
    NDArg *arg = definition->args;
    while (arg) {
        arggen(arg);
        arg = arg->next;
        if (arg) {
            fprintf(fp, ", ");
        }
    }
    fprintf(fp, ") {\n");
    bodygen(definition->body);
    fprintf(fp, "}\n");
}

static void arggen(NDArg *arg) {
    typegen(arg->type);
    fprintf(fp, " ");
    identgen(arg->ident);
}

static void typegen(NDType *type) { fprintf(fp, "int"); }

static void bodygen(NDBody *body) {
    NDStmt *stmt = body->stmt;
    while (stmt) {
        stmtgen(stmt);
        fprintf(fp, ";\n");
        stmt = stmt->next;
    }
}

static void stmtgen(NDStmt *stmt) {
    fprintf(fp, "return ");
    exprgen(stmt->expr);
}

static void exprgen(NDExpr *expr) {
    switch (expr->exprKind) {
    case NDExprKind_NUM:
        numgen((NDExprNum *)expr);
        break;
    case NDExprKind_IDENT:
        identgen((NDExprIdent *)expr);
        break;
    case NDExprKind_FC:
        fcgen((NDExprFc *)expr);
        break;
    }
}

static void numgen(NDExprNum *num) { fprintf(fp, "%i", num->num); }

static void identgen(NDExprIdent *ident) { fprintf(fp, "%s", ident->ident); }

static void fcgen(NDExprFc *fc) {
    identgen(fc->ident);
    fprintf(fp, "(");
    NDExpr *expr = fc->params;
    while (expr) {
        exprgen(expr);
        expr = expr->next;
        if (expr) {
            fprintf(fp, ", ");
        }
    }
    fprintf(fp, ")");
}

void codegen(NDProgram *program, char const *filename) {
    fp = fopen(addExtension(filename, "c"), "w");

    programgen(program);

    fclose(fp);
}