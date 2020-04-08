typedef struct test
{
    int p;
    union {
        int a;
        char c;
    } t;

} test;

#include <stdio.h>
#include <stdlib.h>
int main(int argc, char const *argv[])
{

    test t;
    int i;
    t.t.c = 'a';

    for (i = 0; i < 2; i++)
    {
    t.p = i;
        /* code */

        if (t.p == 1)
        {
            printf("a : %d", t.t.a);
        }
        else
        {
            printf("c : %c", t.t.c);
        }
    t.t.a = 6;
    }

    return EXIT_SUCCESS;
}
