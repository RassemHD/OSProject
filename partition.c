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


Bloc InitBloc(int fd, int id, int libre, int suiv, char nom[MAXNOMFICHIER], char *donnees, int pere)
{
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

void LSEEK(int partition, int NumeroBloc)
{	long OFF_SET = NumeroBloc*sizeof(Bloc);
	if ( OFF_SET != lseek(partition, NumeroBloc*sizeof(Bloc), SEEK_SET) )  
		{ exitError("lseek error"); }
}

void WRITE(int partition, int NumeroBloc, Bloc bloc)
{
	LSEEK(partition, NumeroBloc);
	if (write(partition, &bloc,sizeof(Bloc)) != sizeof(Bloc))
		{ exitError("Write : error"); }
}


Bloc READ (int partition, int NumeroBloc)
{
	if (NumeroBloc > NOMBREBLOCS) {exitError("Bloc out of partition"); }
	LSEEK(partition, NumeroBloc);
    Bloc bloc;
    if (sizeof(Bloc) != read(partition,&bloc,sizeof(Bloc))) { exitError("Read : error"); }
	return bloc;
}
