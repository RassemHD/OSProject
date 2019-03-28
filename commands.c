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
// Affiche le contenu d'un REPERTOIR passé en argument
void ls(int partition, int NumeroBloc)
{
    Bloc bloc=READ(partition, NumeroBloc);
	Bloc b;
	int i=0, vide=1;
	while(i < 15)
	{ 
		if(bloc.LIENS[i] != -1)
		{
		 b=READ(partition, bloc.LIENS[i]);
			if(b.FICHIER_DOSSIER)
			{ printf("'%s'\t",b.NOM_BLOC);} //Fichier
			else
			{ printf("\"%s\":%d:\t",b.NOM_BLOC,b.ID_BLOC);}	//Dossier			
		 vide=0;
		}
	i++;
	
	}
    if(vide) {printf("Le dossier '%s' est vide !\n",bloc.NOM_BLOC);}
	else {printf("\n");}
}

// Affiche le contenu d'un FICHIER passé en argument
void cat(int partition, int NumeroBloc)
{
    Bloc bloc;
	do {
	bloc=READ(partition, NumeroBloc);
	printf("%s",bloc.DONNEES);
	NumeroBloc=bloc.BLOC_SUIVANT;
	}while(NumeroBloc!=-1);

}

// Supprimme un élémet (dossier ou fichier) d'un Répertoir
void rm(int partition, int RepertoirParent)
{
  char nom[MAXNOMFICHIER];
  printf("Fichier à supprimmer :   ");
  fgets(nom,sizeof(nom),stdin);     
  strtok(nom, "\n"); strtok(nom, " ");
  Bloc bloc;
  int id_bloc= idNameFile(partition, RepertoirParent, nom);
		
	if(id_bloc!=-1)
	{
  bloc = READ(partition,id_bloc);
		
  if(bloc.FICHIER_DOSSIER)
   {  	rmvFile(partition, RepertoirParent, id_bloc); 	}
  else
   { rmvFolder(partition, RepertoirParent, id_bloc);}
	}
}

// creation dossier
int mkdir(int partition, int RepertoirParent)
{ 
  Bloc bloc; char nom[MAXNOMFICHIER];
  printf("Nom Dossier :   ");
  fgets(nom,sizeof(nom),stdin);     
  strtok(nom, "\n");
	
	if(elementExistsInFolder(partition,RepertoirParent, nom) == -1)
	{
  int id = firstEmptyBloc(partition);
  bloc=InitBloc(0,id,0,-1,nom,"",RepertoirParent);       
  WRITE(partition, id, bloc);
  addLink(partition, RepertoirParent, id); 
  return 0;
	}
	
  else
	{printf("Un élément dans le répertoir porte déjà ce nom !\n"); return 0;}
}

// changer repertoire
int cd(int partition, int RepertoirCourant)
{
  char nom[MAXNOMFICHIER];
  printf("Se déplacer dans le dossier :   ");
  fgets(nom,sizeof(nom),stdin);     
  strtok(nom, "\n");
   if (strcmp(nom, "..") == 0) 
   {
	  Bloc parent = READ(partition, RepertoirCourant);
	  if(parent.PERE == -1)
	  {printf("Nous somme à la racine de cette partition !\n"); return RepertoirCourant;}
	   else
	  {return parent.PERE;}
   }
   else
   {
	int id_bloc= idNameFile(partition, RepertoirCourant, nom);
	   if(id_bloc!=-1)
	   {
         Bloc bloc = READ(partition, id_bloc);
	     if(bloc.FICHIER_DOSSIER)
		 {printf(" '%s' est un fichier pas un dossier !\n",nom); return RepertoirCourant;}
		 else
		 {return bloc.ID_BLOC;}
	   }
	   else
	   {
	     return RepertoirCourant;
	   
	   }
	}
  
  
}
// Affichage du répertoir courant actuelle 
void pwd(int partition, int RepertoirCourant)
{
printf("\nuser:/Home");
Bloc bloc=READ(partition, RepertoirCourant);
if(bloc.PERE !=-1)
{   recNamePath(partition, bloc.PERE);
	printf("/%s",bloc.NOM_BLOC);
}
printf(">");

}

// copier fichier
int cp(int partition, int RepertoirParent)
{
  char nom[MAXNOMFICHIER];
  printf("Elément à copier :   ");
  fgets(nom,sizeof(nom),stdin);     
  strtok(nom, "\n");
  int id= idNameFile(partition, RepertoirParent, nom);
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

// coller
int past(int partition, int RepertoirPere, int original_ID)
{
	char nom[MAXNOMFICHIER];
	blocName(partition, original_ID, nom);
	int id;
	
  if(original_ID == -1)
	{
	  printf("Aucun fichier copier en attente ! \n"); 
	  return -1;
    }
  else
  {
	if(elementExistsInFolder(partition,RepertoirPere, nom) == -1)  
	{
       Bloc original= READ(partition, original_ID);
       printf("L'élément collé dans ce répertoir est le %s : %s\n",original.FICHIER_DOSSIER?"Fichier":"Dossier", original.NOM_BLOC);
        //addLink(partition, RepertoirCourant,  IDCopie);
      if(original.FICHIER_DOSSIER)
      {
	  //CollerFichier();
      }
      else
      {
	  pastFolder(partition, RepertoirPere, original_ID);
      }
	}
	else 
	{printf("Un élément dans le répertoir porte déjà ce nom !\n"); return -1;}
	
  return 1;
  }

}

