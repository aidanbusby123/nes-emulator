#include<stdio.h>
#include<SDL2/SDL.h>
#include<unistd.h>
#include"common.h"

#define SCREEN_WIDTH (2*WIDTH)
#define SCREEN_HEIGHT (2*HEIGHT)

uint8_t palette[0x3f][3] = {
    {84, 84, 84},
    {0, 30, 116},
    {8, 16, 144},
    {48, 0, 136},
    {68, 0, 100},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {0, 0, 0},
    {},
    {8, 76, 196},
    {48, 50, 236},
    {92, 30, 228},
    {136, 20, 176},
    {160, 20, 100},
    {152, 34, 32},
    {120, 60, 0},
    {84, 90, 0},
    {},
    {8, 124, 0},
    {},
    {},
    {},
    {},
    {},
    {236, 238, 236},
    {},
    {120, 124, 236},
    {},
    {},
    {},
    {},
    {212, 136, 132},
    {},
    {116, 196, 0},
    {76, 208, 32},
    {},
    {},
    {},
    {},
    {},
    {236, 238, 236},
    {},
    {},
    {},
    {},
    {},
    {236, 106, 100}
};

void put_pixel32(SDL_Surface* screen, int x, int y, uint32_t pixel){
    uint32_t *pixels = (uint32_t*)screen->pixels;
    pixels[(y * screen->w) + x] = pixel;
}

void *render(){
    SDL_Window* window = NULL;
    SDL_Surface* screen = NULL;
    //init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    
    //create window
    window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if( window == NULL ){
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    }
    //get window surface
    screen = SDL_GetWindowSurface(window);
    SDL_Renderer *rend = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    int quit = 0;
    SDL_Event e; //event handler

    //uint32_t pixel; // pixel value;
    uint8_t px[3] = {0};
    
    //SDL_RenderSetLogicalSize(rend, SCREEN_WIDTH*SCREEN_WIDTH/WIDTH, SCREEN_HEIGHT*SCREEN_HEIGHT/HEIGHT);
    while (quit == 0){ // game loop
    SDL_RenderClear(rend);
        while (SDL_PollEvent(&e) != 0){
            if (e.type == SDL_QUIT){
                quit = 1;
            }
        }
        while (vclock == 0);
        for (int i = 0; i < SCREEN_HEIGHT; i++){
            for (int n = 0; n < SCREEN_WIDTH; n++){
                // render pixels here
                //pixel = vram[i*n + n];
                //put_pixel32(screen, n, i, pixel);
                px[0] = palette[vbuf[1][((i*WIDTH)/SCREEN_WIDTH)*WIDTH+(n/(SCREEN_WIDTH/WIDTH))]][0];
                px[1] = palette[vbuf[1][((i*WIDTH)/SCREEN_WIDTH)*WIDTH+(n/(SCREEN_WIDTH/WIDTH))]][1];
                px[2] = palette[vbuf[1][((i*WIDTH)/SCREEN_WIDTH)*WIDTH+(n/(SCREEN_WIDTH/WIDTH))]][2];
                SDL_SetRenderDrawColor(rend, px[0], px[1], px[2], 0xff);
                SDL_RenderDrawPoint(rend, n, i);
                
            }
        }
        SDL_RenderPresent(rend);
        vclock = 0;
    
        usleep(17000);
        //SDL_UpdateWindowSurface(window); // update screen
    }
}