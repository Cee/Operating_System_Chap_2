out:	main.c	my_print.o
	gcc -m32 -Wno-implicit -o out my_print.o main.c
	./out
my_print.o:	my_print.asm
	./nasm/nasm -f macho32 -o my_print.o my_print.asm
clean:
	rm my_print.o out
