#include<iostream>
#include<stdio.h>
#include<stdlib.h>
using namespace std;

int main(){
    int *ptr = (int*)malloc(sizeof(int));
    *ptr = 5;
    cout<<ptr<<endl;
    cout<<*ptr<<endl;
    free(ptr);
    cout<<ptr<<endl;
    cout<<*ptr<<endl;

}