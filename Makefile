all:			envio_socket servidor

envio_socket:   envio_socket.c
			gcc envio_socket.c -o envio

servidor:	servidor.c
			gcc servidor.c -o servidor

clean:
			rm -f envio servidor *~