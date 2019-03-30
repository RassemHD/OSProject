/*!
 * @file command.c
 * @version 4
 * @author Saint-Amand Matthieu, Castelain Julien, Hachoud Rassem
 * @brief This file contains all the file management functions. Each command is then called by view.c which breaks down the string into command + arguments for sending it here. 
 *<ul>
 *<li>void cat(int partition, int nBk); </li>
 *<li>void ls(int partition, int nBk)=> allow to list repertory and file on the current path.</li>
 *<li>void rm(int partition, int dirPar, char *name)=> allow to delete a bloc file/repertory with the name send in argument.</li>
 *<li>int mkdir(int score, int dirPar, char *name)=> allow to build a bloc file/repertory with a name send in parameter.</li>
 *<li>void pwd(int partition, int currDir);=> to know the current path where the user is.</li>
 *<li>int cp(int partition, int dirPar)=> to copy a file and send it in a variable to allow the command paste type by the user after this one.</li>
 *<li>int past(int partition, int RepertoirPere, int original_ID)=> retrieve the variable from the copy function</li>
 *<li>int pastFolder(int partition, int RepertoirPere, int original_ID);</li>
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
/*! @brief Displays the content of a directory */
void ls(int partition, int nB)
{
    Bloc bloc=READ(partition, nB);
	Bloc b;
	int i=0, vide=1;
	while(i < 15)
	{ 
		if(bloc.LIENS[i] != -1)
		{
		 b=READ(partition, bloc.LIENS[i]);
			if(b.FICHIER_DOSSIER)
			{ printf(GREEN "'%s'\t" RESET_COLOR,b.NOM_BLOC);} //Fichier
			else
			{ printf(CYAN"\"%s\":%d:\t"RESET_COLOR,b.NOM_BLOC,b.ID_BLOC);}	//Dossier			
		 vide=0;
		}
	i++;
	
	}
    if(vide) {printf("Le dossier '%s' est vide !\n",bloc.NOM_BLOC);}
	else {printf("\n");}
}

/*! @brief Delete a bloc element, file or repertory */
void rm(int partition, int dirPar, char *nom)
{
  Bloc bloc;
  int id_bloc= idNameFile(partition, dirPar, nom);
		
	if(id_bloc!=-1){
		bloc = READ(partition,id_bloc);
			
		if(bloc.FICHIER_DOSSIER){  	
					rmvFile(partition, dirPar, id_bloc); 	
					printf(GREEN"Fichier supprimé"RESET_COLOR);
		}else{ 
				rmvFolder(partition, dirPar, id_bloc);
				printf(GREEN"Répertoire supprimé"RESET_COLOR);
			}
		}
	}
/*! @brief Create a file bloc */
void touch(int partition, int dirPar, char nom[DIM_NAME_FILE], char *donnees)
{
  Bloc bloc;
  int id = firstEmptyBloc(partition);
  bloc=InitBloc(1,id,0,-1,nom,donnees, dirPar);  
  WRITE(partition, id, bloc);
  addLink(partition, dirPar, id); 
}
/*! @brief Display the path of the current repertory */
void pwd(int partition, int currDir)
{
	printf("\nuser:/Home");
	Bloc bloc=READ(partition, currDir);
	if(bloc.PERE !=-1)
	{   recNamePath(partition, bloc.PERE);
		printf("/%s",bloc.NOM_BLOC);
	}
	printf("$>");
}
/*! @brief create a repertory */
int mmkdir(int partition, int dirPar, char *nom)
{ 
  Bloc bloc;
	if(elementExistsInFolder(partition,dirPar, nom) == -1)
	{
		int id = firstEmptyBloc(partition);
		bloc=InitBloc(0,id,0,-1,nom,"",dirPar);       
		WRITE(partition, id, bloc);
		addLink(partition, dirPar, id); 
		printf(GREEN"Directory created"RESET_COLOR);
		return 0;
	}else{
		printf("An element with this name already exists\n"); return 0;
	}
}
/*! @brief changes the current path */
int cd(int partition, int currDir, char *nom)
{
   if (strcmp(nom, "..") == 0) 
   {
	  Bloc parent = READ(partition, currDir);
	  if(parent.PERE == -1)
	 		return currDir;
	  else
	  	return parent.PERE;
   }
   else
   {
	int id_bloc= idNameFile(partition, currDir, nom);
	   if(id_bloc!=-1)
	   {
        Bloc bloc = READ(partition, id_bloc);
	    	if(bloc.FICHIER_DOSSIER)
		  		return currDir;
		 		else
		 			return bloc.ID_BLOC;}
	   else
	    return currDir;
	}
  
  
}

