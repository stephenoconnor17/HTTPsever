#include <stdio.h>
#include <stdlib.h>

char* readin_file(char* fname, long* out_size){
    FILE* fp = fopen(fname, "rb");
    if(!fp){
        perror("fopen");
        return NULL;
    }

    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char* body = malloc(size);
    if(body == NULL){
        fclose(fp);
        return NULL;
    }
    fread(body, 1, size, fp);
    fclose(fp);

    *(out_size) = size;

    return body;
}