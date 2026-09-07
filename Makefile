all: TUIFlashcards 
C_SOURCES= ${wildcard src/*.c src/*/*.c src/*/*/*.c}
HEADERS = ${wildcard include/*.h} 
DESTDIR = "/usr/bin/"
CC?=gcc
EXTENTION=


ifeq ($(OS),Windows_NT)
	EXTENTION=.exe
	CCFLAGS+= -I C:\msys64\ucrt64\include\ncurses
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		LDFLAGS += "-L/usr/local/opt/ncurses/lib"
		CCFLAGS += "-I/usr/local/opt/ncurses/include"
	endif
endif
TUIFlashcards: $(C_SOURCES) $(HEADERS)
	$(CC) $(C_SOURCES) -I ./include/ $(LDFLAGS) $(CCFLAGS) -o $@ -g -lformw -lncursesw -Wall

install: TUIFlashcards
	cp TUIFlashcards $(DESTDIR)
clean:
	rm -f ./TUIFlashcards$(EXTENTION)




ifdef XDG_CONFIG_HOME
CONFIGHOME?="${XDG_CONFIG_HOME}/TUIFlashcards/"
else ifdef APPDATA
CONFIGHOME?= "${APPDATA}/TUIFlashcards/"
else ifdef HOME
CONFIGHOME?= "${HOME}/.config/TUIFlashcards/"
endif
bootstrapConfig:
	mkdir $(CONFIGHOME)
	mkdir $(CONFIGHOME)/Lists/
	cp ./doc/exampleList.list $(CONFIGHOME)/Lists/exampleList.list


	
