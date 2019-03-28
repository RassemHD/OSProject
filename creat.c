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

