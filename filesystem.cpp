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
int close_file(int file_descriptor){
    //bool found_flag = false;
    //int inum;
    // for(int i=0;i<NUMBER_OF_INODE;i++){
    //     if(!fs_block->free_inodes[i]){
    //         if(strcmp(fs_block->files_list[i].filename,name.c_str()) == 0){
    //             found_flag = true;
    //             inum = i;
    //             break;
    //         }
    //     }
    // }
    // if(!found_flag){
    //     cout<<"File Doesnot Exist!"<<endl;
    //     return -1;
    // }
    // int file_desc = -1;
    // for(int i=0;i<16;i++){
    //     if(fs_block->open_files_flags[i]){
    //         if(strcmp(fs_block->open_files[i].fname,name.c_str()) == 0){
    //             file_desc = i;
    //         }
    //     }
    // }
    if(file_descriptor == -1){
        cout<<"File is Not Opened!"<<endl;
        return -1;
    }
    fs_block->open_files_flags[file_descriptor] = false;
    strcpy(fs_block->open_files[file_descriptor].fname,"");
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
        cout<<"File Not Opened in Write Mode!"<<endl;
        return -1;
    }
    int inode_number = fs_block->open_files[file_descriptor].inode_number;
    //char temp_data[INPUT_SIZE];//256kb
    cout<<"Enter String!"<<endl;
    //cin.getline(temp_data,INPUT_SIZE);
    //cout<<temp_data<<endl;
    string temp_data;
    cin>>temp_data;
    //int temp_size = strlen(temp_data);
    int temp_size = temp_data.size();
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
    if(fs_block->open_files[file_descriptor].mode != 2){
        cout<<"File Not Opened in Append Mode!"<<endl;
        return -1;
    }
    int inode_number = fs_block->open_files[file_descriptor].inode_number;
    //char temp_data[INPUT_SIZE];
    cout<<"Enter String!";
    //cin.getline(temp_data,INPUT_SIZE);
    string temp_data;
    cin>>temp_data;
    //int temp_size = strlen(temp_data);
    int temp_size = temp_data.size();

    int previous_block_number = inodes[inode_number].last_disk_block_number;
    int present_length_last_block = strlen(disk_blocks[previous_block_number].data);
    int remaining_size = BLOCK_SIZE - present_length_last_block;
    for(int i=0;i<remaining_size;i++){
        disk_blocks[previous_block_number].data[present_length_last_block + i] = temp_data[i];
    }
    inodes[inode_number].size += remaining_size ;


    temp_size = temp_size - remaining_size;
    int ts;
    if(temp_size > BLOCK_SIZE)ts = BLOCK_SIZE;
    else ts = temp_size;
    int next_free_block = next_free_disk_block();
    if(next_free_block == -1){
        cout<<"No Free Block!"<<endl;
        return -1;
    }
    disk_blocks[previous_block_number].next_block_number = next_free_block;
    for(int i=0;i<ts;i++){
        disk_blocks[next_free_block].data[i] = temp_data[i + remaining_size];
    }
    inodes[inode_number].size += ts;
    inodes[inode_number].last_disk_block_number = next_free_block;
    if(temp_size > BLOCK_SIZE ){
        int blocks_required = ( temp_size / BLOCK_SIZE ) + 1 ;
        next_free_block;
        for(int i=0;i<blocks_required-1;i++){
            next_free_block = next_free_disk_block();
            if(next_free_block == -1){
                cout<<"No Free Block!"<<endl;
                return -1;
            }
            disk_blocks[previous_block_number].next_block_number = next_free_block;
            for(int j=0;j<BLOCK_SIZE;j++){
                disk_blocks[next_free_block].data[j] = temp_data[ (i+1)*BLOCK_SIZE + j + remaining_size];
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
                disk_blocks[next_free_block].data[j] = temp_data[ (blocks_required-1)*BLOCK_SIZE + j + remaining_size];
            }
            inodes[inode_number].last_disk_block_number = next_free_block;
            inodes[inode_number].size += temp_size%BLOCK_SIZE;
        }
    }
    cout<<"Append Done Successfully!";
    return 0;
}

int read_file(int file_descriptor){
    if(fs_block->open_files_flags[file_descriptor] == false){
        cout<<"File Error!"<<endl;
        return -1;
    }
    if(fs_block->open_files[file_descriptor].mode != 0){
        cout<<"File Not Opened in Read Mode!"<<endl;
        return -1;
    }
    int inode_number = fs_block->open_files[file_descriptor].inode_number;

    int current_block = inodes[inode_number].first_disk_block_number;
    while(current_block != -1){
        cout<<disk_blocks[current_block].data;
        current_block = disk_blocks[current_block].next_block_number;        
    }
    return 0;
}

