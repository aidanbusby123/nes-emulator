#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include"common.h"
#include"funcs.h"

struct ctx ctx = {0};

int main(int argc, char **argv){
    if (argc != 2){
        printf("Usage: ./emu6502 <rom file>\n");
    }
    //initialize vars
    FILE *fp;
    unsigned char *bin;

    ctx.cpu.SP = 0xff;
    ctx.cpu.P |= I;

    //open source file
    fp = open_file(argv[1]);
    //write source file to buffer
    int sz = fsize(fp);
    char c;
    //////////////////////////////////////////////

    pthread_t ppu_t, render_t, ppu_oam_t;

    uint16_t exec_addr = 0x0011; //address to start execution
    uint16_t chr_addr = 0x4000; // address of chr data

    parse_header(&fp, ctx.header);

    bin = malloc(sz * sizeof(unsigned char)); 
    ctx.bin = bin;
    for (int i = ftell(fp); i < sz; i++){ // read ROM file
        c = fgetc(fp);
        bin[i] = (unsigned char)c;
    }
    
    for (int i = 0; i < sz; i++){
        ctx.mem[i] = ctx.bin[i];
    }
    ctx.mem[RESET+1] = exec_addr >> 8; // Reset vector, tells CPU memory location of opcodes to begin executing
    ctx.mem[RESET] = exec_addr & 0b0000000011111111;

    pthread_create(&ppu_t, NULL, ppu, NULL);
    pthread_create(&render_t, NULL, render, NULL);
    pthread_create(&ppu_oam_t, NULL, ppu_oam, NULL);

    for (int i = 0; i < 8192; i++){
        ctx.ppu_mem[i] = ctx.mem[i+chr_addr];
    }

    // CPU "scheduler"
    int res = 0;
    ctx.cpu.PC = ctx.mem[RESET] + (ctx.mem[RESET+1] << 8);
    usleep(1000000);
    while (ctx.cpu.PC < sz){
        printf("%x\n", ctx.cpu.PC);
        res = 0;
        for (int n = 0; ops[n].code < 0xff; n++){
            if (ctx.bin[ctx.cpu.PC] == ops[n].code){
                ops[n].f();
                res = 1;
                usleep(10);
                break;
            }
        }
        if (res == 0){
            ctx.cpu.PC++;
        }
    }
    while(1);
}

void parse_header(FILE** fp, struct ines_header header){
    fseek(*fp, 4, SEEK_CUR);
    ctx.header.prg_rom_sz = fgetc(*fp); // size of prg rom (16kB units)
    ctx.header.chr_rom_sz = fgetc(*fp); // size of chr rom (8kB units)
    fseek(*fp, 10, SEEK_CUR);
}