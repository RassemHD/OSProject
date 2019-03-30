/*!
 * @file main.c
 * @version 4
 * @author Saint-Amand Matthieu, Castelain Julien, Hachoud Rassem
 * @brief Allow to build the virtual disk.
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

/*!
 * @brief Initialize a new boc with the specified parameters
 * @param fd : file or directory
 * @param id : identifier in the file system
 * @param libre : the bloc is free or not
 * @param suiv : id of the next bloc where the file continue
 * @param  nom : name of the file
 * @param donnes : initial data in the created file
 * @param pere : id of the parent directory
 */
Bloc InitBloc(int fd, int id, int libre, int suiv, char nom[DIM_NAME_FILE], char *donnees, int pere)
{
 int initLinkTab[15] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }; // to init the links table in the structure of the bloc
  Bloc bloc;
  bloc.F_D =fd ;  bloc.ID_BLOC=id; 
  bloc.LIBRE=libre; bloc.NEXT=suiv; 
  strcpy(bloc.BNAME, nom);
  memcpy(bloc.LINK_TO_BLOC, initLinkTab, sizeof(initLinkTab));
  bloc.PERE = pere;
  return bloc;
}

/*!
 * @brief Create a new bloc in the specified partition, with the given parameters
 * @param part: decriptor of the partition
 * @param fd : file or directory
 * @param id : identifier in the file system
 * @param libre : the bloc is free or not
 * @param suiv : id of the next bloc where the file continue
 * @param  nom : name of the file
 * @param donnes : initial data in the created file
 * @param pere : id of the parent directory
 */
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

/*! 
 *	@brief This function create the base of the virtual file. 
 *  @param name: the name of the partition
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

/*!
 * @brief retrieve the first bloc with no data
 * @param partition  : the descriptor of the partition
 */
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
	exitError("out of memory !\n"); 
}

/*!
 * @brief repositions the file offset of the open file
 * @param partition : file descriptor
 * @param NumeroBloc : id of the bloc
 */
void LSEEK(int partition, int NumeroBloc)
{	long OFF_SET = NumeroBloc*sizeof(Bloc);
	if ( OFF_SET != lseek(partition, NumeroBloc*sizeof(Bloc), SEEK_SET) ){
		exitError("lseek error");
	}
}

/*!
 * @brief write in the specified file 
 * @param partition : file descriptor
 * @param NumeroBloc : id of the bloc
 * @param bloc : the bloc to write
 */
void WRITE(int partition, int NumeroBloc, Bloc bloc)
{
	LSEEK(partition, NumeroBloc);
	if (write(partition, &bloc,sizeof(Bloc)) != sizeof(Bloc)){
	 	exitError("Write error");
	}
}

/*!
 * @brief read the specified file 
 * @param partition : file descriptor
 * @param NumeroBloc : id of the bloc
 */
Bloc READ (int partition, int NumeroBloc)
{
	if (NumeroBloc > NOMBREBLOCS) {
		exitError("Bloc hors de la partition");
	}
	LSEEK(partition, NumeroBloc);
    Bloc bloc;
    if (sizeof(Bloc) != read(partition,&bloc,sizeof(Bloc))) { 
    	exitError("Read : exitError");
    }
	return bloc;
}
