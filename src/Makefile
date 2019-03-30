HEADERS = ../headers

all: fmanager doc

doc : fmanager.c partition.c utils.c commands.c
	doxygen fmanager.c partition.c utils.c commands.c

partition.o: partition.c utils.c $(HEADERS)
	gcc -c partition.c -I$(HEADERS)

utils.o:  partition.c utils.c $(HEADERS)
	gcc -c utils.c -I$(HEADERS)

commands.o:  commands.c partition.c utils.c $(HEADERS)
	gcc -c commands.c -I$(HEADERS)

fmanager.o: fmanager.c commands.c partition.c utils.c $(HEADERS)
	gcc -c fmanager.c -I$(HEADERS)

fmanager: fmanager.o utils.o commands.o partition.o $(HEADERS)
	gcc fmanager.o partition.o utils.o commands.o -o  fmanager

clean:
	rm *.o fmanager
