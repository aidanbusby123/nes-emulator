#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"common.h"
#include"funcs.h"

extern struct ctx ctx;

//Stack operations
void push(uint8_t x){ // stack push
    ctx.mem[ctx.cpu.SP + 0x100] = x;
    ctx.cpu.PC--;
}

uint8_t pop(void){ // stack pop
    uint8_t x = ctx.mem[ctx.cpu.SP + 0x100];
    ctx.cpu.PC++;
    return x;
}

/////////////////////////////////////////////////////////

uint16_t w_addr;

void BRK(void){ // Break
    if ((ctx.cpu.P & I) == 0b00000000){
        ctx.cpu.P = B | ctx.cpu.P;
        ctx.cpu.P = I | ctx.cpu.P;
        ctx.cpu.P |= 0b00100000;
        push(ctx.cpu.PC >> 8);
        push((uint8_t)ctx.cpu.PC);
        ctx.cpu.PC = ctx.mem[0xfffe] + (ctx.mem[0xffff] << 8);
    } else{
        ctx.cpu.PC += 1;
    }
    //printf("BRK\n");
}

void ASL_A(void){ // Arithmetic Shift Left (Absolute)
    ctx.cpu.P = (C | ((ctx.cpu.A >> 7) << 7)) | ctx.cpu.P;
    ctx.cpu.A = ctx.cpu.A << 1;
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^ Z;
    else
        ctx.cpu.P & (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    ctx.cpu.PC += 1;
    //printf("ASL A\n");
}

void JSR(void){ // Jump to SubRoutine
    push((ctx.cpu.PC+2) >> 8);
    ctx.cpu.PC = ctx.bin[ctx.cpu.PC + 1] + (ctx.bin[ctx.cpu.PC + 2] << 8);
    //printf("JSR $%04x\n", ctx.cpu.PC);
}

void BIT_Z(void){
    uint8_t temp = (ctx.mem[ctx.bin[ctx.cpu.PC+1]] & ctx.cpu.A);
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);

    ctx.cpu.P = (N & temp) | ctx.cpu.P;
    if (temp < -128 || temp > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    //printf("BIT $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void BIT_A(void){
    uint8_t temp = (ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.bin[ctx.cpu.PC+2] << 8]) & ctx.cpu.A;
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);

    ctx.cpu.P = (N & temp) | ctx.cpu.P;
    if ((int8_t)temp < -128 || (int8_t)temp > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    ctx.cpu.PC += 2;
    //printf("BIT $%04x\n", ctx.bin[ctx.cpu.PC+1] + ctx.bin[ctx.cpu.PC+2]);
}

void JMP(void){ // JuMP
    ctx.cpu.PC = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8);
    //printf("JMP $%04x\n", ctx.cpu.PC);
}

void ADC_IX(void){ // ADd with Carry (Indirect, X)
    ctx.cpu.A = ctx.cpu.A + ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.X + (ctx.bin[ctx.cpu.PC+2] << 8)] + (ctx.cpu.P & C);
    if (ctx.cpu.A == 0)
        ctx.cpu.A ^= Z;
    else
        ctx.cpu.A &= (~Z);
    
    ctx.cpu.P |= (N & ctx.cpu.A);
    if ((int8_t)ctx.cpu.A < -128 || (int8_t)ctx.cpu.A > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    ctx.cpu.PC += 3;
}

