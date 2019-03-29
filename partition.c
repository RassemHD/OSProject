/*!
 * @file main.c
 * @version 4
 * @author Saint-Amand Matthieu, Castelain Julien, Hachoud Rassem
 * @brief 
 *
 * @see utils.h
 * @see partition.h
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


// Retourne un bloc initialisé avec les valeurs passées en arguments
Bloc InitBloc(int fd, int id, int libre, int suiv, char nom[MAXNOMFICHIER], char *donnees, int pere)
{
 // for simbolic link, for now, there is no link in the virtual file
 const int tab_init[15] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
 Bloc bloc;
 bloc.FICHIER_DOSSIER =fd ;  bloc.ID_BLOC=id; 
 bloc.LIBRE=libre; bloc.BLOC_SUIVANT=suiv; 
 strcpy(bloc.NOM_BLOC, nom);
 strcpy(bloc.DONNEES, donnees); 
 memcpy(bloc.LIENS, tab_init, sizeof(tab_init));
 bloc.PERE = pere;
 return bloc;
}

Bloc creatBloc(int part,int fd, int id, int libre, int suiv, char *nom, char *donnees, int pere)
{
  Bloc bloc = InitBloc(fd,id,libre,suiv,nom,donnees,pere);
  WRITE(part,id, bloc);
  if(pere!=-1){
      addLink(part,pere,id);
  }
  printf("creat bloc : %d\n",id);
  return bloc;
}
/*! @brief This function create the base of the virtual file. 
 * @see creat.c
 */
void creatPartition(char*name)
{
  Bloc bloc;
  int partition;
  
  partition = open(name, O_RDWR | O_CREAT,0777);

  bloc=creatBloc(partition,0,0,0,-1,"Home","",-1);    
  bloc=creatBloc(partition,0,1,0,-1,"Documents","",0);
  bloc=creatBloc(partition,0,2,0,-1, "Images","",0);
  bloc=creatBloc(partition,0,3,0,-1, "Vidéos","",0);
  bloc=creatBloc(partition,0,4,0,-1,"Audios","",0);
  
  for (int i = 5; i < 50; i++) bloc=creatBloc(partition,0,i,0,-1,"B1","DIR",-1);

  for (int i = 50; i < NOMBREBLOCS; i++) bloc=creatBloc(partition,1,i,1,i+1,"B2","FIL",-1);

  close(partition);
}

// Parcours la partition et l'affiche
void printPartition(int partition)
{
	Bloc bloc;
  for (int i = 0; i < NOMBREBLOCS; i++) 
  {  
    bloc=READ(partition,i);
    printf("Bloc Numéro %d :",i);   printBlock(bloc);
    // blocName(partition, i, blocName); printf("Nom deu bloc %d : %s\n",i,blocName);
  }
}
// Prends un bloc en argument, et affiche ses données
void printBlock(Bloc bloc)
{
   printf("\nType : %s | ID: %d | %s | Nom bloc : %s | Bloc suivant : %d\n", 
	 (bloc.FICHIER_DOSSIER?"Fichier":"Dossier"),bloc.ID_BLOC, (bloc.LIBRE?"Libre":"Occupé"), bloc.NOM_BLOC, bloc.BLOC_SUIVANT);
	
	if(bloc.FICHIER_DOSSIER)
	     {  printf("Data : %s\n",bloc.DONNEES); 
		    printf("Taille max data %ld\t Taille réelle data %ld\t | Espace restant sur ce bloc %ld\n",   
				             MAXDONNEEBLOC, strlen(bloc.DONNEES), (MAXDONNEEBLOC-strlen(bloc.DONNEES))); 
		    printf("Data contenu dans ce fichier : %s", bloc.DONNEES);}
	else { for(int i=0;i<15;i++) { printf("%d |", bloc.LIENS[i]);  } }
   printf("\n\n");  

}




// Retourne l'id du premier bloc libre dans la partition (Ordre descendant), si tout les blocs occupé, retourne -1
int firstEmptyBloc(int partition)
{
  Bloc bloc; int i=0; 
	
  while ( i <= NOMBREBLOCS) 
  {
    LSEEK(partition, i);
    if (sizeof(struct Bloc) != read(partition,&bloc,sizeof(Bloc)))  { exitError("firstEmptyBloc : read exitError"); }
	  if(bloc.LIBRE) { return i; }
	  else  { i++;  }
  
  }
	exitError("firstEmptyBloc : Mémoire pleine !\n"); 
}

// "Redéfintion" de la fonction lseek, syntaxe plus légere et prise en compte des exitErrors
void LSEEK(int partition, int NumeroBloc)
{	long OFF_SET = NumeroBloc*sizeof(Bloc);
	if ( OFF_SET != lseek(partition, NumeroBloc*sizeof(Bloc), SEEK_SET) )  
		{ exitError("lseek exitError"); }
}

// "Redéfintion" de la fonction write, syntaxe plus légere et prise en compte des exitErrors
void WRITE(int partition, int NumeroBloc, Bloc bloc)
{
	LSEEK(partition, NumeroBloc);
	if (write(partition, &bloc,sizeof(Bloc)) != sizeof(Bloc))
		{ exitError("Write : exitError"); }
}


// "Redéfintion" de la fonction read, syntaxe plus légere et prise en compte des exitErrors
Bloc READ (int partition, int NumeroBloc)
{
	if (NumeroBloc > NOMBREBLOCS) {exitError("Bloc hors de la partition"); }
	LSEEK(partition, NumeroBloc);
    Bloc bloc;
    if (sizeof(Bloc) != read(partition,&bloc,sizeof(Bloc))) { exitError("Read : exitError"); }
	return bloc;
}
