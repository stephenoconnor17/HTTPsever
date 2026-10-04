#include "Tokeniser.h"
#include <stdio.h>
#include <stdlib.h>

//ripped this right from my Minimal shell project !
static int is_delim(char c) {
    return c == ' ' || c == '\n' || c == '\t'; 
}

void free_tokens(char** args){
    int i = 0;
    while(*(args + i) != NULL){
        free(*(args + i));
        i++;
    }
    free(args);
}

char** tokenise(char* line){
    int onWord = 0;
    int wordCount = 0;
    int i = 0;
    char current = *(line + i);

    while(current != '\0'){
        if(!is_delim(current) && onWord == 0){
            wordCount++;
            onWord = 1;
        }else if(is_delim(current) && onWord == 1){
            onWord = 0;
        }

        i++;
        current = *(line + i);
    }

    //an dynamic array of dynamic strings.
    //+1 for null to acknowledge end of arr.
    char** arr = malloc(sizeof(char*) * (wordCount + 1));
    if(arr == NULL) return NULL;

    //reset for second pass.
    i = 0;
    onWord = 0;
    current = *(line + i);
    int j = 0;
    int wordLength = 0;

    do{
        if(!is_delim(current) && onWord == 0){
            onWord = 1;
            wordLength++;
        }else if((is_delim(current) || current == '\0') && onWord == 1){
            onWord = 0;
            if((*(arr + j) = malloc(sizeof(char) * (wordLength + 1))) == NULL){
                free_tokens(arr);
                return NULL;
            }
            for(int k = 0; k < wordLength; k++){
                *(*(arr + j)+k) = *(line + i - wordLength + k);
                //jth string, kth char assigned 
                //the line's first character up to i minus length of word + k
            }
            
            //because each string must end in a \0
            *(*(arr + j)+wordLength) = '\0';
            j++;
            wordLength = 0;
        }else if(!is_delim(current)){
            wordLength++;
        }
        
        //to make sure we dont read past end of line.
        if(j < wordCount){
            i++;
            current = *(line + i);
        }
    }while(j < wordCount);

    *(arr + wordCount) = NULL;

    return arr;
}