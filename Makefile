all: TUIFlashcards 
C_SOURCES= ${wildcard src/*.c}
HEADERS = ${wildcard src/*.h} 
CC?=gcc

TUIFlashcards: $(C_SOURCES) $(HEADERS)
	$(CC) -lform -lncurses $(C_SOURCES) -o $@

clean:
	rm -f ./TUIFlashcards 
