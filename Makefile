all:			cliente servidor

cliente:   	cliente.c
			gcc cliente.c -o envio

servidor:	servidor.c
			gcc servidor.c -o servidor

clean:
			rm -f cliente servidor *~