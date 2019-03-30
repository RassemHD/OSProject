/*!
 * @file main.c
 *
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

// Prends le parent et ajoute l'id dans son tableau LIENS
int addLink(int partition, int dirP, int lien) 
{
	int i=0;
	Bloc parent = READ(partition,dirP);
    while(i<14 && parent.LIENS[i] != -1)
	  { i++; }
	
 	if(i<=14) { 
		parent.LIENS[i]=lien; 
		WRITE(partition,dirP, parent); 
		return 1; 
	}
	else 	
		exitError("addLink : Mémoire pleine !\n"); 
}

// Supprimme lien du tableau LIENS du repertoir parent, retourne 1 si bien supprimmé -1 sinon
int rmvLink(int partition, int dirP, int lien) 
{
	int i=0;
    Bloc parent = READ(partition,dirP);
	while(i<14 && parent.LIENS[i] != lien)
	{ i++; }
	
 	if(i<=14)
	{parent.LIENS[i]=-1; WRITE(partition,dirP, parent); return 1;}
	/*else
	{	exitError("rmvLink : ce lien ne se trouve pas dans le LIENS du Repertoir pere !\n");}*/
}


// Prends l'id du bloc, et met son nom à l'intérieur de blocName
void blocName(int partition, int nB, char blocName[DIM_NAME_FILE])
{
	Bloc bloc = READ (partition,nB);

	strcpy( blocName, bloc.NOM_BLOC);

}

// Supprimme le bloc dont l'id est passé en argument
void rmvFile(int partition,  int dirP, int nB)
{
	if (nB > NOMBREBLOCS) {exitError("Bloc hors de la partition"); }
	
	rmvLink(partition, dirP, nB);
    Bloc bloc=READ(partition, nB);
	int BLOCSUIVANT = bloc.BLOC_SUIVANT;

	bloc.LIBRE = 1; 
	bloc.BLOC_SUIVANT = -1;
	strcpy(bloc.NOM_BLOC, "");
	memcpy(bloc.LIENS, tab_init, sizeof(tab_init));
	WRITE(partition, nB, bloc);
	
	if(BLOCSUIVANT!=-1)
	   { rmvFile(partition, dirP, BLOCSUIVANT);   }
}

void rmvFolder(int partition,  int dirP, int nB)
{
	// On supprimme le lien de parent bye bye, maitenant faut vraiment le vider
	rmvLink(partition, dirP, nB);
	int i=0; 
	Bloc dossier=READ(partition,nB);
	Bloc bloc;

	while(i<=14)
	{	
		if(dossier.LIENS[i]!=-1)
		 {
			bloc=READ(partition,dossier.LIENS[i]);
			if(bloc.FICHIER_DOSSIER)
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

// Retourne l'id qui correspond au nom d'un fichier à l'intérieur du répertoir pere
// Si n'existe pas retourne -1 
int idNameFile(int partition, int dirP, char nom[DIM_NAME_FILE])
{
    char blcName[DIM_NAME_FILE];
	Bloc parent=READ(partition, dirP); Bloc bloc; int i=0;

	while(i<=14)
	{
	 if(parent.LIENS[i]!=-1) // C.A.D Un Fichier/Dossier qui se trouve à l'intérieur de Repertoir Parent
	 { 	
		 blocName(partition, parent.LIENS[i], blcName);
		 if(!strcmp(nom,blcName))
		 {  return parent.LIENS[i]; }
	 }
     i++;
	}
printf(" '%s' cet élément n'existe pas dans ce repertoir !\n", nom);
return -1;
}

// Vérifie si un fichier/dossier qui porte un certain nom existe ou non dans un répertoir
// 1=>existe  -1=>n'existe pas
int elementExistsInFolder(int partition, int dirP, char nom[DIM_NAME_FILE])
{
    char blcName[DIM_NAME_FILE];
	Bloc parent=READ(partition, dirP); Bloc bloc; int i=0;

	while(i<=14)
	{
	 if(parent.LIENS[i]!=-1) // C.A.D Un Fichier/Dossier qui se trouve à l'intérieur de Repertoir Parent
	 { 	
		 blocName(partition, parent.LIENS[i], blcName);
		 if(!strcmp(nom,blcName)) {  return 1; }
	 }
     i++;
	}
return -1;
}

// Affichage du répertoir courant
void recNamePath(int partition, int RepertoirCourant)
{
Bloc bloc=READ(partition, RepertoirCourant);
if(bloc.PERE !=-1)
{   recNamePath(partition, bloc.PERE);
	printf("/%s",bloc.NOM_BLOC);
}

}

// Prends un RepertoirPere (son id) et un élément (son id)
// Crée la copie de orignal et le met dans Repertoir Pere
// R E C U R S I V I T E

int pastFolder(int partition, int RepertoirPere, int original_ID)
{
	int enfant;
	int i=0;
	int id =firstEmptyBloc(partition);
	Bloc original=READ(partition, original_ID);
	Bloc copie;
	addLink(partition, RepertoirPere,  id);
	copie.FICHIER_DOSSIER= original.FICHIER_DOSSIER;
	copie.ID_BLOC=id;
	copie.LIBRE=0;
	copie.BLOC_SUIVANT=-1;
	copie.PERE=RepertoirPere;
	memcpy(copie.LIENS, tab_init, sizeof(tab_init));
	strcpy(copie.NOM_BLOC,original.NOM_BLOC);
	WRITE(partition, id, copie);
	 while(i<=14)
	 {
	    if(original.LIENS[i]!=-1)
		{
			enfant = pastFolder(partition, id, original.LIENS[i]);
			if(enfant != copie.ID_BLOC)
			  {copie.LIENS[i]=enfant;}
		}
		 i++;
	 }
	WRITE(partition, id, copie);
	return id;
}

void clean_stdin(void) 
{ 
    int c; 
  
    do { 
        c = getchar(); 
    } while (c != '\n' && c != EOF); 
}

// Imprime le message sur le flux standard stderr
void exitError(char *message) 
{
  int errnum;
  fprintf(stderr,"exitError : %s\n",message);
  //fprintf(stderr, "%s\n", strerror( errnum ));
  exit(1);
}


  


