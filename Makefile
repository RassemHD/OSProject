all: creat view doc

doc : view.c partition.c utils.c creat.c commands.c
	doxygen view.c partition.c utils.c creat.c commands.c

partition.o: partition.c utils.c partition.h utils.h
	gcc -c partition.c

utils.o:  partition.c utils.c partition.h utils.h
	gcc -c utils.c

commands.o:  commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c commands.c

creat.o: creat.c commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c creat.c

view.o: view.c commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c view.c

creat: partition.o utils.o commands.o creat.o
	gcc partition.o utils.o commands.o creat.o -o creat

view: view.o utils.o commands.o creat.o
	gcc partition.o utils.o commands.o view.o -o view

clean:
	rm *.o creat view test
