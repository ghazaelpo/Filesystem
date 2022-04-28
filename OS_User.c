#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


/*Macros que se utilizaran para evitar numeros magicos*/

#define MAX_LINUXPATH		(250U)
#define MAX_USERNAME		(50U)
#define MAX_COMMAND			(50U)
#define ZERO_COMPARATOR		(0U)
#define UNDERSCORE			("_")


/*Enumeración para indicar las opciones del menu*/
enum OS_tenMenuSelection{
	OS_enDirCreate,
	OS_enDirDelete,
	OS_enDirChange,
	OS_enDirInfo,
	OS_enFileCreate,
	OS_enFileDelete,
	OS_enFileEdit,
	OS_enFileOpen
};

/*Prototipos de funciones utilizadas*/	
int OS_s32PrintedMenu(void);
void OS_vUserMenu(void);
void OS_vUserInit(void);
void OS_vFileEdit(void);
void OS_vFileOpen(void);
void OS_vFileCreate(void);
void OS_vFileDelete(void);
void OS_vDirectoryInfo(void);
void OS_vDirectoryCreate(void);
void OS_vDirectoryDelete(void);
void OS_vDirectoryChange(void);
void OS_vWriteToFifo(char * aFifoName, char * aDataToWrite);


/*Variables globales para el nombre de usuario y ruta de usuario*/
char aUserName[MAX_USERNAME] = "";
char aLinuxPath[MAX_LINUXPATH] = "";
int s32DirPaths = 0;


/**
* @fn void OS_vWriteToFifo(char * aFifoName, char * aDataToWrite)
* @brief Provee al usuario una interfaz para poder escribir a la fifo del servidor 
* @param[in] char * aFifoName: Puntero a caracter que apunta a un string que contiene la fifo a la cual se quiere escribir
* @param[in] char * aDataToWrite: Puntero a caracter que apunta a un string que contiene la informacion que se quiere escribir
*/
void OS_vWriteToFifo(char * aFifoName, char * aDataToWrite)
{
	int s32ServerFD;
	
	s32ServerFD = open(aFifoName,O_WRONLY);
	write(s32ServerFD, aDataToWrite, strlen(aDataToWrite));
	close(s32ServerFD);
}

/**
* @fn void OS_vDirectoryCreate(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca crear un directorio
* @param[in, out] --
*/
void OS_vDirectoryCreate(void)
{
	char aDataToWrite[MAX_COMMAND] = "";
	char * apCreateDirCommand = "CreateDir_";
	char aDirectoryName[MAX_COMMAND] = "";
	
	/*Solicitud del directorio a crear*/
	printf("New directory name: ");
	scanf("%s", &aDirectoryName);
	
	strcat(aDataToWrite, apCreateDirCommand);
	strcat(aDataToWrite, aDirectoryName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);

	OS_vWriteToFifo("ServerFifo", aDataToWrite);
}

/**
* @fn void OS_vDirectoryDelete(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca eliminar un directorio
* @param[in, out] --
*/
void OS_vDirectoryDelete(void)
{
	char aDataToWrite[MAX_COMMAND] = "";
	char * apDeleteDirCommand = "DeleteDir_";
	char aDirectoryName[MAX_COMMAND] = "";
	
	/*Solicitud del directorio a eliminar*/
	printf("Directory to delete: ");
	scanf("%s", &aDirectoryName);
	
	strcat(aDataToWrite, apDeleteDirCommand);
	strcat(aDataToWrite, aDirectoryName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);
}

/**
* @fn void OS_vDirectoryChange(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca cambiar de directorio 
* @param[in, out] --
*/
void OS_vDirectoryChange(void)
{
	int s32UserFD;
	char * aServiceIndex = NULL;
	char aDataToWrite[MAX_COMMAND] = "";
	char * apChangeDirCommand = "ChangeDir_";
	char aDirectoryName[MAX_COMMAND] = "";
	
	char aBufferGuard[MAX_LINUXPATH] = "";
	char aBufferReader[MAX_LINUXPATH] = "";
	
	/*Solicitud del directorio a cambiar*/
	printf("Jump to directory: ");
	scanf("%s", &aDirectoryName);
	
	strcat(aDataToWrite, apChangeDirCommand);
	strcat(aDataToWrite, aDirectoryName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);

    char resp[15] = "";
    int32_t s32ServerFD = open("ServerFifo",O_RDONLY);
	if (s32ServerFD == -1)
	    printf("No se pudo abrir el archivo\n");
	read(s32ServerFD, resp, sizeof(resp));
    close(s32ServerFD);
	
	/*Opcion para devolverse a una ruta anterior*/
	if(strcmp(aDirectoryName, "..") == ZERO_COMPARATOR)
	{
		int i;
		char * aPathIndex = NULL;
		char aPathCopy[MAX_LINUXPATH] = "";
		char aPathGuard[MAX_LINUXPATH] = "";
		
		strcpy(aPathGuard, aLinuxPath);
		aPathIndex = strtok(aPathGuard, "/");
		strcat(aPathCopy, aPathIndex);
		
		if(strcmp(resp, "DirAceptado") == ZERO_COMPARATOR)
		{
			for(i=0;i<s32DirPaths-1;i++)
			{
				aPathIndex = strtok(NULL, "/");
				strcat(aPathCopy, "/");
				strcat(aPathCopy, aPathIndex);
			}
			
			strcpy(aLinuxPath, aPathCopy);
			s32DirPaths--;
		}
		else
		{
			/*Se queda en el mismo directorio*/
		}
	}
	/*Opcion para entrar a otro directorio dentro de la ruta*/
	else
	{
		if(strcmp(resp, "DirAceptado") == ZERO_COMPARATOR)
		{
			strcat(aLinuxPath, "/");
			strcat(aLinuxPath, aDirectoryName);
			s32DirPaths++;
		}
		else
		{
			printf("Invalid directory: %s\n",aDirectoryName);
		}
	}
}

