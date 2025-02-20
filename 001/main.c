#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum op_c {
    mov_r_r = 0b100010,
    mov_i_r = 0b1011,

};

// if MOD = 11 these are the same for r/m
const char* REG_W0_NAMES[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* REG_W1_NAMES[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

const char* RM_00_NAMES[] = {"[bx + si]",      "[bx + di]", "[bp + si]",
                             "[bp + di]",      "[si]",      "[di]",
                             "DIRECT ADDRESS", "[bx]"};
const char* MOD_01_10_NAMES[] = {"[bx + si ", "[bx + di ", "[bp + si ",
                                 "[bp + di ", "[si + ",    "[di + ",
                                 "[bp + ",    "[bx + "};

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
        char op_c[4];
        char dst_s[10];
        char src_s[10];

        uint8_t w = buffer[i] & 1;
        uint8_t d = (buffer[i] >> 1) & 1;

        // Register-to-register
        if ((buffer[i] >> 2) == mov_r_r) {
            strcpy(op_c, "mov");
            uint8_t reg = (buffer[i + 1] >> 3) & 0b111;
            uint8_t r_m = buffer[i + 1] & 0b111;
            uint8_t mod = buffer[i + 1] >> 6 & 0b11;

            char reg_s[10];
            char r_m_s[10];

            switch (mod) {
            case 0b00:
                strcpy(r_m_s, RM_00_NAMES[r_m]);
            case 0b11:
                const char** mod_names = (w == 0) ? REG_W0_NAMES : REG_W1_NAMES;
                strcpy(reg_s, mod_names[r_m]);
                strcpy(r_m_s, mod_names[reg]);
                break;
            }

            // CHECK THIS!!!!!!
            // the src and dst are swapped but this should be right
            char* src = (d == 0) ? reg_s : r_m_s;
            char* dst = (d == 0) ? r_m_s : reg_s;
            strcpy(src_s, src);
            strcpy(dst_s, dst);

            // Immediate-to-register
        } else if ((buffer[i] >> 4) == mov_i_r) {
            strcpy(op_c, "mov");
            uint8_t w = buffer[i] >> 3 & 0b1;
            uint8_t reg = buffer[i] & 0b111;

            char tmp[10];
            if (w == 0) {
                strcpy(dst_s, REG_W0_NAMES[reg]);
                uint8_t data = buffer[i + 1];
                sprintf(tmp, "%d", data);
                strcpy(src_s, tmp);
            } else {
                strcpy(dst_s, REG_W1_NAMES[reg]);
                uint16_t data = buffer[i + 1] | (uint16_t)buffer[i + 2] << 8;
                i++;
                sprintf(tmp, "%d", data);
                strcpy(src_s, tmp);
            }

        } else {
            printf("unknown op code");
        };
        printf("%s %s, %s\n", op_c, dst_s, src_s);
        fprintf(file, "%s %s, %s\n", op_c, dst_s, src_s);
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int main() {
    size_t fileSize = 0;
    unsigned char* buffer = openFile("listings/listing39", &fileSize);
    if (buffer == NULL) {
        fprintf(stderr, "Error reading file in main\n");
        return EXIT_FAILURE;
    }

    decodeASM(buffer, fileSize);

    free(buffer);
    return EXIT_SUCCESS;
}
