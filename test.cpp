#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
using namespace std;

int main(){
    // int *ptr = (int*)malloc(sizeof(int));
    // *ptr = 5;
    // cout<<ptr<<endl;
    // cout<<*ptr<<endl;
    // free(ptr);
    // cout<<ptr<<endl;
    // cout<<*ptr<<endl;

    char abc[262144];
    //abc = (char*)malloc(sizeof(char)*262144);
    cout<<"Enter String!"<<endl;
    //cin>>abc;
    cin.getline(abc,262144);
    cout<<"\nSize:"<<strlen(abc)<<endl;

}