/**
* @fn void OS_vDirectoryInfo(void)
* @brief Provee una interfaz para obtener del servidor la informacion general del directorio. Simulando el comando "ls -lia" en Linux
* @param[in, out] --
*/
void OS_vDirectoryInfo(void)
{
	int s32UserFD;
	char * aDirInfo = NULL;
	char aDataToWrite[MAX_COMMAND] = "";
	char * apInfoDirCommand = "InfoDir_";
	
	char aBufferReader[MAX_LINUXPATH] = "";
	char aBufferGuard[MAX_LINUXPATH] = "";
	
	strcat(aDataToWrite, apInfoDirCommand);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);
	
	strcpy(aBufferGuard, aBufferReader);
	aDirInfo = strtok(aBufferGuard, "_");
	
	/*Obtencion de la informacion del servidor*/
    char allInfo[1200] = "";
    int32_t s32ServerFD = open("ServerFifo",O_RDONLY);
	if (s32ServerFD == -1)
	    printf("No se pudo abrir el archivo\n");
	read(s32ServerFD, allInfo, sizeof(allInfo));
    printf(allInfo);
    close(s32ServerFD);
}

/**
* @fn void OS_vFileCreate(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca crear un archivo
* @param[in, out] --
*/
void OS_vFileCreate(void)
{
	char aDataToWrite[MAX_COMMAND] = "";
	char * apCreateFileCommand = "CreateFile_";
	char aFileName[MAX_COMMAND] = "";
	
	/*Solicitud del archivo a crear*/
	printf("New file name: ");
	scanf("%s", &aFileName);
	
	strcat(aDataToWrite, apCreateFileCommand);
	strcat(aDataToWrite, aFileName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);

	OS_vWriteToFifo("ServerFifo", aDataToWrite);
}

/**
* @fn void OS_vFileDelete(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca eliminar un archivo
* @param[in, out] --
*/
void OS_vFileDelete(void)
{
	char aDataToWrite[MAX_COMMAND] = "";
	char * apDeleteFileCommand = "DeleteFile_";
	char aFileName[MAX_COMMAND] = "";
	
	/*Solicitud del archivo a eliminar*/
	printf("File to delete: ");
	scanf("%s", &aFileName);
	
	strcat(aDataToWrite, apDeleteFileCommand);
	strcat(aDataToWrite, aFileName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);
}

/**
* @fn void OS_vFileEdit(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca editar un archivo
* @param[in, out] --
*/
void OS_vFileEdit(void)
{
	size_t stLenght = 0;
	int s32UserFileDescriptor;
	char * apDataInput = NULL;
	char aDataToWrite[MAX_COMMAND] = "";
	char * apEditFileCommand = "EditFile_";
	char aFileName[MAX_COMMAND] = "";
	
	/*Solicitud del archivo a editar*/
	printf("File to edit: ");
	scanf("%s", &aFileName);
	
	strcat(aDataToWrite, apEditFileCommand);
	strcat(aDataToWrite, aFileName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);

	char response[25] = "";
    int32_t s32ServerFD = open("ServerFifo",O_RDONLY);
	if (s32ServerFD == -1)
	    printf("No se pudo abrir el archivo\n");
	read(s32ServerFD, response, sizeof(response));
    close(s32ServerFD);

	/*Operaciones para editar el archivo*/
	if (strcmp(response, "YES") == 0)
	{
		printf("You are editing file %s\n", aFileName);
		printf("To save file enter ':q'\n\n\n");

    	s32ServerFD = open("ServerFifo",O_WRONLY);
		if (s32ServerFD == -1)
	    	printf("No se pudo abrir el archivo\n");

		char in;
		int32_t exit = 0;
		while ((in = getchar()) != EOF)
		{
			if (exit == 2)
				break;
			if (in == ':')
				exit = 1;
			if ((exit == 1) && (in == 'q'))
			{
				exit = 2;
			}
			else
				exit == 0;
			
			write(s32ServerFD, &in, sizeof(in));
		}  
		close(s32ServerFD);
	}
	else
	{
		printf("The file does not exist!\n");
	}
}

