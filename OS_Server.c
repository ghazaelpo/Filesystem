#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "OS_Server.h"


/*Varibles de tipo estructura, publicas locales*/
inode_t inodeList[NO_INODES] = {{0}, {0}, {'d',1024,{8,0,0,0,0}, "rwx", 1}}; //16*6
directory_t rootDir[DIRS_PER_BLOCK] = {{2, "."}, {2, ".."}};
currentDirectory_t currentDirectory = {2, ".", 8};
int32_t freeInodeList[FIL_SIZE] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
int32_t freeBlockList[FBL_SIZE] = {9,10,11,12,13,14,15,16,17,18,18,20,21,22,23,24,25,26,27,28};
int8_t superBlock[SB_SIZE_BITS] = {'0'};


/**
* @fn void OS_vWrite2UserFifo(char * aFileName,char * aDataToWrite)
* @brief Provee al servidor una interfaz para poder escribir a la fifo del usuario 
* @param[in] char * aFifoName: Puntero a caracter que apunta a un string que contiene la fifo a la cual se quiere escribir
* @param[in] char * aDataToWrite: Puntero a caracter que apunta a un string que contiene la informacion que se quiere escribir
*/
void OS_vWrite2UserFifo(char * aFileName,char * aDataToWrite)
{
	int s32UserFD;
	
	s32UserFD = open(aFileName,O_WRONLY);
	if (s32UserFD==-1)
		printf("No se pudo abrir el archivo");
		printf("\n");
	write(s32UserFD, aDataToWrite, strlen(aDataToWrite));
	close(s32UserFD);
}

