#ifndef COMMANDS_H
#define COMMANDS_H

void cat(int partition, int nB);
void ls(int partition, int nB);
void rm(int partition, int dirP, char *nom);
int mmkdir(int partition, int dirP, char *nom);
void pwd(int partition, int RepertoirCourant);
int cp(int partition, int dirP, char *nom);
int past(int partition, int F_directory, int original_ID);
int pastFolder(int partition, int F_directory, int original_ID);
void touch(int partition, int dirP, char *nom, char *donnees);
int cd(int partition, int RepertoirCourant, char *nom);
void man(int argc,char* argv);

#endif