/**
* @fn void OS_vFileOpen(void)
* @brief Provee una interfaz para comunicarle al servidor que se busca leer un archivo
* @param[in, out] --
*/
void OS_vFileOpen(void)
{
	size_t stLenght = 0;
	int s32UserFileDescriptor;
	char apDataInput[100];
	char aDataToWrite[MAX_COMMAND] = "";
	char * apOpenFileCommand = "OpenFile_";
	char aFileName[MAX_COMMAND] = "";
	
	/*Solicitud del archivo a leer*/
	printf("File to open: ");
	scanf("%s", &aFileName);
	
	strcat(aDataToWrite, apOpenFileCommand);
	strcat(aDataToWrite, aFileName);
	strcat(aDataToWrite, UNDERSCORE);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);
	
	char response[5024] = "";
    int32_t s32ServerFD = open("ServerFifo",O_RDONLY);
	if (s32ServerFD == -1)
	    printf("No se pudo abrir el archivo\n");
	read(s32ServerFD, response, sizeof(response));
    close(s32ServerFD);

	if (strcmp(response, "NO") != 0)
	{
		printf("The file content is: \n");

    	printf("%s\n\n", response);
	}
	else
	{
		printf("The file does not exist!\n");
	}
}

/**
* @fn int OS_s32PrintedMenu(void)
* @brief Provee el menu de usuario con las opciones para el sistema operativo y retorna la opcion seleccionada
* @param[out] s32MenuSelection: Variable que contiene la opción seleccionada por el usuario
* - Rango = {-2^31 a 2^31-1}
*/
int OS_s32PrintedMenu(void)
{
	int s32MenuSelection;
	
	printf("\n%s\n\n",aLinuxPath);
	
	printf("-----------------------------------OPTION MENU-----------------------------------\n");
	printf("|										|\n");
	printf("|Directory		[0]Create	[1]Delete	[2]Change	[3]Info	|\n");
	printf("|Files			[4]Create	[5]Delete	[6]Edit		[7]Open	|\n");
	printf("---------------------------------------------------------------------------------\n\n");
	
	printf("Choose an option listed above: ");
	scanf("%d", &s32MenuSelection);
	
	return s32MenuSelection;
}

/**
* @fn void OS_vUserMenu(void)
* @brief Provee una interfaz que selecciona las funcionalidades requeridas en base a lo que devuelve OS_s32PrintedMenu()
* @param[in, out] --
*/
void OS_vUserMenu(void)
{
	uint8_t u8MenuSelection;
	char * pLinuxExtension = " ~";
	
	strcat(aLinuxPath, aUserName);
	strcat(aLinuxPath, pLinuxExtension);
	
	while(1)
	{		
		u8MenuSelection = (uint8_t)OS_s32PrintedMenu();
		switch(u8MenuSelection)
		{
		case OS_enDirCreate:
			OS_vDirectoryCreate();
			break;
		case OS_enDirDelete:
			OS_vDirectoryDelete();
			break;
		case OS_enDirChange:
			OS_vDirectoryChange();	
			break;
		case OS_enDirInfo:
			OS_vDirectoryInfo();
			break;
		case OS_enFileCreate:
			OS_vFileCreate();
			break;
		case OS_enFileDelete:
			OS_vFileDelete();
			break;
		case OS_enFileEdit:
			OS_vFileEdit();	
			break;
		case OS_enFileOpen:
			OS_vFileOpen();
			break;
		default:
			printf("\nChoose a valid option\n");
		}
	}
}

/**
* @fn void OS_vUserInit(void)
* @brief Provee la inicialización del sistema solicitando el nombre de usuario y enviandole al servidor su informacion
	para obtener su file system
* @param[in, out] --
*/
void OS_vUserInit(void)
{
	int s32UserFileDescriptor;
	
	char aDataToWrite[MAX_COMMAND] = "";
	char * apFileSystemCommand = "CreateFS_";
	char * apFifoCreateCommand = "mkfifo ";
	char aFifoCompleteCommand[MAX_COMMAND] = "";
	
	printf("Enter Username: ");
	scanf("%s", &aUserName);
	
	strcat(aFifoCompleteCommand, apFifoCreateCommand);
	strcat(aFifoCompleteCommand, aUserName);
	system(aFifoCompleteCommand);
	
	strcat(aDataToWrite, apFileSystemCommand);
	strcat(aDataToWrite, aUserName);
	
	OS_vWriteToFifo("ServerFifo", aDataToWrite);
	system("clear");
}

int main()
{
	OS_vUserInit();	
	OS_vUserMenu();	
	
	return 0;
}


