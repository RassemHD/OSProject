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
int main(int argc, char ** argv) 
{
  if (argc != 2) { exitError(RED_BOLD"Utilisation : ./creat <Nom Partition>)"RESET_COLOR); }
 
  int partition = open(argv[1], O_RDWR);
  int b=1;
  int RepertoirCourant =0;
  int Copie=-1;

	/* for each command and each arguments associated */
	char line[1024];
	char cmd[1024];
	char arg1[1024];
	char arg2[1024];
	int cursor, result, args;

  printf("Commandes :\n dossier => Création d'un dossier\n");
	printf(" delete=>Supprimme un élément du répertoire\n ls => Affiche contenu répertoire\n");

	while(b>0){
		pwd(partition,RepertoirCourant);			   
		fflush(stdout);
		if(!fgets(line,sizeof(line),stdin)) break;

		if(line[0]=='\n') continue;
		line[strlen(line)-1] = 0;
		/* 
		* In this case, we separate the command with 3 strings of character to use it easily		
		*/
		args = sscanf(line,"%s %s %s",cmd,arg1,arg2);
		if(args==0) continue;

		if(!strcmp(cmd,"dossier")) {
			if(args==2){
				mkdir(partition, RepertoirCourant, arg1);
			}else{
				printf(RED_BOLD "Commande : dossier <nomDossier>" RESET_COLOR);
			}
		}else if(!strcmp(cmd,"delete")) {
			if(args==2){
				rm(partition, RepertoirCourant,arg1);
			}else{
				printf(RED_BOLD "Commande : delete <nomDossier>" RESET_COLOR);
			}
		}else if(!strcmp(cmd,"cd")) {
  		    RepertoirCourant = cd(partition, RepertoirCourant);
		}else if(!strcmp(cmd,"copy")) {
     		    Copie = cp(partition, RepertoirCourant);
		}else if(!strcmp(cmd,"paste")) {
     		    past(partition, RepertoirCourant, Copie);
		}else if(!strcmp(cmd,"ls")) {
     		    ls(partition, RepertoirCourant);
		}else if(!strcmp(cmd,"delete")) {

		}else if(!strcmp(cmd,"help")) {
			printf("Commands are:\n");
			printf("    dossier <name_file>\n");
			printf("    copy : indicate the source file then indicate the target file\n");
			printf("    delete <name_file>\n");
			printf("    cd\n");
			printf("    paste\n");
			printf("    ls\n");
			printf("    help\n");
			printf("    quit\n");
			printf("    exit\n");
		} else if(!strcmp(cmd,"quit")) {
			b=0;
		} else if(!strcmp(cmd,"exit")) {
			b=0;
		} else {
			printf(RED"Unknown command: %s\n"RESET_COLOR,cmd);
			printf(RED_BOLD"Type 'help' for a list of commands.\n"RESET_COLOR);
		}
	}
	printf(CYAN "Disconnected\n" RESET_COLOR);
  close(partition);
  return 0;
}
