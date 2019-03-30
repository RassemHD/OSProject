#ifndef PARTITION_H
#define PARTITION_H

//be careful, if this parameters are changed you gotta create a new virtual file with [-c] command
#define NOMBREBLOCS 100          
#define TAILLEBLOC 50000		 
#define DIM_NAME_FILE  30		 
#define TAILLEREPERTOIR 15      
#define MAXDONNEEBLOC (TAILLEBLOC - 5*sizeof(int) - sizeof(int[15])  - sizeof(char[DIM_NAME_FILE])) 
typedef struct Bloc
{
  char BNAME[DIM_NAME_FILE];         // The bloc can be a file or a directory, so this array retrieve the bloc name
  int LINK_TO_BLOC[TAILLEREPERTOIR];	      //this array contains a set of link where each link is a path towards another bloc
  int PERE;						             // the id of the parent directory
  int NEXT;				        // contains the bloc id which contains the rest of the file
  int F_D;			     // allow to know if the bloc is a file or a directory
  int ID_BLOC;                  // identity of the bloc
  int LIBRE;				           // to know if the bloc is free or not
}Bloc;

void LSEEK(int partition, int nB);
Bloc READ(int partition, int nB);
void WRITE(int partition, int nB, Bloc bloc);

Bloc InitBloc(int fd, int id, int libre, int suiv, char *nom, char *donnees, int pere);
Bloc creatBloc(int part,int fd, int id, int libre, int suiv, char *nom, char *donnees, int pere);
void creatPartition(char*name);
int firstEmptyBloc(int partition);

void printBlock(Bloc bloc);
void printPartition(int partition);

#endif
