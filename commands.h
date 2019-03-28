#ifndef COMMANDS_H
#define COMMANDS_H

void cat(int partition, int NumeroBloc);
void ls(int partition, int NumeroBloc);
void rm(int partition, int RepertoirParent);
int mkdir(int partition, int RepertoirParent);
void pwd(int partition, int RepertoirCourant);
int cp(int partition, int RepertoirParent);
int past(int partition, int RepertoirPere, int original_ID);
int pastFolder(int partition, int RepertoirPere, int original_ID);
int cd(int partition, int RepertoirCourant);

#endif