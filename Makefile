CC = gcc
CFLAGS = -Wall

all: mywai mywhich mycp

mywai: mywai.o
	$(CC) mywai.o -o mywai
	
mywhich: mywhich.o
	$(CC) mywhich.o -o mywhich
	
mycp: mycp.o
	$(CC) mycp.o -o mycp

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f mywai mycp mywhich $(wildcard *.o)