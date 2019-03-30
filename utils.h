#ifndef UTIL_H
#define UTIL_H

int  addLink(int partition, int dirP, int lien);
int  rmvLink(int partition, int dirP, int lien);
void rmvFolder(int partition,  int dirP, int n_b);
void rmvFile(int partition, int dirP, int n_b);
int  idNameFile(int partition, int dirP, char *nom);
void blocName(int partition, int n_b, char *blocName);
int elementExistsInFolder(int partition, int dirP, char *nom);
void exitError(char *message);
int file_exist(char* nom_fichier);
void recNamePath(int partition, int RepertoirCourant);

#endif
