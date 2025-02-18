#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char* MOD_W0_NAMES[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

enum op_c {
    mov = 0b100010,
};
const char* MOD_W1_NAMES[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

unsigned char* openFile(const char* fileName, size_t* fileSize) {
    FILE* file = fopen(fileName, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    assert(fseek(file, 0, SEEK_END) == 0);
    long size = ftell(file);
    assert(size != -1);
    rewind(file);

    unsigned char* buffer = (unsigned char*)malloc(size);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, size, file);
    if (bytesRead != (size_t)size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    fclose(file);
    *fileSize = size;
    return buffer;
}

int decodeASM(unsigned char* buffer, size_t size) {

    FILE* file = fopen("output.asm", "w");
    fprintf(file, "bits 16\n\n");

    for (size_t i = 0; i < size; i += 2) {
        int w = buffer[i] & 1;
        int d = (buffer[i] >> 1) & 1;
        enum op_c op = buffer[i] >> 2;

        switch (op) {
        case mov:
            printf("mov ");
            fprintf(file, "mov ");
            break;
        default:
            printf("unknown op code");
        }
        int reg = (buffer[i + 1] >> 3) & 0b111;
        int r_m = buffer[i + 1] & 0b111;

        int src = 0;
        int dst = 0;
        if (d == 0) {
            src = reg;
            dst = r_m;
        } else {
            src = r_m;
            dst = reg;
        }
        if (w == 0) {
            printf("%s, %s\n", MOD_W0_NAMES[dst], MOD_W0_NAMES[src]);
            fprintf(file, "%s, %s\n", MOD_W0_NAMES[dst], MOD_W0_NAMES[src]);
        } else {
            printf("%s, %s\n", MOD_W1_NAMES[dst], MOD_W1_NAMES[src]);
            fprintf(file, "%s, %s\n", MOD_W1_NAMES[dst], MOD_W1_NAMES[src]);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int main() {
    size_t fileSize = 0;
    unsigned char* buffer = openFile("listing38", &fileSize);
    if (buffer == NULL) {
        fprintf(stderr, "Error reading file in main\n");
        return EXIT_FAILURE;
    }

    decodeASM(buffer, fileSize);

    free(buffer);
    return EXIT_SUCCESS;
}
