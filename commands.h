#ifndef COMMANDS_H
#define COMMANDS_H

void cat(int partition, int nB);
void ls(int partition, int nB);
void rm(int partition, int RepertoirParent, char *nom);
int mmkdir(int partition, int RepertoirParent, char *nom);
void pwd(int partition, int RepertoirCourant);
int cp(int partition, int RepertoirParent);
int past(int partition, int RepertoirPere, int original_ID);
int pastFolder(int partition, int RepertoirPere, int original_ID);
void touch(int partition, int RepertoirParent, char *nom, char *donnees);
int cd(int partition, int RepertoirCourant, char *nom);
void man(int argc,char* argv);

#endif