/**
* @fn void OS_vServerAlgo(void)
* @brief Algoritmo principal que engloba toda la funcionalidad del servidor
* @param[in, out] --
*/
void OS_vServerAlgo(void)
{
	int s32ServerFD;
	char * aFileName = NULL;
	char * aServiceIndex = NULL;
	
	char aBufferSender[MAX_FIFODATA] = "";
	char aBufferGuard[MAX_FIFODATA] = "";
	char aBufferReader[MAX_FIFODATA] = "";
	
	sleep(2);
	//system("clear");
	printf("************Bienvenido al sistema operativo************\nEsperando solicitudes de usuarios...\n");
	
	/*******************READ FIFO**************************/
	s32ServerFD = open("ServerFifo",O_RDONLY);
	if (s32ServerFD == -1)
	    printf("No se pudo abrir el archivo\n");
	read(s32ServerFD, aBufferReader, sizeof(aBufferReader));
	close(s32ServerFD);
	/******************************************************/
	
	strcpy(aBufferGuard, aBufferReader);
	aServiceIndex = strtok(aBufferGuard, "_");
	
	/*Se solicita crear un file system*/
	if(strcmp(aServiceIndex, "CreateFS") == ZERO_COMPARATOR)
	{
        printf("Se creo FS");
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		CreateFileSystem(apUserName);
	}
	/*Se solicita crear un directorio*/
	if(strcmp(aServiceIndex, "CreateDir") == ZERO_COMPARATOR)
	{
		char apDirectoryName[DIRNAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apDirectoryName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
        CreateDirectory(apUserName, apDirectoryName, currentDirectory.c_inode);
	}
	/*Se solicita eliminar un directorio*/
	if(strcmp(aServiceIndex, "DeleteDir") == ZERO_COMPARATOR)
	{
		char apDirectoryName[DIRNAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apDirectoryName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		DeleteDirectory(apUserName, apDirectoryName, currentDirectory.c_inode);
	}
	/*Se solicita cambiar de directorio*/
	if(strcmp(aServiceIndex, "ChangeDir") == ZERO_COMPARATOR)
	{
		char apDirectoryName[DIRNAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apDirectoryName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		int8_t exists = ChangeDirectory(apUserName, apDirectoryName, currentDirectory.c_inode);

		int32_t fd = open("ServerFifo", O_WRONLY);
		char resp[15] = "";
		if(strcmp(apDirectoryName, "..") == ZERO_COMPARATOR)
		{
			sprintf(resp, "DirAceptado");
			write(fd, resp, sizeof(resp));
		}
		else 
		{
			if(exists == 0)	
			{
				sprintf(resp, "DirAceptado");
				write(fd, resp, sizeof(resp));
			}
			else
			{
				sprintf(resp, "DirAceptado");
				write(fd, resp, sizeof(resp));
			}
		}
		close(fd);
	}
	/*Se solicita toda la informacion del directorio "ls -lia"*/
	if(strcmp(aServiceIndex, "InfoDir") == ZERO_COMPARATOR)
	{
		char apUserName[USRNAME_SIZE] = "";
		char apDirectoriesAvailable[FILEDATA_SIZE] = "";
		char * apDirectories = "documents_home_page";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		ListDirectories(apUserName, currentDirectory.c_block);
	}
	/*Se solicita crear un archivo*/
	if(strcmp(aServiceIndex, "CreateFile") == ZERO_COMPARATOR)
	{
		char apFileName[FILENAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apFileName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		CreateFile(apUserName, apFileName, currentDirectory.c_inode);
	}
	/*Se solicita eliminar un archivo*/
	if(strcmp(aServiceIndex, "DeleteFile") == ZERO_COMPARATOR)
	{
		char apFileName[FILENAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apFileName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		DeleteFile(apUserName, apFileName, currentDirectory.c_inode);
	}
	/*Se solicita editar un archivo*/
	if(strcmp(aServiceIndex, "EditFile") == ZERO_COMPARATOR)
	{
		char apFileName[FILENAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apFileName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
        EditFile(apUserName, apFileName, currentDirectory.c_inode);
	}
	/*Se solicita leer un archivo*/
	if(strcmp(aServiceIndex, "OpenFile") == ZERO_COMPARATOR)
	{
		char apFileData[FILEDATA_SIZE] = "";
		char apFileName[FILENAME_SIZE] = "";
		char apUserName[USRNAME_SIZE] = "";
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apFileName, aServiceIndex);
		
		aServiceIndex = strtok(NULL, "_");
		strcpy(apUserName, aServiceIndex);
		
		ReadFile(apUserName, apFileName, currentDirectory.c_inode);
	}
}

int main()
{
	system("mkfifo ServerFifo");
	//system("clear");
	printf("************Bienvenido al sistema operativo************\nEsperando solicitudes de usuarios...\n");
	
	while(1)
	{
		OS_vServerAlgo();
	}

    return 0;
}

/**
* @fn void CreateFileSystem(char apUserName[USRNAME_SIZE])
* @brief Provee una interfaz para crear el file system del nuevo usuario
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
*/
void CreateFileSystem(char apUserName[USRNAME_SIZE])
{
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_CREAT|O_WRONLY);
    perror("");
    if (fd == -1)
    {
        printf("Can not open FS on CreateFS() \n");
    }

    write(fd, superBlock, sizeof(superBlock));
    write(fd, inodeList, sizeof(inodeList));
    write(fd, rootDir, sizeof(rootDir));

    int32_t cont = 0;
    do
    {
        write(fd, superBlock, sizeof(superBlock));
        cont++;
    } while (cont < 1000);
    
    close(fd);
}

/**
* @fn int32_t GetFreeInode(void)
* @brief Provee una interfaz para obtener un inodo libre
* @param[out] int32_t newFreeInode: Entero que retorna l inodo libre
*/
int32_t GetFreeInode(void)
{
    int32_t newFreeInode = -1;
    for (size_t i = 0; i < FIL_SIZE; i++)
    {
        if (freeInodeList[i] != 0)
        {
            newFreeInode = freeInodeList[i];
            freeInodeList[i] *= 0;
            break;
        }
    }
    if (newFreeInode == -1)
    {
        printf("LIL está vacía\n");
        //TODO: Qué hacer cuando se vacíe
    }

    return newFreeInode;
}

/**
* @fn int32_t GetFreeBlock(void)
* @brief Provee una interfaz para obtener un bloque libre
* @param[out] int32_t newFreeBlock: Entero que retorna l bloque libre
*/
int32_t GetFreeBlock(void)
{
    int32_t newFreeBlock = -1;
    for (size_t i = 0; i < FBL_SIZE; i++)
    {
        if (freeBlockList[FBL_SIZE - i] != 0)
        {
            newFreeBlock = freeBlockList[FBL_SIZE - i];
            freeBlockList[FBL_SIZE - i] *= 0;
            break;
        }
    }
    if (newFreeBlock == -1)
    {
        printf("LBL está vacía\n");
        //TODO: Qué hacer cuando se vacíe
    }

    return newFreeBlock;
}

/**
* @fn void CreateDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para crear el directorio seleccionado
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apDirectoryName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del directorio que desea crear el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
*/
void CreateDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t freeInode = GetFreeInode();
    int32_t freeBlock = GetFreeBlock();
    inodeList[freeInode].i_type = 'd';
    inodeList[freeInode].i_size = 1024;
    inodeList[freeInode].i_contentTable[0] = freeBlock;
    inodeList[freeInode].i_owner = 1;

    directory_t newDirectory;
    newDirectory.d_inode = freeInode;
    strcpy(newDirectory.d_name, apDirectoryName);

    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDWR);

    if (fd == -1)
    {
        perror("");
        printf("Can not open FS on CreateDirectory()\n");
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        int32_t i_inode;
        int8_t d_name[12];
        read(fd, &i_inode, sizeof(i_inode));
        read(fd, d_name, sizeof(d_name));

        if (i_inode == 0)
        {
            lseek(fd, (((currentDirectoryBlock-1)*BLOCK_SIZE_BITS) + (i*DIR_SIZE_BYTES)), SEEK_SET);
            write(fd, &newDirectory, sizeof(newDirectory));
            break;
        } 
    }

    lseek(fd, ((freeBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    write(fd, &newDirectory.d_inode, sizeof(newDirectory.d_inode));
    int8_t namePoint[12] = ".";
    write(fd, namePoint, sizeof(namePoint));
    write(fd, &currentDirectoryInode, sizeof(currentDirectoryInode));
    int8_t namePointPoint[12] = "..";
    write(fd, namePointPoint, sizeof(namePointPoint));
   
    close(fd);
}

/**
* @fn void DeleteDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para eliminar el directorio seleccionado
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apDirectoryName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del directorio que desea eliminar el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
*/
void DeleteDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDONLY);

    if (fd == -1)
    {
        printf("Can not open FS on DeleteDirectory()");
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    int32_t directoryBlock = -1;
    int8_t d_name[12];
    int32_t d_inode;
    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        read(fd, &d_inode, sizeof(d_inode));
        read(fd, &d_name, sizeof(d_name));
        if (strcmp(d_name, apDirectoryName) == 0)
        {
            directoryBlock = inodeList[d_inode].i_contentTable[0];
            break;
        }  
    }  

    if (directoryBlock == -1)
    {
        printf("The file you want to delete does not exists\n");
    }    

    close(fd);

    inodeList[d_inode].i_type = '0';
}

/**
* @fn void CreateFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para crear el archivo seleccionado
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apFileName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del archivo que desea crear el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
*/
void CreateFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t freeInode = GetFreeInode();
    int32_t freeBlock = GetFreeBlock();
    inodeList[freeInode].i_type = 'f';
    inodeList[freeInode].i_size = 0;
    inodeList[freeInode].i_contentTable[0] = freeBlock;
    inodeList[freeInode].i_owner = 1;

    file_t newFile;
    newFile.f_inode = freeInode;
    strcpy(newFile.f_name, apFileName);

    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDWR);

    if (fd == -1)
    {
        printf("Can not open FS on CreateFile()");
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
		int32_t i_inode;
        int8_t d_name[12];
        read(fd, &i_inode, sizeof(i_inode));
        read(fd, d_name, sizeof(d_name));
        if (i_inode == 0)
        {
            lseek(fd, (((currentDirectoryBlock-1)*BLOCK_SIZE_BITS) + (i*DIR_SIZE_BYTES)), SEEK_SET);
            write(fd, &newFile, sizeof(newFile));
            break;
        }  
    }
   
    close(fd);
}

/**
* @fn void DeleteFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para eliminar el archivo seleccionado
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apFileName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del archivo que desea eliminar el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
*/
void DeleteFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDONLY);

    if (fd == -1)
    {
        printf("Can not open FS on CreateFile()");
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    int32_t fileBlock = -1;
    int8_t f_name[12];
    int32_t f_inode;
    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        read(fd, &f_inode, sizeof(int32_t));
        read(fd, &f_name, sizeof(f_name));
        if (strcmp(f_name, apFileName) == 0)
        {
            fileBlock = inodeList[f_inode].i_contentTable[0];
            break;
        }  
    }  

    if (fileBlock == -1)
    {
        printf("The file you want to delete does not exists\n");
    }    

    close(fd);

    inodeList[f_inode].i_type = '0';
}

