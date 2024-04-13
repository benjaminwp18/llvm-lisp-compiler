int printf(string fmt, ...);

int main()
{
    int i;
    i = 0;
    for (; i < 8; i = i + 1)
    {
        printf("Loop Iteration: %d\n", i);
    }

    for (; ; i = i + 1)
    {
        printf("Loop Iteration: %d\n", i);
        if (i > 10) {
            return 0;
        }
    }

    for (;;)
    {
        printf("Loop Iteration: %d\n", i);
        if (i == 30) {
            return 1;
        }

        i = i + 2;
    }
    
    return 0;
}