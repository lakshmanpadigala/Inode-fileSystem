#include <bits/stdc++.h>
#include<iostream>
#include<stdio.h>

using namespace std;

#ifndef _INODE_
#define _INODE_

#define BLOCK_SIZE 1024
#define NUMBER_OF_INODE 100
#define NUMBER_OF_DISK_BLOCKS 500

struct inode{
    int size;
    char fileName[8];
    int first_disk_block_number;
};

struct disk_block{
    int next_block_number;
    char data[1024];
};

struct file{
    char fname[8];
    int mode;
    int inode_number;
};

struct file_inode_map{
    char filename[8];
};

struct main_block{
  struct file_inode_map files_list[NUMBER_OF_INODE];
  int file_count;
  struct file open_files[16];
  int open_count;
  bool open_files_flags[16];
  bool free_inodes[NUMBER_OF_INODE];
  bool free_disk_blocks[NUMBER_OF_DISK_BLOCKS];
};


int create_filesystem(string name);
int mount_disk(string name);
int unmount_disk(string name);
int next_free_inode();
int open_file(string name);
int close_file(string name);
int create_file(string name);
int next_free_disk_block();
int next_free_inode();
int next_free_open_file();
int write_file(int file_descriptor);
int append_file(int file_descriptor);

#endif