/**
* @fn void ListDirectories(char apUserName[USRNAME_SIZE], int32_t currentBlockNumber)
* @brief Provee una interfaz para leer los directorios existentes en la ruta actual
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
*/
void ListDirectories(char apUserName[USRNAME_SIZE], int32_t currentBlockNumber)
{
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDONLY);
    int32_t fp = open("ServerFifo", O_WRONLY);

    if (fd == -1)
    {
        printf("Can not open FS on ListDirectories()");
    }

	char allInfo[1200] = "";
	char buff[100] = "";

    lseek(fd, ((currentBlockNumber-1)*BLOCK_SIZE_BITS), SEEK_SET);

    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        int32_t i_inode;
        int8_t i_name[12];
        read(fd, &i_inode, sizeof(int));
        if (i_inode == 0)
            break;   

        read(fd, i_name, sizeof(i_name));

		if (inodeList[i_inode].i_type == 'f')
		{
			sprintf(buff, "'f'   %d    %d    %d bytes    %s\n", i_inode, inodeList[i_inode].i_contentTable[0], inodeList[i_inode].i_size, i_name);
            strcat(allInfo, buff);
		}
        else
		{
            sprintf(buff, "'d'   %d    %d    %d bytes    %s\n", i_inode, inodeList[i_inode].i_contentTable[0], inodeList[i_inode].i_size, i_name);
			strcat(allInfo, buff);
		}
	}
	write(fp, allInfo, sizeof(allInfo));
    
    close(fd);
	close(fp);
}

