all:			envio_socket recepcao_socket

envio_socket:   envio_socket.c
			gcc envio_socket.c -o envio

recepcao_socket:	recepcao_socket.c
			gcc recepcao_socket.c -o recepcao

clean:
			rm -f envio recepcao *~