void ADC_Z(void){ // ADd with Carry (Zero Page)
    ctx.cpu.A = ctx.cpu.A + ctx.mem[ctx.bin[ctx.cpu.PC+1]] + (ctx.cpu.P & C);
    if (ctx.cpu.A == 0)
        ctx.cpu.A ^= Z;
    else
        ctx.cpu.A &= (~Z);
    
    ctx.cpu.P |= (N & ctx.cpu.A);
    if ((int8_t)ctx.cpu.A < -128 || (int8_t)ctx.cpu.A > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    ctx.cpu.PC += 2;
}

void ADC(void){ // ADd with Carry
    ctx.cpu.A = ctx.cpu.A + ctx.bin[ctx.cpu.PC+1] + (ctx.cpu.P & C);
    if (ctx.cpu.A == 0)
        ctx.cpu.A ^= Z;
    else
        ctx.cpu.A &= (~Z);
    
    ctx.cpu.P |= (N & ctx.cpu.A);
    if ((int8_t)ctx.cpu.A < -128 || (int8_t)ctx.cpu.A > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    ctx.cpu.PC += 2;
}

void ADC_A(void){ // ADd with Carry (Absolute)
    ctx.cpu.A = ctx.cpu.A + ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)] + (ctx.cpu.P & C);
    if (ctx.cpu.A == 0)
        ctx.cpu.A ^= Z;
    else
        ctx.cpu.A &= (~Z);
    
    ctx.cpu.P |= (N & ctx.cpu.A);
    if ((int8_t)ctx.cpu.A < -128 || (int8_t)ctx.cpu.A > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    ctx.cpu.PC += 3;
}

void ADC_IY(void){ // ADd with Carry (Indirect, Y)
    ctx.cpu.A = (uint8_t)(ctx.cpu.A + (int16_t)ctx.mem[(ctx.mem[ctx.bin[ctx.cpu.PC+1]] + ctx.cpu.Y + ((ctx.mem[ctx.bin[ctx.cpu.PC+1]+1] + (ctx.mem[(ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.Y])] >> 7)) << 8))] + (ctx.cpu.P & C));
    if (ctx.cpu.A == 0)
        ctx.cpu.A ^= Z;
    else
        ctx.cpu.A &= (~Z);
    
    ctx.cpu.P |= (N & ctx.cpu.A);
    if ((int8_t)ctx.cpu.A < -128 || (int8_t)ctx.cpu.A > 127)
        ctx.cpu.P |= V;
    else
        ctx.cpu.P &= (~V);
    ctx.cpu.PC += 3;
}

void STY_Z(void){ // STore Y register (Zero Page)
    ctx.mem[ctx.bin[ctx.cpu.PC+1]] = ctx.cpu.Y;
    w_addr = ctx.bin[ctx.cpu.PC+1];
    //printf("STY $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC+=2;
}

void STA_Z(void){ // STore A register (Zero Page)
    ctx.mem[ctx.bin[ctx.cpu.PC+1]] = ctx.cpu.A;
    w_addr = ctx.bin[ctx.cpu.PC+1];
    //printf("STA $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC+=2;
}

void STX_Z(void){ // STore X register (Zero Page)
    ctx.mem[ctx.bin[ctx.cpu.PC+1]] = ctx.cpu.X;
    w_addr = ctx.bin[ctx.cpu.PC+1];
    //printf("STX $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC+=2;
}

void STY_A(void){ // STore Y register (Absolute)
    ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)] = ctx.cpu.Y;
    w_addr = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8);
    //printf("STY $%04x\n", ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8));
    ctx.cpu.PC += 3;
}

void STA_A(void){ // STore A register (Absolute)
    ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)] = ctx.cpu.A;
    w_addr = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8);
    //printf("STA $%04x\n", ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8));
    ctx.cpu.PC+=3;
}

void STX_A(void){ // STore X register (Absolute)
    ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)] = ctx.cpu.X;
    w_addr = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8);
    //printf("STX $%04x\n", ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8));
    ctx.cpu.PC += 3;
}

void STA_ZX(void){ // STore A register (Zero Page, X)
    ctx.mem[ctx.bin[ctx.cpu.PC+1]+(int8_t)ctx.cpu.X] = ctx.cpu.A;
    ctx.cpu.PC += 2;
}

void STA_AX(void){ // STore Accumulator (Absolute, X)
    ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8) + ctx.cpu.X] = ctx.cpu.A;
    w_addr = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8) + ctx.cpu.X;
    ctx.cpu.PC += 3;
}

