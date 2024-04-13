int printf(string fmt, ...);

int main()
{
    int i;
    float f;
    string s;
    bool b;

    if (b) {
        {
            f = i + f;
            f = f + i;
            i = i + i;
            f = i + i;
            f = f + f;
        }
        {
            f = i - f;
            f = f - i;
            i = i - i;
            f = i - i;
            f = f - f;
        }
        {
            f = i * f;
            f = f * i;
            i = i * i;
            f = i * i;
            f = f * f;
        }
        {
            f = i / f;
            f = f / i;
            i = i / i;
            f = i / i;
            f = f / f;
        }
        {
            i = -i;
            f = -f;
            f = -i;
        }
    }
    else {
        {
            b = !b;
            b = true;
            b = false;
        }
        {
            s = "hello!";
            s = "bye";
        }
    }

    if (-i + f/2 * (-3 - -f * i / 2.2) - (-1.23 + f/(i+1)) == 2.1) {
        i = i * 2 + 1 - -i / (i + 6);
        f = f * 2 + 1 - -f / (f + 6);
    }

    if (1 != 2);
    if (1 > 2);
    if (i >= f);
    if (f < i);
    if (1 <= 2);
    if (1 <= i);

    return 0;
}