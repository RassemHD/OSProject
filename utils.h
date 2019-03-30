#ifndef UTIL_H
#define UTIL_H

int  addLink(int partition, int dirP, int lien);
int  rmvLink(int partition, int dirP, int lien);
void rmvFolder(int partition,  int dirP, int NumeroBloc);
void rmvFile(int partition, int dirP, int NumeroBloc);
int  idNameFile(int partition, int dirP, char *nom);
void blocName(int partition, int NumeroBloc, char *blocName);
int elementExistsInFolder(int partition, int dirP, char *nom);
void recNamePath(int partition, int RepertoirCourant);
void exitError(char *message);
void clean_stdin(void);

#endif
