#include<string.h>
#include<errno.h>
#include<asm/segment.h>

char msg[24];//在内核空间存放字符串

//从用户空间取name写到内核空间msg内
//若name长度超过23则返回-EINVAL
int sys_iam(const char* name){
    char temp[24];
    int i;
    for(i=0;i<24;i++){
        temp[i]=get_fs_byte(name+i);//从用户空间取得name存放在缓冲区temp内
        if(temp[i]=='\0')
            break;
    }

    int len=strlen(temp);
    if(len>23)
        return -EINVAL;//字符过长
    strcpy(msg,temp);//复制缓冲区到msg
    return i;
}

//从内核空间取msg写到用户空间name里
//若msg长度大于等于size则返回-EINVAL
int sys_whoami(char* name, unsigned int size){
    int len=strlen(msg);
    if(len>=size)
        return -EINVAL;//大小不够

    int i;
    for(i=0;i<size;i++){
        put_fs_byte(msg[i],name+i);//取msg放到用户空间的name中
        if(msg[i]=='\0')
            break;
    }
    return i;
}