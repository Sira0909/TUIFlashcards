all: TUIFlashcards 
C_SOURCES= ${wildcard src/*.c src/*/*.c src/*/*/*.c}
HEADERS = ${wildcard include/*.h} 
CC?=gcc

TUIFlashcards: $(C_SOURCES) $(HEADERS)
	$(CC) -lform -lncurses $(C_SOURCES) -I ./include/ -o $@ -g

install: TUIFlashcards
	sudo cp TUIFlashcards /usr/bin/
clean:
	rm -f ./TUIFlashcards 