/*! @brief Copy a file */
int cp(int partition, int dirPar)
{
  char nom[DIM_NAME_FILE];
  printf("Elément à copier :   ");
  fgets(nom,sizeof(nom),stdin);     
  strtok(nom, "\n");
  int id= idNameFile(partition, dirPar, nom);
  if(id==-1)
  {
	  return -1;
  }
  else
  { 
	  printf("Elément copié !\n");
	  return id;
  }
}

/*! @brief Past a file previously copied */
int past(int partition, int RepertoirPere, int original_ID)
{
	char nom[DIM_NAME_FILE];
	blocName(partition, original_ID, nom);
	int id;
	
  if(original_ID == -1)
	{
	  printf("None file waiting... \n"); 
	  return -1;
    }
  else
  {
	if(elementExistsInFolder(partition,RepertoirPere, nom) == -1)  
	{
       Bloc original= READ(partition, original_ID);
       printf("L'élément collé dans ce répertoir est le %s : %s\n",original.FICHIER_DOSSIER?"Fichier":"Dossier", original.NOM_BLOC);
        //addLink(partition, currDir,  IDCopie);
      if(original.FICHIER_DOSSIER)
      {
      }
      else
      {
	  pastFolder(partition, RepertoirPere, original_ID);
      }
	}
	else 
	{printf("An element in this directory already exists\n"); return -1;}
  return 1;
  }
}
/*! @brief Allow to display the help text for a specific command */
void man(int argc,char* argv){
  if(argc==2){
    if(strcmp(argv,"cd")==0){
      printf("Commande cd : permet de changer de répertoire courant -\n argument : le chemin du répertoire \n Exemple : cd chemin ou ./cd chemin \n");
    }else if(strcmp(argv,"ls")==0){
      printf("Commande ls : liste le contenu d'un répertoire\n");
    }else if(strcmp(argv,"cat")==0){
      printf("Commande cat : afficher le contenu du fichier passé en parametre\n Exemple : cat nom_fichier\n");
    }else if(strcmp(argv,"rm")==0){
      printf("Commande rm : supprime le fichier entré en paramètre \n Exemple : rm nom_fichier \n");
    }else if(strcmp(argv,"mv")==0){
      printf("Commande mv : déplace un fichier vers un répertoire\nExemple : mv nom_fichier nom_repertoir");
    }else if(strcmp(argv,"mkdir")==0){
      printf("Commande mkdir: crée un dossier\n Exemple : mkdir nom_dossier \n");
    }else if(strcmp(argv,"pwd")==0){
      printf("Commande pwd : affiche le répertoire courant \n aucun argument\n");
    }else if(strcmp(argv,"cp")==0){
      printf("Commande cp : copie un fichier dans un dossier  \n Arguments : paramètre 1 : fichier source, paramètre 2 : le chemin destination \n Exemple : cp test.c chemin \n ");
    }else{
      printf("Cette commande n'existe pas sur notre programme ! \n");
    }
  }else if (argc==1){
    	printf("Veuillez entrer une commande après le man, exemple: man cp\n");
  } else {
    printf("Veuillez entrer le bon nombre d'argument\n");
  }
}

