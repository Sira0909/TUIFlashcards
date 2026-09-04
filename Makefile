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