void LDY_Z(void){ // LoaD Y register (Zero Page)
    ctx.cpu.Y = ctx.mem[ctx.bin[ctx.cpu.PC+1]];
    if (ctx.cpu.Y == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.Y) | ctx.cpu.P;
    //printf("LDY $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void LDA_Z(void){ // LoaD Accumulator (Zero Page)
    ctx.cpu.A = ctx.mem[ctx.bin[ctx.cpu.PC+1]];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    //printf("LDA $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void LDX_Z(void){ // LoaD X register (Zero Page)
    ctx.cpu.X = ctx.mem[ctx.bin[ctx.cpu.PC+1]];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.X) | ctx.cpu.P;
    //printf("LDX $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
    
}

void LDY(void){ // LoaD Y register (Immediate)
    ctx.cpu.Y = ctx.bin[ctx.cpu.PC+1];
    if (ctx.cpu.Y == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.Y) | ctx.cpu.P;
    ctx.cpu.PC += 2;
    //printf("LDY #$%02x\n", ctx.cpu.Y);
}

void LDA_IX(void){ // LoaD Accumulator (Indirect, X)
    ctx.cpu.A = ctx.mem[(ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.X] + (ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.X + 1] << 8))];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    //printf("LDA ($%02x,X)  (A = %02x)\n", ctx.bin[ctx.cpu.PC+1], ctx.cpu.A);
    ctx.cpu.PC += 2;
}

void LDX(void){ // LoaD X register (Immediate)
    ctx.cpu.X = ctx.bin[ctx.cpu.PC+1];
    if (ctx.cpu.X == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.X) | ctx.cpu.P;
    ctx.cpu.PC += 2;
    //printf("LDX #$%02x\n", ctx.cpu.X);
}

void LDA(void){ // LoaD Accumulator (Immediate)
    ctx.cpu.A = ctx.bin[ctx.cpu.PC+1];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    ctx.cpu.PC += 2;
    //printf("LDA #$%02x\n", ctx.cpu.A);
}

void LDY_A(void){ // LoaD Y register (Absolute)
    ctx.cpu.Y = ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)];
    if (ctx.cpu.Y == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.Y) | ctx.cpu.P;
    //printf("LDY $%04x\n", ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8));
    ctx.cpu.PC += 3;
}

void LDA_A(void){ // LoaD Accumulator (Absolute)
    ctx.cpu.A = ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    //printf("LDA $%04x\n", ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8));
    ctx.cpu.PC += 3;
}

void LDX_A(void){ // LoaD X register (Absolute)
    ctx.cpu.X = ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8)];
    if (ctx.cpu.X == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.X) | ctx.cpu.P;
    //printf("LDX $%04x\n", ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8));
    ctx.cpu.PC += 3;
}

void LDA_IY(void){ // LoaD Accumulator (Indirect, Y)
    ctx.cpu.A = ctx.mem[(ctx.mem[ctx.bin[ctx.cpu.PC+1]] + ctx.cpu.Y + ((ctx.mem[ctx.bin[ctx.cpu.PC+1]+1] + (ctx.mem[(ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.Y])] >> 7)) << 8))];
        if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P; 
    //printf("LDA ($%02x),Y  (A = %02x)\n", ctx.bin[ctx.cpu.PC+1], ctx.cpu.A);
    ctx.cpu.PC += 2;
}

void LDY_ZX(void){ // LoaD Y register (Zero Page, X)
    ctx.cpu.Y = ctx.mem[ctx.bin[ctx.cpu.PC+1]] + ctx.cpu.X;
    if (ctx.cpu.Y == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.Y) | ctx.cpu.P;
    //printf("LDY $%02x,X\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void LDA_ZX(void){ // LoaD Accumulator (Zero Page, X)
    ctx.cpu.A = ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.X];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    //printf("LDA $%02x,X\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void LDX_ZY(void){ // LoaD X register (Zero Page, Y)
    ctx.cpu.X = ctx.mem[ctx.bin[ctx.cpu.PC+1] + ctx.cpu.Y];
    if (ctx.cpu.X == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.X) | ctx.cpu.P;
    //printf("LDX $%02x,Y\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void LDA_AY(void){ // LoaD Accumulator (Absolute, Y)
    ctx.cpu.A = ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8) + ctx.cpu.Y];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    //printf("LDA $%04x, Y\n", ctx.bin[ctx.cpu.PC + 1]);
    ctx.cpu.PC += 3;
}

