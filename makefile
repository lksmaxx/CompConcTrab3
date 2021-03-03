


all: lab.out
	./lab.out entrada.bin

lab.out: lab.c
	gcc lab.c -o lab.out -lpthread -lm -Wall


write: wr.out
	./wr.out entrada.bin

wr.out: wr.c
	gcc wr.c -o wr.out lab.out -Wall
