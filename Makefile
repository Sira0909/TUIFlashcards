all: main
C_SOURCES= ${wildcard src/*.c}
HEADERS = ${wildcard src/*.h} 
CC?=gcc

main: $(C_SOURCES) $(HEADERS)
	$(CC) -lform -lncurses $(C_SOURCES) -o $@

clean:
	rm -f ./main
