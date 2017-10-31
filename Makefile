exec : main.o tcp.o
	gcc -o web main.o tcp.o -lcrypto
main.o:main.c tcp.c
	gcc -c main.c
tcp.o:tcp.c tcp.h
	gcc -c tcp.c
