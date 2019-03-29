/*!
 * @file view.c
 *
 * @version 4
 * @author Saint-Amand Matthieu, Castelain Julien, Hachoud Rassem
 * @brief The purpose of this file is to set up a shell-like management interface in which the memory, defined 
 *  at program launch, limits the possibilities of manipulation within the directory.
 *  The user will therefore be able to perform operations such as: 
 *	<li>viewing file data </li>
 *	<li>copy a file and paste it in another position</li>
 *	<li>the creation of a file or folder inside the management directory </li>
 *	<li>delete some data</li>
 *
 * @file =main.c
 *
 * @see partition.h
 * @see utils.h
 * @see command.h
 */
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "partition.h"
#include "utils.h"
#include "commands.h"

#define DIM 300
#define CYAN "\033[1;36m" // Bold cyan
#define RED "\033[0;31m" // simple red
#define RED_BOLD "\033[1;31m"//bold red
#define RESET_COLOR "\033[0m"
/*!
 * @brief Here, the program starts an infinite loop that simulates the control interface and waits 
 * for the user to enter his order. This is then separated into arg1 arg2 command to easily 
 * launch the associated function.
 */
int file_exist(char* nom_fichier)
{
  struct stat fstat;                 
  return lstat(nom_fichier, &fstat);
}
int main(int argc, char ** argv) 
{
  if (argc != 3) { exitError(RED_BOLD"Utilisation : ./creat [-c|-v] <Nom Partition>)"RESET_COLOR); }
  int partition;
  if(!strcmp(argv[1],"-c")){
	if (!file_exist(argv[2])){
    	printf(RED_BOLD"Fichier déjà existant.\n"RESET_COLOR);
   	 	return 0;
  	}
	else{
		creatPartition(argv[2]);
		partition = open(argv[2], O_RDWR);
	}
  }else if(!strcmp(argv[1],"-v")){
	partition = open(argv[2], O_RDWR);
  }

  int b=1;
  int currDir =0;
  int Copie=-1;

	/* for each command and each arguments associated */
	char line[1024];
	char cmd[1024];
	char arg1[1024];
	char arg2[1024];
	int cursor, result, args;
	
	printf(CYAN "\n========== SIMPLE FILE MANAGER ==========\n" RESET_COLOR);

	while(b>0){
		pwd(partition,currDir);			   
		fflush(stdout);
		if(!fgets(line,sizeof(line),stdin)) break;

		if(line[0]=='\n') continue;
		line[strlen(line)-1] = 0;
		/*
		* In this case, we separate the command with 3 strings of character to use it easily		
		*/
		args = sscanf(line,"%s %s %s",cmd,arg1,arg2);
		if(args==0) continue;

		if(!strcmp(cmd,"mkdir")) {
			if(args==2){
				mmkdir(partition, currDir, arg1);
			}else{
				printf(RED_BOLD "Commande : mkdir <directory name>" RESET_COLOR);
			}
		}else if(!strcmp(cmd,"rm")) {
			if(args==2){
				rm(partition, currDir,arg1);
			}else{
				printf(RED_BOLD "Commande : rm <file name>" RESET_COLOR);
			}
		}else if(!strcmp(cmd,"cd")) {
			if(args==2){
				currDir = cd(partition,currDir,arg1);
			}else{
				printf(RED_BOLD "Commande : cd <path>" RESET_COLOR);
			}
		}else if(!strcmp(cmd,"cp")) {
     		    Copie = cp(partition, currDir);
		}else if(!strcmp(cmd,"past")) {
     		    past(partition, currDir, Copie);
		}else if(!strcmp(cmd,"ls")) {
     		    ls(partition, currDir);
		}else if(!strcmp(cmd,"touch")) {
				touch(partition,currDir,arg1,"");
		}else if(!strcmp(cmd,"man")) {
				man(args,arg1);
		} else if(!strcmp(cmd,"quit") || !strcmp(cmd,"exit")) {
			b=0;
		}else {
			printf(RED"Unknown command: %s\n"RESET_COLOR,cmd);
			printf(RED_BOLD"Type 'help' for a list of commands.\n"RESET_COLOR);
		}
	}

	printf(CYAN "\tDisconnected\n" RESET_COLOR);
    close(partition);
    return 0;
}
