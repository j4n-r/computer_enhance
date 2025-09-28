/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "sim86_shared.h"

/* #pragma comment (lib, "sim86_shared_debug.lib") */
#define ASM_BUF_SIZE 512
// typedef enum {
//     ax, // 0
//     bx,// 1
//     cx, // 2
//     dx, // 3
//     sp, // 4 
//     bp, // 5
//     si, // 6
//     di, // 7

// } reg_idx;

static uint32_t reg[8] = {0};

void print_reg() {
    printf("\nFinal registers:\n"
           "ax: 0x%04X (%d)\n"
           "bx: 0x%04X (%d)\n"
           "cx: 0x%04X (%d)\n"
           "dx: 0x%04X (%d)\n"
           "sp: 0x%04X (%d)\n"
           "bp: 0x%04X (%d)\n"
           "si: 0x%04X (%d)\n"
           "di: 0x%04X (%d)\n"
           ,reg[0], reg[0], reg[1], reg[1], reg[2], reg[2], reg[3], reg[3]
           ,reg[4], reg[4], reg[5], reg[5], reg[6], reg[6], reg[7], reg[7]);
}          

void handle_move_instruction(instruction *Decoded) {
    uint32_t dest_idx = Decoded->Operands[0].Register.Index-1;

    if (Decoded->Operands[1].Type == Operand_Immediate) {
        uint32_t new_value = Decoded->Operands[1].Immediate.Value;

        printf("%s %s, %d ; %s:0x%X->0x%X\n", 
            Sim86_MnemonicFromOperationType(Decoded->Op),
            Sim86_RegisterNameFromOperand(&Decoded->Operands[0].Register),
            Decoded->Operands[0].Immediate.Value,
            Sim86_MnemonicFromOperationType(Decoded->Op),
            reg[dest_idx], new_value);

        reg[dest_idx] = new_value;
    } else {
        uint32_t new_value = reg[Decoded->Operands[1].Register.Index - 1];

        printf("%s %s, %s ; %s:0x%X->0x%X\n", 
            Sim86_MnemonicFromOperationType(Decoded->Op),
            Sim86_RegisterNameFromOperand(&Decoded->Operands[0].Register),
            Sim86_RegisterNameFromOperand(&Decoded->Operands[1].Register),
            Sim86_MnemonicFromOperationType(Decoded->Op),
            reg[dest_idx], new_value);

        reg[dest_idx] = new_value;
    }


};

int main(int argc, char *argv[])
{

    u32 Version = Sim86_GetVersion();
    printf("Sim86 Version: %u (expected %u)\n", Version, SIM86_VERSION);
    if(Version != SIM86_VERSION)
    {
        printf("ERROR: Header file version doesn't match DLL.\n");
        return -1;
    }

    uint8_t buf[ASM_BUF_SIZE] = {0};

    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("fopen failed");
        exit(1);
    }
    fseek(fptr, 0, SEEK_END);
    uint32_t file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, 1, file_size, fptr);
    

    instruction_table Table;
    Sim86_Get8086InstructionTable(&Table);
    printf("8086 Instruction Instruction Encoding Count: %u\n", Table.EncodingCount);
    
    u32 Offset = 0;
    while(Offset < file_size)
    {
        instruction Decoded;
        Sim86_Decode8086Instruction(sizeof(buf) - Offset, buf + Offset, &Decoded);
        if(Decoded.Op)
        {
            Offset += Decoded.Size;
            if (Decoded.Op == Op_mov) {
                handle_move_instruction(&Decoded);
            }
            // printf("Size:%u Op:%s Flags:0x%x\n", Decoded.Size,
            //        Sim86_MnemonicFromOperationType(Decoded.Op), Decoded.Flags);
        }
        else
        {
            printf("Unrecognized instruction\n");
            break;
        }
    }

    print_reg();
    
    return 0;
}
