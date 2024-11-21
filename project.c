#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Ensure PC is within valid bounds (memory size is 64 KB)
    if (PC < 0x0000 || PC >= 0x10000) {
        return 1; // Halt
    }

    // Check for word alignment
    if (PC % 4 != 0) {
        return 1; // Halt
    }

    // Fetch the instruction
    *instruction = Mem[PC >> 2]; // Convert byte address to word index

    return 0; // Success
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // Extract opcode (bits 31-26)
    *op = (instruction >> 26) & 0x3F;

    // Extract register fields
    *r1 = (instruction >> 21) & 0x1F; // Bits 25-21
    *r2 = (instruction >> 16) & 0x1F; // Bits 20-16
    *r3 = (instruction >> 11) & 0x1F; // Bits 15-11

    // Extract function code (bits 5-0)
    *funct = instruction & 0x3F;

    // Extract offset (bits 15-0)
    *offset = instruction & 0xFFFF;

    // Extract jump target (bits 25-0)
    *jsec = instruction & 0x03FFFFFF;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // Clear all control signals
    memset(controls, 0, sizeof(struct_controls));

    switch (op) {
        case 0x00: // R-type
            controls->RegDst = 1;
            controls->ALUSrc = 0;
            controls->MemtoReg = 0;
            controls->RegWrite = 1;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->ALUOp = 7; // R-type ALU operation determined by funct
            controls->Jump = 0;
            break;

        case 0x08: // ADDI
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            controls->ALUOp = 0; // Addition
            break;

        case 0x23: // LW
            controls->ALUSrc = 1;
            controls->MemtoReg = 1;
            controls->RegWrite = 1;
            controls->MemRead = 1;
            controls->ALUOp = 0; // Addition
            break;

        case 0x2B: // SW
            controls->ALUSrc = 1;
            controls->MemWrite = 1;
            controls->ALUOp = 0; // Addition
            break;

        case 0x0F: // LUI
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            controls->ALUOp = 6; // Shift left 16
            break;

        case 0x04: // BEQ
            controls->Branch = 1;
            controls->ALUSrc = 0;
            controls->ALUOp = 1; // Subtraction
            break;

        case 0x02: // J
            controls->Jump = 1;
            break;

        default:
            return 1; // Halt condition (invalid opcode)
    }

    return 0; // Success 
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

