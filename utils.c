/*!
 * @file main.c
 *
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

int tab_init[15] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
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
int rmvLink(int partition, int dirP, int lien) {
	int i=0;
    Bloc parent = READ(partition,dirP);
	while(i<14 && parent.LINK_TO_BLOC[i] != lien)
	{ i++; }
	
 	if(i<=14)
	{parent.LINK_TO_BLOC[i]=-1; WRITE(partition,dirP, parent); return 1;}
}
void blocName(int partition, int nB, char blocName[DIM_NAME_FILE]){
	Bloc bloc = READ (partition,nB);

	strcpy( blocName, bloc.BNAME);

}
void rmvFile(int partition,  int dirP, int nB){
	if (nB > NOMBREBLOCS) {exitError("Bloc hors de la partition"); }
	
	rmvLink(partition, dirP, nB);
    Bloc bloc=READ(partition, nB);
	int BLOCSUIVANT = bloc.NEXT;

	bloc.LIBRE = 1; 
	bloc.NEXT = -1;
	strcpy(bloc.BNAME, "");
	memcpy(bloc.LINK_TO_BLOC, tab_init, sizeof(tab_init));
	WRITE(partition, nB, bloc);
	
	if(BLOCSUIVANT!=-1)
	   { rmvFile(partition, dirP, BLOCSUIVANT);   }
}

void rmvFolder(int partition,  int dirP, int nB)
{
	rmvLink(partition, dirP, nB);
	int i=0; 
	Bloc dossier=READ(partition,nB);
	Bloc bloc;

	while(i<=14)
	{	
		if(dossier.LINK_TO_BLOC[i]!=-1)
		 {
			bloc=READ(partition,dossier.LINK_TO_BLOC[i]);
			if(bloc.F_D)
			 { rmvFile(partition, dossier.ID_BLOC, bloc.ID_BLOC); }
		 	else
		 	 {rmvFolder(partition, dossier.ID_BLOC, bloc.ID_BLOC); }	
		 }
	 i++;
	}

 rmvFile(partition,dirP,nB);
}
void CreationFichier(int partition, int dirP, char nom[DIM_NAME_FILE], char *donnees)
{
  Bloc bloc;
  int id = firstEmptyBloc(partition);

  bloc=InitBloc(1,id,0,-1,nom,donnees, dirP);  
  WRITE(partition, id, bloc);
  addLink(partition, dirP, id); 
}
int idNameFile(int partition, int dirP, char nom[DIM_NAME_FILE]){
    char blcName[DIM_NAME_FILE];
	Bloc parent=READ(partition, dirP); Bloc bloc; int i=0;

	while(i<=14)
	{
	 if(parent.LINK_TO_BLOC[i]!=-1) 
	 { 	
		 blocName(partition, parent.LINK_TO_BLOC[i], blcName);
		 if(!strcmp(nom,blcName))
		 {  return parent.LINK_TO_BLOC[i]; }
	 }
     i++;
	}
printf(" '%s' cet élément n'existe pas dans ce repertoir !\n", nom);
return -1;
}

/*! @brief to verify if a folder exist in the current directory */
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

void recNamePath(int partition, int currDir){
 Bloc bloc=READ(partition, currDir);
	if(bloc.PERE !=-1){   recNamePath(partition, bloc.PERE);
		printf("/%s",bloc.BNAME);
	}
}

// Prends un F_directory (son id) et un élément (son id)
// Crée la copie de orignal et le met dans Repertoir Pere
// R E C U R S I V I T E

int pastFolder(int partition, int F_directory, int original_ID){
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
	memcpy(copie.LINK_TO_BLOC, tab_init, sizeof(tab_init));
	strcpy(copie.BNAME,original.BNAME);
	WRITE(partition, id, copie);
	 while(i<=14)
	 {
	    if(original.LINK_TO_BLOC[i]!=-1)
		{
			enfant = pastFolder(partition, id, original.LINK_TO_BLOC[i]);
			if(enfant != copie.ID_BLOC)
			  {copie.LINK_TO_BLOC[i]=enfant;}
		}
		 i++;
	 }
	WRITE(partition, id, copie);
	return id;
}
void clean_stdin(void) { 
    int c; 
  
    do { 
        c = getchar(); 
    } while (c != '\n' && c != EOF); 
}
void exitError(char *message) {
  int errnum;
  fprintf(stderr,"exitError : %s\n",message);
  exit(1);
}


  


