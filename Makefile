hepsi:	derle

derle:
	gcc -I ./include/ -o ./lib/fields.o -c ./src/fields.c
	gcc -I ./include/ -o ./bin/tarsau ./lib/fields.o ./src/main.c