/**
* @fn int8_t ChangeDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para permitir al usuario cambiarse de directorio
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apDirectoryName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del directorio al que desea moverse el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
* @param[out] int8_t --: Retorno que indica si existe o no el directorio al cual el usuario desea moverse
*/
int8_t ChangeDirectory(char apUserName[USRNAME_SIZE], char apDirectoryName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDONLY);

    if (fd == -1)
    {
        printf("Can not open FS on ChangeDirectory()");
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    int32_t directoryBlock = -1;
    int8_t d_name[12];
    int32_t d_inode;
    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        read(fd, &d_inode, sizeof(d_inode));
        read(fd, &d_name, sizeof(d_name));
        if (strcmp(d_name, apDirectoryName) == 0)
        {
            directoryBlock = inodeList[d_inode].i_contentTable[0];
            break;
        }  
    }  

	close(fd);

    currentDirectory.c_inode = d_inode;
    strcpy(currentDirectory.c_name, d_name);
    currentDirectory.c_block = directoryBlock;

    if (directoryBlock == -1)
    {
        return -1;
    }    
    else
    {
        return 0;
    }
}

/**
* @fn int8_t EditFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para editar un archivo que haya sido creado por el usuario
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apFileName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del archivo que desea leer el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
* @param[out] int8_t --: Retorno que indica si existe o no el archivo llamado char apFileName[DIRNAME_SIZE]
*/
int8_t EditFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDWR);

    if (fd == -1)
    {
        printf("Can not open FS on EditFile()");
        return -1;
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    int32_t fileBlock = -1;
    int8_t f_name[12];
    int32_t f_inode;
    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        read(fd, &f_inode, sizeof(int32_t));
        read(fd, &f_name, sizeof(f_name));
        if (strcmp(f_name, apFileName) == 0)
        {
            fileBlock = inodeList[f_inode].i_contentTable[0];
            break;
        }  
    }  
    close(fd);

    int32_t fp = open("ServerFifo", O_RDWR);
    if (fileBlock == -1)
    {
        char buff[10] = "NO";
        write(fp, buff, sizeof(buff));
        return 0;
    }   
    else
    {
        char buff[10] = "YES";
        write(fp, buff, sizeof(buff));
    }
    close(fp);
  
    fd = open(apUserName, O_WRONLY);
    lseek(fd, ((fileBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    fp = open("ServerFifo", O_RDONLY);

    char in;
    int32_t exit = 0;
    int32_t size = 0;
    while (in != EOF)
    {
        read(fp, &in, sizeof(in));

        if (in == ':')
            exit = 1;
        if ((exit == 1) && (in == 'q'))
        {
            size--;
            size--;
            break;
        }
        else
            exit == 0;
        
        write(fd, &in, sizeof(in));
        size++;
    }  
    close(fd);
    close(fp);

    inodeList[f_inode].i_size = size;

    return 0;
}

/**
* @fn int8_t ReadFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
* @brief Provee una interfaz para leer un archivo que haya sido creado por el usuario
* @param[in] char apUserName[USRNAME_SIZE]: Arreglo caracteres que almacena informacion que contiene el nombre del usuario
* @param[in] char apFileName[DIRNAME_SIZE]: Arreglo caracteres que almacena el nombre del archivo que desea leer el usuario
* @param[in] int32_t currentDirectoryInode: Entero que almacena el directorio actual del usuario
* @param[out] int8_t --: Retorno que indica si existe o no el archivo llamado char apFileName[DIRNAME_SIZE]
*/
int8_t ReadFile(char apUserName[USRNAME_SIZE], char apFileName[DIRNAME_SIZE], int32_t currentDirectoryInode)
{
    int32_t currentDirectoryBlock = inodeList[currentDirectoryInode].i_contentTable[0];
    
    strcat(apUserName, ".txt");
    int32_t fd = open(apUserName, O_RDONLY);

    if (fd == -1)
    {
        printf("Can not open FS on CreateFile()");
        return -1;
    }

    lseek(fd, ((currentDirectoryBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);

    int32_t fileBlock = -1;
    int8_t f_name[12];
    int32_t f_inode;
    for (size_t i = 0; i < DIRS_PER_BLOCK; i++)
    {
        read(fd, &f_inode, sizeof(int32_t));
        read(fd, &f_name, sizeof(f_name));
        if (strcmp(f_name, apFileName) == 0)
        {
            fileBlock = inodeList[f_inode].i_contentTable[0];
            break;
        }  
    }  

    close(fd);

    int32_t fp = open("ServerFifo", O_WRONLY);

    if (fileBlock == -1)
    {
        char buff[10] = "NO";
        write(fp, buff, sizeof(buff));
        close(fp);
    }    
    else
    {
        char send[5024];

        fd = open(apUserName, O_RDONLY);
        lseek(fd, ((fileBlock-1)*BLOCK_SIZE_BITS), SEEK_SET);
        fp = open("ServerFifo", O_WRONLY);

        char buffer;
        char buff2[2];
        for (size_t i = 0; i <= inodeList[f_inode].i_size; i++)
        {
            read(fd, &buffer, sizeof(buffer));
            buff2[0] = buffer;
            strcat(send, buff2);
        }
        write(fp, &send, sizeof(send));

        close(fd);
        close(fp);

    }

    return 0;
}
