all: TUIFlashcards 
C_SOURCES= ${wildcard src/*.c src/play/*.c}
HEADERS = ${wildcard src/*.h src/play/*.h} 
CC?=clang

TUIFlashcards: $(C_SOURCES) $(HEADERS)
	clang -lform -lncurses $(C_SOURCES) -o $@

install: TUIFlashcards
	sudo cp TUIFlashcards /usr/bin/
clean:
	rm -f ./TUIFlashcards 
