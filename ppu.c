#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include"common.h"

// pattern tables
#define PATTERN_0 0x0000
#define PATTERN_1 0x1000

// universal palette memory location (used to calculate address of other palettes)
#define UNV_BACKGRND 0x3f00

// name tables
#define NAMETABLE_0 0x2000
#define NAMETABLE_1 0x2400
#define NAMETABLE_2 0x2800
#define NAMETABLE_3 0x2c00

struct ppu_s ppu_s = { // PPU struct
    PPUCTRL,
    PPUMASK,
    PPUSTATUS,
    PPUSCROLL,
    0x0000,
    0
};

uint8_t vbuf[2][256*240];

int vclock = 0;

void *ppu_oam(){
    int ppu_data = 0;
    while (1){
        if (w_addr == PPUADDR){
            ppu_s.addr = (ppu_s.addr << 8) + ctx.mem[PPUADDR];
            w_addr = 0;
        }
        if (w_addr == PPUDATA){
            w_addr = 0;
            ctx.ppu_mem[ppu_s.addr] = ctx.mem[PPUDATA];
            ppu_data = ctx.ppu_mem[ppu_s.addr];
            ppu_s.addr++;
        }
    }
}


void *ppu(){  
    // render pixels to VRAM buffer
    uint64_t tile = 0;
    uint8_t tile_low = 0; //low bits of tile
    uint8_t tile_high = 0; //high bits of tile
    uint16_t tile_palette = 0;
    uint16_t temp[2] = {0};
    
    while (1){
        if (ctx.mem[OAMDMA] > 0){
            for (int i = ctx.mem[OAMADDR]; i <= ctx.mem[OAMADDR] + 0xff; i++){
                ctx.oam[i] = ctx.mem[i];
            }
            ctx.mem[OAMDMA] = 0;
        }
        while (vclock == 1);
        for (int i = 0; i < (WIDTH/8)*(HEIGHT/8); i++){ // background graphics
            temp[0] = ctx.ppu_mem[NAMETABLE_0 + ((WIDTH/8)*(HEIGHT/8)) + ((i/4) % 16) + (i/64)*8];// attribute table entry for current 4x4 tile area
            temp[1] = (temp[0] >> (2*(((2 *(((i%4)/2) + 2*(((i/32)%4)/2)))) % 4))) & 0b00000011;
            //if (temp[1]>0)printf("%x, %x\n", temp[1], temp[0]);
            tile_palette = UNV_BACKGRND+(temp[1]*4); // address of proper palette colors
            for (int n = 0; n < 8; n++){
                for (int k = 0; k < 8; k++){
                    tile_low = /*(ctx.ppu_mem[PATTERN_0 + (((n + i*16)))] >> (7-k)) & 0b00000001;*/(ctx.ppu_mem[PATTERN_1 + ctx.ppu_mem[NAMETABLE_0+i]*16 + n] >> (7-k)) & 0b00000001; // low bit of tile pixel
                    tile_high = /*(ctx.ppu_mem[PATTERN_0 + (n + i*16 + 8)] >> (7-k)) & 0b00000001;*/ (ctx.ppu_mem[PATTERN_1 + ctx.ppu_mem[NAMETABLE_0+i]*16 + n + 8] >> (7-k)) & 0b00000001; // high bit of tile pixel
                    //if(tile_low > 0 || tile_high > 0)printf("%x, %x\n", tile_low, tile_high);
                    vbuf[0][((i/32)*64*32) + (n * WIDTH) + (i%32)*8 + k] = /*ctx.ppu_mem[UNV_BACKGRND];*/ctx.ppu_mem[tile_palette + (tile_low + (tile_high * 2))]; // set current pixel to color represented by tile_low and tile_high  
                }
            }
        }
            
        for (int i = 0; i < WIDTH * HEIGHT; i++){
            vbuf[1][i] = vbuf[0][i];
        }
        vclock = 1;
    }
}

