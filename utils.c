#include "rec.h"

char *readFile(char const *filename) {
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Cannot open the file.\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(sizeof(char) * size);
    if (buffer == NULL) {
        fprintf(stderr, "malloc error\n");
        return NULL;
    }

    size_t read_size = fread(buffer, sizeof(char), size, fp);
    if (read_size != size) {

        fprintf(stderr, "fread error\n");
        return NULL;
    }
    fclose(fp);

    return buffer;
}

char *takeSubString(char const *string, int len) {
    char *sub = calloc(len + 1, sizeof(char));
    memcpy(sub, string, len);
    return sub;
}

char *addExtension(char const *filename, char const *extension) {
    int cap = strlen(filename) + strlen(extension) + 2;
    char *string = malloc(sizeof(char) * cap);
    snprintf(string, cap, "%s.%s", filename, extension);
    return string;
}

void error(char const *message) {
    fprintf(stderr, message);
    exit(1);
}
