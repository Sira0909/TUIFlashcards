all: TUIFlashcards 
C_SOURCES= ${wildcard src/*.c src/*/*.c src/*/*/*.c}
HEADERS = ${wildcard include/*.h} 
DESTDIR = "/usr/bin/"
CC?=gcc
EXTENTION=

ifeq ($(OS),Windows_NT)
	EXTENTION=.exe
	CCFLAGS+= -I C:\msys64\ucrt64\include\ncurses
endif
TUIFlashcards: $(C_SOURCES) $(HEADERS)
	$(CC) $(C_SOURCES) -I ./include/  -o $@ -g -lformw -lncursesw $(CCFLAGS)

install: TUIFlashcards
	cp TUIFlashcards $(DESTDIR)
clean:
	rm -f ./TUIFlashcards$(EXTENTION)
