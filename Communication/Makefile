################################
#
# Makefile CS (client/serveur)
#  mode connecté
#  bibliothèque : fonctionsTCP
#
################################

# For Solaris
#LD_FLAGS = -lsocket -lnsl

# For Linux
LD_FLAGS =

all: joueur serveur

#fonctionsTCP.o: fonctionsTCP.c fonctionsTCP.h
#	gcc -Wall -c fonctionsTCP.c

joueur: joueur.c fonctionsTCP.o  outils.o
	gcc -Wall joueur.c -o joueur fonctionsTCP.o outils.o  $(LD_FLAGS)

serveur: serveur.c fonctionsTCP.o  
	gcc -fPIC -Wall serveur.c -o serveur yokai-fPIC.o fonctionsTCP.o   $(LD_FLAGS)	



clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm joueur ;rm serveur ; 
