all: main.c
	gcc main.c -g -o main.o -lmariadbclient -lwiringPi -lwiringPiDev -c
	gcc -g -o main main.o -lmariadbclient -lwiringPi -lwiringPiDev

clean:
	rm -f main 
	rm -f main.o
