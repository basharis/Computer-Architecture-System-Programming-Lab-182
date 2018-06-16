#include <stdio.h>
#include <stdlib.h>
/* change to whatever you like: */
#define MAX_PHYSICAL_MEMORY 4096

int main()
{
    long *M = calloc(1, 8);
    int size = 1;
    int read = 0;
    long num = 0;
    while ((read = scanf(" %ld",&num ))==1 )
    {
        M[size-1] = num;
        M = realloc(M, (++size)*8);
    }
    /*M = realloc(M,(size -1) * 8);*/
    int i = 0;
    while (M[i] || M[i + 1] || M[i + 2])
        if ((M[M[i]] -= M[M[i + 1]]) < 0)
            i = M[i + 2];
        else
            i += 3;
    for (i = 0; i < size; ++i)
    {
        printf("%ld ", M[i]);
    }
    printf("\n");
    return 0;
}