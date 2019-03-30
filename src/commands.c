/*!
 * @file command.c
 * @version 4
 * @author Saint-Amand Matthieu, Castelain Julien, Hachoud Rassem
 * @brief This file contains all the file management functions. Each command is then called by view.c which breaks down the string into command + arguments for sending it here. 
 *<ul>
 *<li>void ls(int partition, int nBk)=> allow to list repertory and file on the current path.</li>
 *<li>void rm(int partition, int dirPar, char *name)=> allow to delete a bloc file/repertory with the name send in argument.</li>
 *<li>int mkdir(int score, int dirPar, char *name)=> allow to build a bloc file/repertory with a name send in parameter.</li>
 *<li>void pwd(int partition, int currDir);=> to know the current path where the user is.</li>
 *<li>int cp(int partition, int dirPar)=> to copy a file and send it in a variable to allow the command paste type by the user after this one.</li>
 *<li>int past(int partition, int F_directory, int original_ID)=> retrieve the variable from the copy function</li>
 *<li>int pastFolder(int partition, int F_directory, int original_ID);</li>
 *<li>int cd(int partition, int currDir);</li>
 *</ul>
 *
 * @see partition.h
 * @see partition.h
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

#define CYAN "\033[1;36m" // Bold cyan
#define GREEN "\033[1;32m" // Bold green == ANSI color
#define BLUE "\x1b[34m"
#define WHITE "\x1b[37m"
#define RED "\033[0;31m"
#define RESET_COLOR "\033[0m"


/*!
 * @brief Displays the content of a directory 
 * @param partition : descriptor of the partition
 * @param nB : number of the block
 */
void ls(int partition, int nB)
{
  	Bloc bloc=READ(partition, nB);
	Bloc b;
	int i=0, vide=1;
	while(i < 15){ 
		if(bloc.LINK_TO_BLOC[i] != -1){
		 	b=READ(partition, bloc.LINK_TO_BLOC[i]);
			if(b.F_D){
				printf(GREEN "'%s'\t" RESET_COLOR,b.BNAME);
			}
			else{
				printf(CYAN"\"%s\":%d:\t"RESET_COLOR,b.BNAME,b.ID_BLOC);
			}				
		 	vide=0;
		}
		i++;
	}
    if(vide) {
    	printf("Empty folder: '%s'!\n",bloc.BNAME);
    }
	else {
		printf("\n");
	}
}

/*!
 * @brief Delete a bloc element, file or repertory
 * @param partition : descriptor of the partition
 * @param dirPar : id of the parent directory
 * @param nom : name of the file to remove
 */
void rm(int partition, int dirPar, char *nom)
{
  Bloc bloc;
  int id_bloc= idNameFile(partition, dirPar, nom);
		
	if(id_bloc!=-1){
		bloc = READ(partition,id_bloc);
			
		if(bloc.F_D){  	
					rmvFile(partition, dirPar, id_bloc); 	
					printf(GREEN"File deleted!"RESET_COLOR);
		}else{ 
				rmvFolder(partition, dirPar, id_bloc);
				printf(GREEN"Directory deleted!"RESET_COLOR);
			}
		}
	}

/*!
 * @brief Create a new file bloc
 * @param partition : descriptor of the partition
 * @param dirPar : id of the parent directory
 * @param nom : name of the file to remove
 * @param donnees : intitial data of the created file 
 */
void touch(int partition, int dirPar, char nom[DIM_NAME_FILE], char *donnees)
{
 	Bloc bloc;
 	int id = firstEmptyBloc(partition);
 	bloc=InitBloc(1,id,0,-1,nom,donnees, dirPar);  
 	WRITE(partition, id, bloc);
 	addLink(partition, dirPar, id); 
	printf(GREEN"File created."RESET_COLOR);
}

/*!
 * @brief Display the path of the current diractory
 * @param partition : descriptor of the partition
 * @param currDirr : id of the current directory
 */
void pwd(int partition, int currDir)
{
	printf("\n@user/Home");
	Bloc bloc=READ(partition, currDir);
	if(bloc.PERE !=-1){
		recNamePath(partition, bloc.PERE);
		printf("/%s",bloc.BNAME);
	}
	printf("$>");
}

