#include <iostream>


int main()
{
    int* a = new int[10];
    for(int i = 0;i < 10;++i)
        a[i] = 1;
    std::cout << *a << '\n';
    return 0;
}