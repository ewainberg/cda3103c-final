#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    //Switch operation based on ALUControl (0 to 7)
    switch(ALUControl){
        case 0:
            //Addition
            *ALUresult = A + B;
            break;
        case 1:
            //Subtraction
            *ALUresult = A - B;
            break;
        case 2:
            //Set less than (signed)
            if (((signed) A) < ((signed) B)){
                *ALUresult = 1;
            }
            else {
                *ALUresult = 0;
            }
            break;
        case 3:
            //Set less than (unsigned)
            if (A < B){
                *ALUresult = 1;
            }
            else {
                *ALUresult = 0;
            }
            break;
        case 4:
            //Bitwise AND
            *ALUresult = A & B;
            break;
        case 5:
            //Bitwise OR
            *ALUresult = A | B;
            break;
        case 6:
            //Bitshift left by 16 bits
            *ALUresult = B << 16;
            break;
        case 7:
            //Bitwise NOT
            *ALUresult = ~A;
            break;
    }

    //Set Zero based on ALUresult
    if (*ALUresult == 0) {
        *Zero = 1;
    }
    else {
        *Zero = 0;
    }
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
int instruction_decode(unsigned op, struct_controls *controls)
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

        case 0x0A: // SLTI
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            controls->ALUOp = 2;
            break;

        case 0x0B: // SLTIU
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            controls->ALUOp = 3;
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
    //Copy data from r1 to data1
    *data1 = Reg[r1];

    //Copy data from r2 to data2
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	// Check if the sign bit (bit 15) is 1, indicating a negative number
	if (offset & 0x8000) {
		// Extends with 1s, negative number
		*extended_value = offset | 0xFFFF0000; 

	} else {
		// Extends with 0s, positive number
		*extended_value = offset & 0x0000FFFF;
	}
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //Check if this is an r type
    if (ALUOp < 7) {
        //It isn't an r type, so use whatever is set.
        //If ALUSrc is deasserted, use data2. Otherwise, use extended_value
        if (ALUSrc == 0) {
            ALU(data1, data2, ALUOp, ALUresult, Zero);
        }
        else {
            ALU(data1, extended_value, ALUOp, ALUresult, Zero);
        }
    }
    else {
        //It is an r type, determine which one
        switch (funct) {
            case 0x20:
                //Add
                ALU(data1, data2, 0, ALUresult, Zero);
                break;
            case 0x22:
                //Subtract
                ALU(data1, data2, 1, ALUresult, Zero);
                break;
            case 0x24:
                //And
                ALU(data1, data2, 4, ALUresult, Zero);
                break;
            case 0x25:
                //Or
                ALU(data1, data2, 5, ALUresult, Zero);
                break;
            case 0x2A:
                //Set less than (signed)
                ALU(data1, data2, 2, ALUresult, Zero);
                break;
            case 0x2B:
                //Set less than (unsigned)
                ALU(data1, data2, 3, ALUresult, Zero);
                break;
            default:
                return 1; //Halt, invalid function
        }
    }

    //End function without halt
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // Check if the address is word-aligned and within bounds
    if (ALUresult % 4 != 0 || ALUresult >= 0x10000) {
        return 1; // Halt: address not word-aligned or out of bounds
    }

    if (MemRead) {
        // Read data from memory 
        *memdata = Mem[ALUresult >> 2];
    }

    if (MemWrite) {
        // Write data to memory 
        Mem[ALUresult >> 2] = data2;
    }

    return 0; // Success
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    //If RegWrite is deasserted, then don't write to a register
    if (RegWrite==0) {
        return;
    }

    //If MemtoReg is deasserted, use ALUresult. Otherwise, use memdata
    if (MemtoReg==0) {
        //Using ALUresult

        //If RegDst is 0, write to r2. Otherwise, write to r3
        if (RegDst==0) {
            Reg[r2] = ALUresult; //write ALUresult to r2
        }
        else {
            Reg[r3] = ALUresult; //write ALUresult to r3
        }
    }
    else {
        //Using memdata

        //If RegDst is 0, write to r2. Otherwise, write to r3
        if (RegDst==0) {
            Reg[r2] = memdata; //write memdata to r2
        }
        else {
            Reg[r3] = memdata; //write memdata to r3
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	// Update PC for normal instruction execution
	*PC += 4;

	// Handle Branch
	if (Branch && Zero) {
		*PC += (extended_value << 2);
	}

	// Handle jump
	if (Jump) {
		*PC = (*PC & 0xF0000000) | (jsec << 2); // Set PC to jump target
	}
}

