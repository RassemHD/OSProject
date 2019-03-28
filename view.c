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
#include "commands.h"


int main(int argc, char ** argv) 
{
  if (argc != 2) { exitError("Utilisation : ./creat <Nom Partition>)"); }
 
  int partition = open(argv[1], O_RDWR);
  char nom[MAXNOMFICHIER]; 
  int b=1;
  char nombloc[MAXNOMFICHIER];
  char menu[10];
  int RepertoirCourant =0;
  int Copie=-1;

  printf("Commandes :\n dossier => Création d'un dossier\n");
	printf("delete=>Supprimme un élément du répertoir\n ls => Affiche contenu répertoir\n");

	while(b>0)
	{
	pwd(partition,RepertoirCourant);			   
	fgets(menu,10,stdin);   
	strtok(menu, "\n");

	if (strcmp(menu, "dossier") == 0) 
    { 
      	 mkdir(partition, RepertoirCourant);
    } 
    else if (strcmp(menu, "delete") == 0)
    {
         rm(partition, RepertoirCourant);
    }
    else if (strcmp(menu, "ls") == 0)
    {
         ls(partition, RepertoirCourant);
    }		
    else if (strcmp(menu, "cd") == 0)
    {
         RepertoirCourant = cd(partition, RepertoirCourant);
    }
	  else if (strcmp(menu, "copier") == 0)
    {
         Copie = cp(partition, RepertoirCourant);
    }
	  else if (strcmp(menu, "coller") == 0)
    {
         past(partition, RepertoirCourant, Copie);
    }	
    else if (strcmp(menu, "exit") == 0)
    {  
        b=-1;
    }
	  else{
      printf("La commande '%s' n'est pas reconnue",menu);}
	}

  close(partition);
  return 0;
}