/*!
 * @brief Create a new diirectory
 * @param partition : descriptor of the partition
 * @param dirPar : id of the parent directory
 * @param nom : name of the new directory
 */
int mmkdir(int partition, int dirPar, char *nom)
{ 
  Bloc bloc;
	if(elementExistsInFolder(partition,dirPar, nom) == -1){
		int id = firstEmptyBloc(partition);
		bloc=InitBloc(0,id,0,-1,nom,"",dirPar);       
		WRITE(partition, id, bloc);
		addLink(partition, dirPar, id); 
		printf(GREEN"Directory created"RESET_COLOR);
		return 0;
	}else{
		printf("An element with this name already exists\n");
		return 0;
	}
}

/*!
 * @brief Change the current working directory
 * @param partition : descriptor of the partition
 * @param dirPar : id of the parent directory
 * @param nom : name of the new working directory
 */
int cd(int partition, int currDir, char *nom)
{
   if (strcmp(nom, "..") == 0){
	  Bloc parent = READ(partition, currDir);
	  if(parent.PERE == -1)
	 		return currDir;
	  else
	  	return parent.PERE;
   }
   else{
		int id_bloc= idNameFile(partition, currDir, nom);
	    if(id_bloc!=-1){
        	Bloc bloc = READ(partition, id_bloc);
	    	if(bloc.F_D)
		  		return currDir;
		 		else
		 			return bloc.ID_BLOC;}
	   	else
	    return currDir;
	} 
}

/*!
 * @brief Copy a file 
 * @param partition : descriptor of the partition
 * @param dirPar : id of the parent directory
 * @param nom : name of the file to copy
 */
int cp(int partition, int dirPar, char *nom)
{
  int id= idNameFile(partition, dirPar, nom);
  if(id==-1){
	  return -1;
  }
  else{ 
	  printf(GREEN"Copy done !\n"RESET_COLOR);
	  return id;
  }
}

/*!
 * @brief Past a file previously copied
 * @param partition : descriptor of the partition
 * @param F_directory : id of the destination directory
 * @param original_ID : id of the file to past
 */
int past(int partition, int F_directory, int original_ID)
{
	char nom[DIM_NAME_FILE];
	blocName(partition, original_ID, nom);
	int id;
	
  if(original_ID == -1){
	  printf("None file waiting... \n"); 
	  return -1;
  }else{
	if(elementExistsInFolder(partition,F_directory, nom) == -1){
       Bloc original= READ(partition, original_ID);
      if(original.F_D){

      }
      else{
	  	pastFolder(partition, F_directory, original_ID);
      }
	}
	else {
		printf("An element in this directory already exists\n"); 
		return -1;
	}
  	return 1;
  }
}

/*!
 * @brief Allow to display the help text for a specific command
 * @param argc : nember of the parameters pased to the man command
 * @param argv : name of the command
 */
void man(int argc,char* argv){
  if(argc==2){
    if(strcmp(argv,"cd")==0){
      printf("Command cd : Allow to change current directory -\n argument : the path \n Example : cd path or ./cd path \n");
    }else if(strcmp(argv,"ls")==0){
      printf("Command ls : List the content of a repertory\n");
    }else if(strcmp(argv,"cat")==0){
      printf("Command cat : Display the content of a file passed in argument\n Example : cat name_file\n");
    }else if(strcmp(argv,"rm")==0){
      printf("Command rm : Delete a file retrieved in argument \n Example : rm name_file \n");
    }else if(strcmp(argv,"mv")==0){
      printf("Command mv : move a file into a directory\nExample : mv name_file name_directory");
    }else if(strcmp(argv,"mkdir")==0){
      printf("Command mkdir: create a directory\n Example : mkdir name_directory \n");
    }else if(strcmp(argv,"pwd")==0){
      printf("Command pwd :display the current path \n aucun argument\n");
    }else if(strcmp(argv,"cp")==0){
      printf("Command cp : copy a file into a directory \n Arguments : paramètre 1 : fichier source, paramètre 2 : le chemin destination \n Example : cp test.c chemin \n ");
    }else{
      printf("This command doesn't exists \n");
    }
  }else if (argc==1){
    	printf("Please, enter a command after man : man <command> !\n");
  } else {
    printf("Please, enter argument\n");
  }
}

