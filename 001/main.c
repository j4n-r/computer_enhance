
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OP_MOV 0b10001

int openFile(char *fileName, unsigned char *buffer) {
	FILE *file = fopen(fileName, "r");
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

int decodeASM(unsigned char *buffer) {
	char outputBuf[10000];
	char *registers[2][8] = {
	    {"AL", "CL", "DL", "BL", "AH", "CH", "DH",
	     "BH"}, // W = 0 (8-bit registers)
	    {"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"}
	    // W = 1 (16-bit registers)
	};

	for (uint8_t i = 0; i < sizeof(buffer); i++) {
		uint8_t byte = buffer[i];
		int op = byte >> 2;
		if (op == OP_MOV) {
			strcpy("mov ", outputBuf);
		}
		int w = byte & 1 ? 1 : 0;
		int b = byte >> 1 & 1 ? 1 : 0;

		uint8_t byte2 = buffer[i + 1];
		uint8_t mod = byte2 >> 5;
		for (uint8_t i = 0; i < 8; i++) {
			if (i == mod) {
				if (!w) {
					strcpy(registers[0][i], outputBuf);
				} else {
					strcpy(registers[1][i], outputBuf);
				}
			}
		}
	}
	printf("%s", outputBuf);
}

int main() {
	unsigned char *buffer;
	if (openFile("listing37", buffer)) {
		perror("Error reading file main");
		return EXIT_FAILURE;
	}
	decodeASM(buffer);
	return EXIT_SUCCESS;
}
