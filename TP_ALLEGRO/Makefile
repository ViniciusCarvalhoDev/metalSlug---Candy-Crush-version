ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: candyc.exe

candyc.exe: candyc.o
	gcc -o candyc.exe candyc.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

candyc.o: candyc.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c candyc.c		

clean:
	del candyc.o
	del candyc.exe