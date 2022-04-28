#include <stdlib.h>


/*Definiciones utilizadas por el servidor para evitar numeros magicos*/

#define     KB_BITS                 1024
#define     BYTE_BITS               8

#define     BLOCK_SIZE_BITS         KB_BITS
#define     INODE_SIZE_BYTES        64
#define     DIR_SIZE_BYTES          16
#define     DIR_SIZE_BITS           DIR_SIZE_BYTES*BYTE_BITS
#define     SB_SIZE_BITS            NO_SUPER_BLOCKS*BLOCK_SIZE_BITS
#define     FBL_SIZE                20
#define     FIL_SIZE                20

#define     INODES_PER_BLOCK        BLOCK_SIZE_BITS/INODE_SIZE_BYTES
#define     DIRS_PER_BLOCK          BLOCK_SIZE_BITS/DIR_SIZE_BYTES

#define     NO_INODE_BLOCKS         6
#define     NO_INODES               NO_INODE_BLOCKS*INODES_PER_BLOCK
#define     NO_SUPER_BLOCKS         1

#define     ANSI_COLOR_RED          "\x1B[31m"
#define     ANSI_COLOR_GREEN        "\x1B[32m"
#define     ANSI_COLOR_RESET        "\x1B[0m"

#define     MAX_FIFODATA		(50U)

#define     USRNAME_SIZE		(20U)
#define     DIRNAME_SIZE		(20U)
#define     FILENAME_SIZE		(20U)
#define     FILEDATA_SIZE		(200U)

#define     ZERO_COMPARATOR		(0U)


/*Estructuras utilizadas por el servidor*/
typedef struct owner_t
{
    int8_t o_name[15];
    int8_t o_owner;
}owner_t;

typedef struct directory_t
{
    int32_t d_inode;      //4
    int8_t d_name[12];  //12
}directory_t;

typedef struct currentDirectory_t
{
    int32_t c_inode;      //4
    int8_t c_name[12];  //12
    int32_t c_block;
}currentDirectory_t;

typedef struct file_t
{
    int32_t f_inode;      //4
    int8_t f_name[12];  //12
}file_t;

typedef struct inode_t
{
    int8_t i_type; //'0' libre, 'd' directorio, 'f' archivo
    int32_t i_size;
    int32_t i_contentTable[5]; //4 directos y 1 indirecto
    int8_t i_permission[4];
    int8_t i_owner;
    int8_t i_empty[31];
}inode_t;


/*Prototipos de funciones publicas globales utilizadas por el servidor*/
void CreateFileSystem(char apUserName[USRNAME_SIZE]);
int32_t GetFreeInode(void);
int32_t GetFreeBlock(void);
void CreateDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode);
void DeleteDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode);
void CreateFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode);
void DeleteFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode);
void ListDirectories(char apUserName[USRNAME_SIZE], int32_t currentBlockNumber);
int8_t ChangeDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode);
int8_t EditFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode);
int8_t ReadFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode);
