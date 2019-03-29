all: fmanager doc

doc : fmanager.c partition.c utils.c commands.c
	doxygen fmanager.c partition.c utils.c commands.c

partition.o: partition.c utils.c partition.h utils.h
	gcc -c partition.c

utils.o:  partition.c utils.c partition.h utils.h
	gcc -c utils.c

commands.o:  commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c commands.c

fmanager.o: fmanager.c commands.c partition.c utils.c partition.h utils.h commands.h
	gcc -c fmanager.c

fmanager: fmanager.o utils.o commands.o
	gcc partition.o utils.o commands.o view.o -o fmanager

clean:
	rm *.o fmanager test
