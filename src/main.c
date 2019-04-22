#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fields.h"

typedef enum boolean{
    false = 0,
    true = 1
} bool;


char ** getFileNames(IS is,int numberFiles){
    char ** files =(char *) malloc(sizeof(char *) * numberFiles);
    for(int i=0;i<numberFiles;i++){
      files[i]=strdup(is->fields[i+2]);
    }
    return  files;
}


void zip(IS is){
    bool isDefaultFileName = false;
    int numberFiles = 0;

    if(!(strcmp(is->fields[is->NF-2],"-o") == 0)){
        isDefaultFileName=true;
    }else {
        isDefaultFileName=false;
        numberFiles = is->NF-4;
        char ** fileNameNames = getFileNames(is,numberFiles);
    }
}

void unzip(){

}

void parse_line(IS is){
    if(strcmp(is->fields[0],"tarsau") == 0){
        if(strcmp(is->fields[1],"-a") == 0){
            //to do: Dosyalar açılacak.
            unzip();
        }else if(strcmp(is->fields[1],"-b") == 0){
            //to do: Dosyalar birleştirilecek.
            zip(is);
        }else {
            printf("Lütfen düzgün bir işlem tipi giriniz.\n");
        }
    } else{
      printf("Lütfen düzgün bir komut giriniz.\n");
    }
}

int main(int argc, char **argv) {
    IS is = new_inputstruct(NULL);
    while (1){
        get_line(is);
        parse_line(is);
    }
}