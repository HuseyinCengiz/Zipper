#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/fields.h"
#include <sys/types.h>
#include <sys/stat.h>

#define HEADER_LENGTH 1000

typedef enum boolean{
    false = 0,
    true = 1
} bool;

static char my_header[HEADER_LENGTH];

int calculateSizeofFiles(char *fileNames[],int fileNumber) {
    struct stat buf;
    int exists,sum=0;
    for(int i=0;i<fileNumber;i++){
       exists=stat(fileNames[i],&buf);
       if(exists >= 0){
           sum+=buf.st_size;
       }
    }
    return sum;
}

char ** getFileNames(IS is,int numberFiles){
    char ** files = (char **) malloc(sizeof(char *) * numberFiles);
    for(int i=0;i<numberFiles;i++){
      files[i]=strdup(is->fields[i+2]);
    }
    return  files;
}

bool checkDefaultFileName(IS is) {
   if(strcmp(is->fields[is->NF - 2], "-o") != 0){
        return true;
    }else {
        return false;
    }
}

char * readPermission(char * fileName){
    struct stat fileStat;
    if(stat(fileName,&fileStat) < 0){
        printf("Dosya okumada hata!");
        exit(EXIT_FAILURE);
    }

    int u = ((fileStat.st_mode & S_IRUSR) ? 4 : 0)  + ((fileStat.st_mode & S_IWUSR) ? 2 : 0) + ((fileStat.st_mode & S_IXUSR) ? 1 : 0);
    int g = ((fileStat.st_mode & S_IRGRP) ? 4 : 0)  + ((fileStat.st_mode & S_IWGRP) ? 2 : 0) + ((fileStat.st_mode & S_IXGRP) ? 1 : 0);
    int o = ((fileStat.st_mode & S_IROTH) ? 4 : 0)  + ((fileStat.st_mode & S_IWOTH) ? 2 : 0) + ((fileStat.st_mode & S_IXOTH) ? 1 : 0);

    char* uString = malloc(sizeof(char));
    char* gString = malloc(sizeof(char));
    char* oString = malloc(sizeof(char));
    sprintf(uString, "%d", u);
    sprintf(gString, "%d", g);
    sprintf(oString, "%d", o);

    char * permission = malloc(sizeof(char) * 5);
    strcpy(permission,"0");
    strcat(permission,uString);
    strcat(permission,gString);
    strcat(permission,oString);
    strcat(permission,"\0");
    return permission;
}
int readSize(char * fileName){
    struct stat fileStat;
    if(stat(fileName,&fileStat) < 0){
        printf("Dosya okumada hata!");
        exit(EXIT_FAILURE);
    }
    return fileStat.st_size;
}

void makeHeader(char *fileNames[],int numberFiles){
    char line[1000];
    for(int i = 0; i < numberFiles; i++) {
        IS is = new_inputstruct(fileNames[i]);
        char * permission = readPermission(fileNames[i]);
        int fileSize = readSize(fileNames[i]);
        sprintf(line,"%s,%s,%d|",fileNames[i],permission,fileSize);
        strcat(my_header,line);
    }
}

void zip(IS is){
    char *fileName="a.sau";
    bool isDefault = checkDefaultFileName(is);
    int numberFiles = isDefault ? is->NF - 2 : is-> NF - 4 ;
    char ** fileNames = getFileNames(is,numberFiles);
    makeHeader(fileNames,numberFiles);
    FILE *fp;
    fp=fopen(fileName,"wa");
    fprintf(fp,"%010d|%s",strlen(my_header)+11,my_header);
    fclose(fp);

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

char *trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}


int main(int argc, char **argv) {
    IS is = new_inputstruct(NULL);
    while (1){
        get_line(is);
        parse_line(is);
    }
}