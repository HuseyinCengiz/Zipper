#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/fields.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define HEADER_LENGTH 10000
#define BODY_LINE_LENGTH 10000
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
int getNumberFiles(IS is){
    if(strcmp(is->fields[is->NF - 2], "-o") != 0){
        if(strcmp(is->fields[is->NF - 1], "-o") == 0){
            return is->NF-3;
        }
        return is->NF-2;
    }else {
        return is->NF-4;
    }
}
char * checkDefaultFileName(IS is) {
   if(strcmp(is->fields[is->NF - 2], "-o") != 0){
        return strdup("a.sau");
    }else {
        return strdup(is->fields[is->NF-1]);
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
bool isBinaryFile(char * fileName){
    struct stat fileStat;
    if(stat(fileName,&fileStat) < 0){
        printf("Dosya okumada hata!");
        exit(EXIT_FAILURE);
    }
    if(fileStat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
        return true;
    }else {
        return false;
    }
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
    strcpy(my_header,"");
    for(int i = 0; i < numberFiles; i++) {
        IS is = new_inputstruct(fileNames[i]);
        char * permission = readPermission(fileNames[i]);
        int fileSize = readSize(fileNames[i]);
        sprintf(line,"%s,%s,%d|",fileNames[i],permission,fileSize);
        strcat(my_header,line);
    }
}
void writeToFile(char * fileName,char **fileNames,int numberFiles){
    FILE *fp;
    fp = fopen(fileName,"wa");
    fprintf(fp,"%010d|%s",strlen(my_header) + 11,my_header);
    for (int i = 0; i < numberFiles; ++i) {
        IS myFile = new_inputstruct(fileNames[i]);
        while (get_line(myFile) >= 0) {
            fprintf(fp,"%s",strdup(myFile->text1));
        }
    }
    fclose(fp);
}
void checkIsBinary(char **fileNames,int numberFiles){
    for(int i=0;i<numberFiles;i++){
        if(isBinaryFile(fileNames[i]))
        {
            printf("%s giriş dosyası biçimi uyumsuz!",fileNames[i]);
            exit(1);
        }
    }
}
bool zipControl(char **fileNames,int numberFiles){
    if(numberFiles>32){
        printf("Giriş dosyalarının adedi 32'den fazla olamaz");
        return false;
    }
    checkIsBinary(fileNames,numberFiles);
    if(calculateSizeofFiles(fileNames,numberFiles)>209715200){
        printf("Giriş dosyalarının toplam boyutu 200 MByte'dan fazla olamaz.");
        return false;
    }
    return true;
}
void zip(IS is){
    char * fileName = checkDefaultFileName(is);
    int numberFiles = getNumberFiles(is);
    char ** fileNames = getFileNames(is,numberFiles);
    if(zipControl(fileNames,numberFiles)){
        makeHeader(fileNames,numberFiles);
        writeToFile(fileName,fileNames,numberFiles);
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