#ifndef PARTITION_H
#define PARTITION_H

#define NOMBREBLOCS 100          // Nombre de Blocs qui composent la partition = si mémoire sup : blocage de l'écriture
#define TAILLEBLOC 32768		 // Taille de chaque Bloc
#define DIM_NAME_FILE  20		 // Taille maximale pour le nom d'un Bloc (Fichier ou Dossier)
#define TAILLEREPERTOIR 15       // Nombre de documents (Fichier ou repertoir) qu'un repertoir peut contenir
#define MAXDONNEEBLOC (TAILLEBLOC - 5*sizeof(int) - sizeof(int[15])  - sizeof(char[DIM_NAME_FILE])) // Taille maximale de la donnée que le bloc peut contenir

// Structure d'un Bloc
typedef struct Bloc
{
  int FICHIER_DOSSIER;			     // Indique si le bloc représente un fichier ou un dossier (1 => fichier 0 => dossier)
  int ID_BLOC;                       // Identifiant unique du bloc à l'intérieur de la partition
  int LIBRE;				         // Indique si le bloc est occupé ou non (1 => Libre 0 => occupé) 
  char NOM_BLOC[DIM_NAME_FILE];      // Le nom du Bloc (Nom du fichier ou nom du dossier)
  int PERE;						     // L'id du répertoir Parent du fichier/dossier
  int LIENS[TAILLEREPERTOIR];			
  int BLOC_SUIVANT;				     // Contient l'id du bloc qui contient la suite du fichier
	 						         //Prends -1 si le fichier est contenu dans un seul bloc ou si le bloc représente un dossier
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
