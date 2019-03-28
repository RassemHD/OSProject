all: creat view

fonctions.o: fonctions.c head.h
	gcc -c fonctions.c

creat.o: creat.c fonctions.c head.h
	gcc -c creat.c

view.o: view.c fonctions.c head.h
	gcc -c view.creat

creat: creat.o fonctions.o
	gcc creat.o fonctions.o -o creat

view: view.o fonctions.o
	gcc view.o fonctions.o -o view

clean:
	rm -rf *.o creat view test