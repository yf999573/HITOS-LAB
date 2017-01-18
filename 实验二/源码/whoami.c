#include<errno.h>
#include<stdio.h>
#define __LIBRARY__
#include<unistd.h>

_syscall2(int,whoami,char*,name,int,size);

int main(int argc,char* argv[]){
    char name[24];
    whoami(name,24);
    printf("%s\n",name);    
    return 0;
}
