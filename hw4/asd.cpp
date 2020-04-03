#include <iostream>
using namespace std;

int main() {
    class T {
        int a[10];
    };
    T *a = 0;
    T *b = new T;
    *(size_t*)b = (size_t)a;
    T *c = (T*)*(size_t*)b;
    cout << c << endl; 
}