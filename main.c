#include "rec.h"

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "please filename.\n");
    }
    char const *filename = argv[1];
    char *buffer = readFile(addExtension(filename, "rec"));
    if (buffer == NULL) {
        return 1;
    }

    TK *token = tokenize(buffer);
    NDProgram *program = parse(token);
    codegen(program, filename);

    free(buffer);

    return 0;
}