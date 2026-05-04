#include <stdio.h>

int main() {
    int i = 0;
    {
        int j = 1;
        printf("%d %d\n", i, j);
    }
    return 0;
}
