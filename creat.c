/*!
 * @file creat.c
 * @version 4
 * @author Saint-Amand Matthieu, Castelain Julien, Hachoud Rassem
 * @brief This program allows you to create a block file, each block 
 * of which is identified with an ID, a name and linked to a parent block.
 * 
 *
 * @see partition.h
 * @see partition.h
 * @see command.h
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
#include "commands.h"
 
int main(int argc, char ** argv) 
{
  creatPartition(argv[1]);
  return 0;
}

