#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
using namespace std;
int *p1;
int *p2;

int main(){
    p1 = (int*)malloc(sizeof(int)*2);
    p2 = (int*)malloc(sizeof(int)*2);

    p1[0] = 100;
    p2[0] = 250;
    p1[1] = 101;
    p2[1] = 251;

    FILE *file;
    file = fopen("okay","w+");

    fwrite(p1,sizeof(int)*2,1,file);
    fwrite(p2,sizeof(int)*2,1,file);
    fclose(file);
    free(p1);
    free(p2);


    p1 = (int*)malloc(sizeof(int)*2);
    p2 = (int*)malloc(sizeof(int)*2);

    FILE *file1;
    file1 = fopen("okay","r");

    fread(p1,sizeof(int)*2,1,file1);
    fread(p2,sizeof(int)*2,1,file1);

    fclose(file1);

    cout<<p1[0]<<" "<<p2[0]<<endl;
    cout<<p1[1]<<" "<<p2[1]<<endl;
    return 0;
}











// int main(){
//     // int *ptr = (int*)malloc(sizeof(int));
//     // *ptr = 5;
//     // cout<<ptr<<endl;
//     // cout<<*ptr<<endl;
//     // free(ptr);
//     // cout<<ptr<<endl;
//     // cout<<*ptr<<endl;

//     char abc[262144];
//     //abc = (char*)malloc(sizeof(char)*262144);
//     cout<<"Enter String!"<<endl;
//     //cin>>abc;
//     cin.getline(abc,262144);
//     cout<<"abc:"<<abc<<endl;
//     cout<<"\nSize:"<<strlen(abc)<<endl;

// }

// #include <fstream>
// #include <string>

//  // Add this code inside your main() function
// std::ifstream ifs("text.txt");      
// std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());