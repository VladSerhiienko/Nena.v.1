CC=gcc
Flags=-m64 -g0 -std=gnu89

ex_5: main.o functions.o dfp.o matrix.o 
	$(CC) $(Flags) main.o functions.o dfp.o matrix.o -o ex_5 -lm 
main.o: main.c
	$(CC) $(Flags) -c main.c -o main.o
functions.o: functions.c
	$(CC) $(Flags) -c functions.c -o functions.o
dfp.o: dfp.c
	$(CC) $(Flags) -c dfp.c -o dfp.o
matrix.o: matrix.c
	$(CC) $(Flags) -c matrix.c -o matrix.o 
clean:
	rm -rf *.o ex_5
