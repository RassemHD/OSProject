/*!
 * @file main.c
 *
 * @see partition.h
 * @see command.h
 */
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "partition.h"
#include "utils.h"

int initLinkTab[15] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }; // to init the links table in the structure of the bloc

/*!
 * @brief add an id to the link table of a parent directory 
 * @param partition : descriptor of the partition
 * @param dirP : id of the parent directory in the file system
 * @param lien : the new link to add
 */
int addLink(int partition, int dirP, int lien) 
{
	int i=0;
	Bloc parent = READ(partition,dirP);
    while(i<14 && parent.LINK_TO_BLOC[i] != -1)
	  { i++; }
	
 	if(i<=14) { 
		parent.LINK_TO_BLOC[i]=lien; 
		WRITE(partition,dirP, parent); 
		return 1; 
	}
	else 	
		exitError("Full memory\n"); 
}

/*!
 * @brief remove an id from the link table of a parent directory 
 * @param partition : descriptor of the partition
 * @param dirP : id of the parent directory in the file system
 * @param lien : the id to remove
 */
int rmvLink(int partition, int dirP, int lien) {
	int i=0;
    Bloc parent = READ(partition,dirP);
	while(i<14 && parent.LINK_TO_BLOC[i] != lien)
	{ i++; }
	
 	if(i<=14)
	{parent.LINK_TO_BLOC[i]=-1; WRITE(partition,dirP, parent); return 1;}
}

/*!
 * @brief get the boc name by his id
 * @param partition : descriptor of the partition
 * @param nB : id of the bloc
 * @param blocName : a string containing the name fo th bloc 
 */
void blocName(int partition, int nB, char blocName[DIM_NAME_FILE]){
	Bloc bloc = READ (partition,nB);

	strcpy( blocName, bloc.BNAME);

}

/*!
 * @brief remove a bloc from the partition
 * @param partition : descriptor of the partition
 * @param dirP : parent directory
 * @param nB : id of the bloc
 */
void rmvFile(int partition,  int dirP, int nB)
{
	if (nB > NOMBREBLOCS) {
		exitError("Bloc out of the partition");
	}
	rmvLink(partition, dirP, nB);
    Bloc bloc=READ(partition, nB);
	int BLOCSUIVANT = bloc.NEXT;
	bloc.LIBRE = 1; 
	bloc.NEXT = -1;
	strcpy(bloc.BNAME, "");
	memcpy(bloc.LINK_TO_BLOC, initLinkTab, sizeof(initLinkTab));
	WRITE(partition, nB, bloc);
	
	if(BLOCSUIVANT!=-1){
		rmvFile(partition, dirP, BLOCSUIVANT);   
	}
}

/*!
 * @brief remove a directory from the partition
 * @param partition : descriptor of the partition
 * @param dirP : parent directory
 * @param nB : id of the bloc
 */
void rmvFolder(int partition,  int dirP, int nB)
{
	rmvLink(partition, dirP, nB);
	int i=0; 
	Bloc dossier=READ(partition,nB);
	Bloc bloc;

	while(i<=14){	
		if(dossier.LINK_TO_BLOC[i]!=-1){
			bloc=READ(partition,dossier.LINK_TO_BLOC[i]);
			if(bloc.F_D){
				rmvFile(partition, dossier.ID_BLOC, bloc.ID_BLOC);
			}
		 	else{
		 		rmvFolder(partition, dossier.ID_BLOC, bloc.ID_BLOC);
		 	}	
		}
	 	i++;
	}
 	rmvFile(partition,dirP,nB);
}

/*!
 * @brief get the id of the file having the specified name
 * @param partition : descriptor of the partition
 * @param nom : name of the file
 */
int idNameFile(int partition, int dirP, char nom[DIM_NAME_FILE])
{
    char blcName[DIM_NAME_FILE];
	Bloc parent=READ(partition, dirP); Bloc bloc; int i=0;

	while(i<=14){
	 if(parent.LINK_TO_BLOC[i]!=-1){ 	
		 blocName(partition, parent.LINK_TO_BLOC[i], blcName);
		 if(!strcmp(nom,blcName)){  
		 	return parent.LINK_TO_BLOC[i]; 
		 }
	 }
     i++;
	}
	printf("%s : element not found in this directory !\n", nom);
	return -1;
}

/*!
 * @brief to verify if a file/directory exist in the specified directory
 * @param partition : descriptor of the partition
 * @param dirP : parent directory
 * @param nom : name of the file
 */
int elementExistsInFolder(int partition, int dirP, char nom[DIM_NAME_FILE]){
    char blcName[DIM_NAME_FILE];
	Bloc parent=READ(partition, dirP); Bloc bloc; int i=0;

	while(i<=14)
	{
	 if(parent.LINK_TO_BLOC[i]!=-1) 
	 { 	
		 blocName(partition, parent.LINK_TO_BLOC[i], blcName);
		 if(!strcmp(nom,blcName)) {  return 1; }
	 }
     i++;
	}
 return -1;
}

/*!
 * @brief print recursively the path of the specified directory
 * @param partition : descriptor of the partition
 * @param dir: the directory
 */
void recNamePath(int partition, int dir)
{
 	Bloc bloc=READ(partition, dir);
	if(bloc.PERE !=-1){
		recNamePath(partition, bloc.PERE);
		printf("/%s",bloc.BNAME);
	}
}

/*!
 * @brief to past a directory in an other one
 * @param partition : descriptor of the partition
 * @param F_directory : id of the destination directory
 * @param original_ID : id of the original directory to past
 */
int pastFolder(int partition, int F_directory, int original_ID)
{
	int enfant;
	int i=0;
	int id =firstEmptyBloc(partition);
	Bloc original=READ(partition, original_ID);
	Bloc copie;
	addLink(partition, F_directory,  id);
	copie.F_D= original.F_D;
	copie.ID_BLOC=id;
	copie.LIBRE=0;
	copie.NEXT=-1;
	copie.PERE=F_directory;
	memcpy(copie.LINK_TO_BLOC, initLinkTab, sizeof(initLinkTab));
	strcpy(copie.BNAME,original.BNAME);
	WRITE(partition, id, copie);
	 while(i<=14){
	    if(original.LINK_TO_BLOC[i]!=-1){
			enfant = pastFolder(partition, id, original.LINK_TO_BLOC[i]);
			if(enfant != copie.ID_BLOC){
				copie.LINK_TO_BLOC[i]=enfant;
			}
		}
		i++;
	 }
	WRITE(partition, id, copie);
	return id;
}

/*! 
 * @brief allow to know if a file exists in the user's system
 * @param nom_fichier : the name of the file
 */
int file_exist(char* nom_fichier)
{
  struct stat fstat;                 
  return lstat(nom_fichier, &fstat);
}

/*! 
 * @brief print an error message then exit
 * @param massege : error message
 */
void exitError(char *message)
{
  int errnum;
  fprintf(stderr,"exit : %s\n",message);
  exit(1);
}


  


