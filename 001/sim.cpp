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

#include "../sim86_shared.h"
/* #pragma comment (lib, "sim86_shared_debug.lib") */
#define ASM_BUF_SIZE 512

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

    FILE *fptr = fopen("listings/listing_0043_immediate_movs", "r");
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
            printf("Size:%u Op:%s Flags:0x%x\n", Decoded.Size, Sim86_MnemonicFromOperationType(Decoded.Op), Decoded.Flags);
        }
        else
        {
            printf("Unrecognized instruction\n");
            break;
        }
    }
    
    return 0;
}