void list_open_files(){
    cout<<"Filename"<<"\t"<<"Mode"<<"\t"<<"Inode Number"<<endl;
    for(int i=0;i<16;i++){
        if(fs_block->open_files_flags[i]==true){
            cout<<fs_block->open_files[i].fname <<"\t\t"<<fs_block->open_files[i].mode<<"\t\t"<<fs_block->open_files[i].inode_number << endl;
        }
    }
}

void list_files(){
    cout<<"Filename"<<"\t"<<"Size"<<"\t"<<"Inode Number"<<endl;
    for(int i=0 ;i<NUMBER_OF_INODE;i++){
        if(!fs_block->free_inodes[i]){
            cout<<inodes[i].fileName<<"\t\t"<<inodes[i].size<<"\t\t"<<i<<endl;
        }
    }
}

int delete_file(string filename){
    bool found_flag = false;
    int inum;
    for(int i=0;i<NUMBER_OF_INODE;i++){
        if(!fs_block->free_inodes[i]){
            if(strcmp(fs_block->files_list[i].filename,filename.c_str()) == 0){
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
    fs_block->free_inodes[inum] = true;
    (fs_block->file_count)--;
    int current_block = inodes[inum].first_disk_block_number;
    inodes[inum].first_disk_block_number = -1;
    inodes[inum].last_disk_block_number = -1;
    memset(fs_block->files_list[inum].filename, 0, sizeof(fs_block->files_list[inum].filename));
    while(current_block != -1){
        memset(disk_blocks[current_block].data,0,BLOCK_SIZE);
        disk_blocks[current_block].next_block_number = -1;
        fs_block->free_disk_blocks[current_block] = true;
        current_block = disk_blocks[current_block].next_block_number;
    }
    cout<<"File Deleted Successfully!"<<endl;
    return 0;
}


int file_utilites(){
    int option,temp;
    string filename,disk_name;
    int file_descriptor;
    while(1){
        cout<<"\n___________________________Enter Option____________________________________"<<endl;
        cout<<"|1.create file      2.open file         3.read file      4.write file     |\n";
        cout<<"|5.append file      6.close file        7.delete file    8.list files     |\n";
        cout<<"|9.list of opened files                 10.unmount disk                   |\n";
        cout<<"|   0: read mode            1: write mode               2: append mode    |\n";
        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cin>>option;
        switch (option)
        {
        case 1:
            cout<<"Enter file name:";
            cin>>filename;
            create_file(filename);
            break;
        case 2:
            cout<<"Enter file name:";
            cin>>filename;
            temp = open_file(filename);
            cout<<"\nFile Descriptor:"<<temp<<"\n";
            break;
        case 3:
            cout<<"Enter file Descriptor:";
            cin>>file_descriptor;
            read_file(file_descriptor);
            break;
        case 4:
            cout<<"Enter file Descriptor:";
            cin>>file_descriptor;
            write_file(file_descriptor);
            break;
        case 5:
            cout<<"Enter file Descriptor:";
            cin>>file_descriptor;
            append_file(file_descriptor);
            break;
        case 6:
            cout<<"Enter file Descriptor:";
            cin>>file_descriptor;
            close_file(file_descriptor);
            break;
        case 7:
            cout<<"Enter file name:";
            cin>>filename;
            delete_file(filename);
            break;
        case 8:
            list_files();
            break;
        case 9:
            list_open_files();
            break;
        case 10:
            cout<<"Enter Disk Name:";
            cin>>disk_name;
            unmount_disk(disk_name);
            return 0;
        default:
            cout<<"Enter Vaild Input!"<<endl;
            break;
        }
    }

}




int main(){
    int option;
    string disk_name;
    while(1){
        cout<<"1:create disk"<<endl;
        cout<<"2:mount disk"<<endl;
        cout<<"3:exit"<<endl;
        cin>>option;
        if(option == 3){
            cout<<"-------------------Thank you!---------------------------------\n";
            break;
        }
        switch (option)
        {
        case 1:
            cout<<"Enter Disk Name:";
            cin>>disk_name;
            create_filesystem(disk_name);
            break;
        case 2:
            cout<<"Enter Disk Name:";
            cin>>disk_name;
            mount_disk(disk_name);
            file_utilites();
            break;
        default:
            cout<<"Enter Vaild Input!"<<endl;
            break;
        }
    }
    return 0;
}

