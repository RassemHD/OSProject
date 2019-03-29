all: view doc

doc : view.c partition.c utils.c commands.c
	doxygen view.c partition.c utils.c commands.c

partition.o: partition.c utils.c partition.h utils.h
	gcc -c partition.c

utils.o:  partition.c utils.c partition.h utils.h
	gcc -c utils.c

commands.o:  commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c commands.c

view.o: view.c commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c view.c

view: view.o utils.o commands.o
	gcc partition.o utils.o commands.o view.o -o view

clean:
	rm *.o view test
