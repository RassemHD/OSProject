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


int addLink(int partition, int RepertoirParent, int lien) 
{
	int i=0;
	Bloc parent = READ(partition,RepertoirParent);
    while(i<14 && parent.LIENS[i] != -1)
	  { i++; }
	
 	if(i<=14) 
	{ 
	parent.LIENS[i]=lien; 
	WRITE(partition,RepertoirParent, parent); 
	return 1; 
	}
	else { 	exitError("addLink : Mémoire pleine !\n");  }
}

int rmvLink(int partition, int RepertoirParent, int lien) 
{
	int i=0;
    Bloc parent = READ(partition,RepertoirParent);
	while(i<14 && parent.LIENS[i] != lien)
	{ i++; }
	
 	if(i<=14)
	{parent.LIENS[i]=-1; WRITE(partition,RepertoirParent, parent); return 1;}
}


void blocName(int partition, int NumeroBloc, char blocName[MAXNOMFICHIER])
{
	Bloc bloc = READ (partition,NumeroBloc);
	strcpy( blocName, bloc.NOM_BLOC);

}

void rmvFile(int partition,  int RepertoirParent, int NumeroBloc)
{
	if (NumeroBloc > NOMBREBLOCS) {exitError("Bloc hors de la partition"); }
	
	rmvLink(partition, RepertoirParent, NumeroBloc);
    Bloc bloc=READ(partition, NumeroBloc);
	int BLOCSUIVANT = bloc.BLOC_SUIVANT;
	bloc.LIBRE = 1; 
	bloc.BLOC_SUIVANT = -1;
	strcpy(bloc.NOM_BLOC, "");
    strcpy(bloc.DONNEES, ""); 
	memcpy(bloc.LIENS, tab_init, sizeof(tab_init));
	WRITE(partition, NumeroBloc, bloc);
	
	if(BLOCSUIVANT!=-1)
	   { rmvFile(partition, RepertoirParent, BLOCSUIVANT);   }
}

void rmvFolder(int partition,  int RepertoirParent, int NumeroBloc)
{
	rmvLink(partition, RepertoirParent, NumeroBloc);
	int i=0; 
	Bloc dossier=READ(partition,NumeroBloc);
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

 rmvFile(partition,RepertoirParent,NumeroBloc);
}

int idNameFile(int partition, int RepertoirParent, char nom[MAXNOMFICHIER])
{
    char blcName[MAXNOMFICHIER];
	Bloc parent=READ(partition, RepertoirParent); Bloc bloc; int i=0;
	while(i<=14)
	{
	 if(parent.LIENS[i]!=-1)
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

int elementExistsInFolder(int partition, int RepertoirParent, char nom[MAXNOMFICHIER])
{
    char blcName[MAXNOMFICHIER];
	Bloc parent=READ(partition, RepertoirParent); Bloc bloc; int i=0;
	while(i<=14)
	{
	 if(parent.LIENS[i]!=-1)
	 { 	
		 blocName(partition, parent.LIENS[i], blcName);
		 if(!strcmp(nom,blcName)) {  return 1; }
	 }
     i++;
	}
return -1;
}

void recNamePath(int partition, int RepertoirCourant)
{
Bloc bloc=READ(partition, RepertoirCourant);
if(bloc.PERE !=-1)
{   recNamePath(partition, bloc.PERE);
	printf("/%s",bloc.NOM_BLOC);
}

}

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
strcpy(copie.DONNEES, original.DONNEES);
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

void exitError(char *message) 
{
  int errnum;
  fprintf(stderr,"error : %s\n",message);
  exit(1);
}


  


