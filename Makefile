CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build:
	$(CC) $(CFLAGS) image_editor.c functions.c cases.c utilities.c -g -o image_editor -lm
	
clean:
	rm -f image_editor

