#include "inode.h"
struct inode* inodes;
struct disk_block* disk_blocks;
struct main_block* fs_block;

int create_filesystem(string name){

    fs_block = (main_block*)malloc(sizeof(main_block));
    inodes = (inode*)malloc(sizeof(inode)*NUMBER_OF_INODE);
    disk_blocks = (disk_block*)malloc(sizeof(disk_block)*NUMBER_OF_DISK_BLOCKS);
    
    fs_block->file_count = -1;
    fs_block->open_count = -1;
    
    for(int i=0;i<16;i++)fs_block->open_files_flags[i] = false;

    for(int i=0;i<NUMBER_OF_INODE;i++){
        fs_block->free_inodes[i] = true;
        inodes[i].first_disk_block_number = -1;
        inodes[i].last_disk_block_number = -1;
        strcpy(inodes[i].fileName, "");
        inodes[i].size = 0;
    }
    for(int i=0;i<NUMBER_OF_DISK_BLOCKS;i++){
        fs_block->free_disk_blocks[i] = true;
        disk_blocks[i].next_block_number = -1;
    }
    
    cout<<sizeof(main_block)+sizeof(inode) * NUMBER_OF_INODE+sizeof(disk_block)*NUMBER_OF_DISK_BLOCKS<<endl;

    FILE *file;
    file = fopen(name.c_str(),"w+");

    fwrite(&fs_block,sizeof(struct main_block),1,file);
    for(int i=0;i<NUMBER_OF_INODE;i++){
        fwrite(&(inodes[i]),sizeof(struct inode),1,file);
    }
    for(int i=0;i<NUMBER_OF_DISK_BLOCKS;i++){
        fwrite(&(disk_blocks[i]),sizeof(struct disk_block),1,file);
    }
    fclose(file);

    free(fs_block);
    free(inodes);
    free(disk_blocks);

    return 0;
}


int mount_disk(string fsname){
    FILE *file;
    file = fopen(fsname.c_str(),"r");

    fs_block = (main_block*)malloc(sizeof(main_block));
    inodes = (inode*)malloc(sizeof(inode)*NUMBER_OF_INODE);
    disk_blocks = (disk_block*)malloc(sizeof(disk_block)*NUMBER_OF_DISK_BLOCKS);

    fread(&fs_block,sizeof(struct main_block),1,file);
    //cout<<sizeof(fs_block);
    for(int i=0;i<NUMBER_OF_INODE;i++){
        fread(&(inodes[i]),sizeof(struct inode),1,file);
        //cout<<sizeof(inodes[i]);
    }
    for(int i=0;i<NUMBER_OF_DISK_BLOCKS;i++){
        fread(&(disk_blocks[i]),sizeof(struct disk_block),1,file);
    }
    fclose(file);
    return 0;
}

int unmount_disk(string fsname){
    FILE *file;
    file = fopen(fsname.c_str(),"w+");

    fwrite(&fs_block,sizeof(struct main_block),1,file);
    for(int i=0;i<NUMBER_OF_INODE;i++){
        fwrite(&(inodes[i]),sizeof(struct inode),1,file);
    }
    for(int i=0;i<NUMBER_OF_DISK_BLOCKS;i++){
        fwrite(&(disk_blocks[i]),sizeof(struct disk_block),1,file);
    }
    fclose(file);

    free(fs_block);
    free(inodes);
    free(disk_blocks);

    return 0;
}
int next_free_inode(){
    for(int i=0;i<NUMBER_OF_INODE;i++){
        if(fs_block->free_inodes[i]){
            fs_block->free_inodes[i] = false;
            return i;
        }
    }
    return -1;
}
int next_free_disk_block(){
    for(int i=0;i<NUMBER_OF_DISK_BLOCKS;i++){
        if(fs_block->free_disk_blocks[i]){
            fs_block->free_disk_blocks[i] = false;
            return i;
        }
    }
    return -1;
}

