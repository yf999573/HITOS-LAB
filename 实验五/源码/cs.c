#define   __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER 520

int main()
{
    int i;
    for( i = 0 ; i < NUMBER; i++)
    {
        printf("Consumer: %d\n", i);
    }
    return 0;
}
