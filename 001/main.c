
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int openFile(char* fileName, unsigned char* buffer) {
    FILE* file = fopen(fileName, "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    assert(fseek(file, 0, SEEK_END) == 0);
    long fileSize = ftell(file);
    assert(fileSize != -1);
    rewind(file);

    buffer = malloc(fileSize);
    assert(buffer != NULL);

    size_t bytesRead = fread(file, 1, fileSize, file);
    if (bytesRead != fileSize) {
        perror("Error readind file");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int decodeASM(unsigned char* buffer) {}

int main() {
    unsigned char* buffer;
    if (openFile("listing37", buffer)) {
        perror("Error reading file main");
        return EXIT_FAILURE;
    }
    printf("file read: ");
    return EXIT_SUCCESS;
}
