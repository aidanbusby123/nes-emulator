#include<stdio.h>
#include<stdlib.h>
#include"common.h"

FILE* open_file(char *file){
    FILE* fp;
    if ((fp = fopen(file, "rb")) == NULL)
        return NULL;

    return fp;
}

int fsize(FILE* fp){ // return size
    int sz;
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return sz;
}