all: TUIFlashcards 
C_SOURCES= ${wildcard src/*.c src/*/*.c src/*/*/*.c}
HEADERS = ${wildcard include/*.h} 
DESTDIR = "/usr/bin/"
CC?=gcc

TUIFlashcards: $(C_SOURCES) $(HEADERS)
	$(CC) $(C_SOURCES) -I ./include/ -o $@ -g -lformw -lncursesw

install: TUIFlashcards
	cp TUIFlashcards $(DESTDIR)
clean:
	rm -f ./TUIFlashcards 