void LDA_AX(void){ // LoaD Accumulator (Absolute, X)
    ctx.cpu.A = ctx.mem[ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8) + ctx.cpu.X];
    if (ctx.cpu.A == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.A) | ctx.cpu.P;
    //printf("LDA $%04x, X\n", ctx.bin[ctx.cpu.PC + 1]);
    ctx.cpu.PC += 3;
}

void INY(void){ // INcrement Y
    ctx.cpu.Y += 1;
    if (ctx.cpu.Y == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P = (N & ctx.cpu.X) | ctx.cpu.P;
    ctx.cpu.PC += 1;
}

void BNE(void){ // Branch on Not Equal
    //printf("BNE $%02x\n", ctx.bin[ctx.cpu.PC+1]);
    if ((Z & ctx.cpu.P) == 0){
        ctx.cpu.PC += (int8_t)(ctx.bin[ctx.cpu.PC+1]) + 2;
    } else {
        ctx.cpu.PC += 2;
    }
}

void CPX_I(void){ // ComPare X register (Immediate)
    if ((ctx.cpu.X - ctx.bin[ctx.cpu.PC+1]) == 0){
        ctx.cpu.P ^= Z;
    } else {
        ctx.cpu.P &= (~Z);
    }
    ctx.cpu.P = (N & (ctx.cpu.X - ctx.bin[ctx.cpu.PC+1])) | ctx.cpu.P;
    if (ctx.cpu.X > ctx.bin[ctx.cpu.PC+1]){
        ctx.cpu.P ^= C;
    } else {
        ctx.cpu.P &= (~C);
    }
    //printf("CPX #$%02x\n", ctx.bin[ctx.cpu.PC+1]);
    ctx.cpu.PC += 2;
}

void INC_Z(void){ // INCrement memory (Zero Page)
    w_addr = ctx.mem[ctx.bin[ctx.cpu.PC+1]];
    ctx.mem[ctx.bin[ctx.cpu.PC+1]] += 1;
    if (ctx.mem[ctx.bin[ctx.cpu.PC+1]] == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P |= (N & ctx.mem[ctx.bin[ctx.cpu.PC+1]]);
    ctx.cpu.PC += 2;
}

void INX(void){ // INcrement X
    ctx.cpu.X += 1;
    if (ctx.cpu.X == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P = (N & ctx.cpu.X) | ctx.cpu.P;
    //printf("INX\n");
    ctx.cpu.PC += 1;
}

void NOP(void){ // No OPeration
    //printf("NOP\n");
    ctx.cpu.PC += 1;
}

void INC_A(void){ // INCrement memory (Absolute)
    w_addr = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8);
    ctx.mem[w_addr] += 1;
    if (ctx.mem[w_addr] == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P |= (N & ctx.mem[w_addr]);
    ctx.cpu.PC += 3;
}

void BEQ(void){ // Branch on EQual
    //printf("BEQ #$%02x\n", ctx.bin[ctx.cpu.PC+1]);
    if ((Z & ctx.cpu.P) == Z){
        ctx.cpu.PC += (int8_t)(ctx.bin[ctx.cpu.PC+1]) + 2;
    } else {
        ctx.cpu.PC += 2;
    }
}

void INC_ZX(void){ // INCrement memory (Zero Page, X)
    w_addr = ctx.bin[ctx.cpu.PC+1]+ctx.cpu.X;
    ctx.mem[w_addr] += 1;
    if (ctx.mem[w_addr] == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    ctx.cpu.P |= (N & ctx.mem[w_addr]);
    ctx.cpu.PC += 3;
}

void INC_AX(void){ // INCrement memory (Absolute, X)
    w_addr = ctx.bin[ctx.cpu.PC+1] + (ctx.bin[ctx.cpu.PC+2] << 8) + ctx.cpu.X;
    ctx.mem[w_addr] += 1;
    if (ctx.mem[w_addr] == 0)
        ctx.cpu.P ^= Z;
    else
        ctx.cpu.P &= (~Z);
    
    ctx.cpu.P |= (N & ctx.mem[w_addr]);
    ctx.cpu.PC += 3;
}