int create_file(string name){
    int free_inode = next_free_inode();
    for(int i=0;i<=NUMBER_OF_INODE;i++){
        if(!fs_block->free_inodes[i]){
            if(strcmp(fs_block->files_list[i].filename,name.c_str()) == 0){
                cout<<"Already File exists!"<<endl;
                return -1;
            }
        }
    }
    if(free_inode == -1){
        cout<<"No Empty Inodes!"<<endl;
        return -1;
    }
    int free_block = next_free_disk_block();
    if(free_block == -1){
        cout<<"No Empty Block!";
        return -1;
    }
    strcpy(fs_block->files_list[free_inode].filename,name.c_str());
    strcpy(inodes[free_inode].fileName,name.c_str());
    inodes[free_inode].first_disk_block_number = free_block;
    inodes[free_inode].last_disk_block_number = free_block;
    inodes[free_inode].size = 0;
    fs_block->file_count += 1;
    cout<<"File created!"<<endl;
    return 0;
}
int open_file(string name){
    int mode;
    bool found_flag = false;
    int inum;
    for(int i=0;i<NUMBER_OF_INODE;i++){
        if(!fs_block->free_inodes[i]){
            if(strcmp(fs_block->files_list[i].filename,name.c_str()) == 0){
                found_flag = true;
                inum = i;
                break;
            }
        }
    }
    if(!found_flag){
        cout<<"File Doesnot Exist!"<<endl;
        return -1;
    }
    for(int i=0;i<16;i++){
        if(fs_block->open_files_flags[i]){
            if(strcmp(fs_block->open_files[i].fname,name.c_str()) == 0){
                cout<<"File Already Opened in "<<fs_block->open_files[i].mode<<" Mode!"<<endl;
                return i;
            }
        }
    }
    cout<<"Enter mode:";
    cin>>mode;
    // 0: read mode         // 1: write mode            // 2: append mode
    (fs_block->open_count)++;
    int open_position = next_free_open_file();
    if(open_position == -1){
        cout<<"Open files capacity reached!"<<endl;
        return -1;
    }

    strcpy(fs_block->open_files[open_position].fname,name.c_str());
    fs_block->open_files[open_position].mode = mode;
    fs_block->open_files[open_position].inode_number = inum;

    return open_position;
}
int close_file(string name){
    bool found_flag = false;
    int inum;
    for(int i=0;i<NUMBER_OF_INODE;i++){
        if(!fs_block->free_inodes[i]){
            if(strcmp(fs_block->files_list[i].filename,name.c_str()) == 0){
                found_flag = true;
                inum = i;
                break;
            }
        }
    }
    if(!found_flag){
        cout<<"File Doesnot Exist!"<<endl;
        return -1;
    }
    int file_desc = -1;
    for(int i=0;i<16;i++){
        if(fs_block->open_files_flags[i]){
            if(strcmp(fs_block->open_files[i].fname,name.c_str()) == 0){
                file_desc = i;
            }
        }
    }
    if(file_desc == -1){
        cout<<"File is Not Opened!"<<endl;
        return -1;
    }
    fs_block->open_files_flags[file_desc] = false;
    strcpy(fs_block->open_files[file_desc].fname,"");
    cout<<"File Closed Successfully!"<<endl;
    return 0;
}

int next_free_open_file(){
    for(int i=0;i<16;i++){
        if(fs_block->open_files_flags[i]==false){
            fs_block->open_files_flags[i] = true;
            return i;
        }
    }
    return -1;
}

int write_file(int file_descriptor){
    if(fs_block->open_files_flags[file_descriptor] == false){
        cout<<"File Error!"<<endl;
        return -1;
    }
    if(fs_block->open_files[file_descriptor].mode != 1){
        cout<<"File Not Openen in Write Mode!"<<endl;
        return -1;
    }
    int inode_number = fs_block->open_files[file_descriptor].inode_number;
    char temp_data[INPUT_SIZE];//256kb
    cout<<"Enter String!";
    cin.getline(temp_data,INPUT_SIZE);
    int temp_size = strlen(temp_data);
    //inodes[inode_number].size = temp_size;
    int ts;
    if(temp_size > BLOCK_SIZE)ts = BLOCK_SIZE;
    else ts = temp_size;
    int previous_block_number = inodes[inode_number].first_disk_block_number;
    for(int i=0;i<ts;i++){
        disk_blocks[inodes[inode_number].first_disk_block_number].data[i] = temp_data[i];
    }
    inodes[inode_number].size += ts;
    if(temp_size > BLOCK_SIZE ){
        int blocks_required = ( temp_size / BLOCK_SIZE ) + 1 ;
        int next_free_block;
        for(int i=0;i<blocks_required-1;i++){
            next_free_block = next_free_disk_block();
            if(next_free_block == -1){
                cout<<"No Free Block!"<<endl;
                return -1;
            }
            disk_blocks[previous_block_number].next_block_number = next_free_block;
            for(int j=0;j<BLOCK_SIZE;j++){
                disk_blocks[next_free_block].data[j] = temp_data[ (i+1)*BLOCK_SIZE + j ];
            }
            inodes[inode_number].last_disk_block_number = next_free_block;
            inodes[inode_number].size += BLOCK_SIZE;
            previous_block_number = next_free_block;
        }
        if(temp_size % BLOCK_SIZE > 0){
            next_free_block = next_free_disk_block();
            if(next_free_block == -1){
                cout<<"No Free Block!"<<endl;
                return -1;
            }
            disk_blocks[previous_block_number].next_block_number = next_free_block;
            for(int j=0;j<(temp_size%BLOCK_SIZE);j++){
                disk_blocks[next_free_block].data[j] = temp_data[ (blocks_required-1)*BLOCK_SIZE + j ];
            }
            inodes[inode_number].last_disk_block_number = next_free_block;
            inodes[inode_number].size += temp_size%BLOCK_SIZE;
        }
    }
    return 0;
}

int append_file(int file_descriptor){
    if(fs_block->open_files_flags[file_descriptor] == false){
        cout<<"File Error!"<<endl;
        return -1;
    }
    if(fs_block->open_files[file_descriptor].mode != 1){
        cout<<"File Not Openen in Write Mode!"<<endl;
        return -1;
    }
    int inode_number = fs_block->open_files[file_descriptor].inode_number;
    char temp_data[INPUT_SIZE];
    cout<<"Enter String!";
    cin.getline(temp_data,INPUT_SIZE);
    int temp_size = strlen(temp_data);

    int previous_block_number = inodes[inode_number].last_disk_block_number;
    int present_length_last_block = strlen(disk_blocks[previous_block_number].data);
    int remaining_size = BLOCK_SIZE - present_length_last_block;
    for(int i=0;i<remaining_size;i++){
        //todo..................
    }

}

int main(){
    string a = "abc";
    cout<<create_filesystem(a)<<endl;
    cout<<mount_disk(a)<<endl;
    string f = "f1.txt";
    cout<<create_file(f)<<endl;
    f = "f2.txt";
    cout<<create_file(f)<<endl;
    cout<<unmount_disk(string("abc"));
    return 0;
}