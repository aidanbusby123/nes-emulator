#ifndef COMMON_H
#define COMMON_H

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"funcs.h"

#define MEM_SZ 0x10000
#define PPU_MEM_SZ 0x03fff

#define C 0b00000001
#define Z 0b00000010
#define I 0b00000100
#define D 0b00001000
#define B 0b00010000
#define V 0b01000000
#define N 0b10000000

// PPU registers
#define PPUCTRL 0x2000
#define PPUMASK 0x2001
#define PPUSTATUS 0x2002
#define OAMADDR 0x2003
#define OAMDATA 0x2004
#define PPUSCROLL 0x2005
#define PPUADDR 0x2006
#define PPUDATA 0x2007
#define OAMDMA 0x4014

//
#define RESET 0xFFFC

//screen constants
#define WIDTH 256
#define HEIGHT 240

struct ppu_s{ // 6502 ppu
    uint16_t ctrl;
    uint16_t mask;
    uint16_t status;
    uint16_t scroll;
    uint16_t addr;
    uint16_t data;
};

void *ppu(); // PPU function
void *ppu_oam(); // function to deal with ppu registers

extern uint8_t vbuf[][256*240]; // video buffer for display
extern int vclock; // video timing clock
extern uint16_t w_addr; // Used to store address that is being written to by CPU. Used to update PPU registers.

struct _6502{ // 6502 cpu
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t SP;
    uint16_t PC;
    uint8_t P;
};

struct ines_header{
    uint8_t prg_rom_sz;
    uint8_t chr_rom_sz;
    uint8_t map_num_low;
    uint8_t ignr_mirror; // ignore mirroring control
    uint8_t trainer; // 512 byte trainer at $7000-$71ff
    uint8_t rom_back; // Battery backed PRG RAM ($6000-$7fff)
    uint8_t vt_mirror; // verticle mirroring
    uint8_t hz_mirror; // horizontal mirroring

    uint8_t map_num_high;
    uint8_t nes_2_format;
    uint8_t playchoice10;
    uint8_t vs_unisystem;
    
};

struct ctx{
    struct _6502 cpu;
    unsigned char mem[MEM_SZ];
    unsigned char ppu_mem[PPU_MEM_SZ];
    uint8_t oam[256];
    unsigned char *buf;
    unsigned char *bin;
    struct ines_header header;
};

extern struct ctx ctx;

typedef struct op{
    uint8_t code;
    void (*f)(void);
}op;

extern op ops[];

FILE* open_file(char *file);
int fsize(FILE* fp);

void parse_header(FILE** fp, struct ines_header header);

void *render(); // rendering function


#endif