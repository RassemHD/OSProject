#ifndef UTIL_H
#define UTIL_H

int  addLink(int partition, int RepertoirParent, int lien);
int  rmvLink(int partition, int RepertoirParent, int lien);
void rmvFolder(int partition,  int RepertoirParent, int NumeroBloc);
void rmvFile(int partition, int RepertoirParent, int NumeroBloc);
int  idNameFile(int partition, int RepertoirParent, char *nom);
void blocName(int partition, int NumeroBloc, char *blocName);
int elementExistsInFolder(int partition, int RepertoirParent, char nom[MAXNOMFICHIER]);
void recNamePath(int partition, int RepertoirCourant);
void exitError(char *message);
void clean_stdin(void);

#endif