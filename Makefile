all: main

main: main.o
	gcc -Wall main.o -o main

main.o: main.c
	gcc -Wall -c main.c

clean:
	rm *.o
	rm main
