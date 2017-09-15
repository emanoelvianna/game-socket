#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdbool.h>
/* utilizando os utilitarios */
#include "estrutura.h"

/* definindo a porta de conexão  */
#define PORT 4242
/* definicao do tamanho do buffer */
#define BUFFER_LENGTH 1500
/* definicao do tamanho da matriz */
#define N_LINHAS 3
#define N_COLUNAS 3

char matriz [N_LINHAS][N_COLUNAS];
char *input_ifname;
unsigned char mac_server[ETHERNET_ADDR_LEN];

/* definição dos jogadores  */
bool jogador1 = false;
bool jogador2 = false;

/* metodo para incializar a matriz vazia */
void iniciarMatriz() 
{
  int i, j;
  for (i = 0; i < N_LINHAS; i++) {
    for (j = 0; j < N_COLUNAS; j++) {
      matriz[i][j] = '.';
    }
  }
}

/* metodo para desenhar a matriz */

void desenhaMatriz() 
{
  printf("   1   2   3   \n");
  printf(" 1 %c | %c | %c \n", matriz[0][0], matriz[0][1], matriz[0][2]);
  printf("  ---|---|--- \n ");
  printf("2 %c | %c | %c \n", matriz[1][0], matriz[1][1], matriz[1][2]);
  printf("  ---|---|--- \n ");
  printf("3 %c | %c | %c \n", matriz[2][0], matriz[2][1], matriz[2][2]);
  printf("\n");
}

/* metodo para adicionar uma jogada a matriz */
void adicionarJogada(int linha, int coluna, char peca) 
{
  int i, j;
  for (i = 0; i < N_LINHAS; i++) {
    for (j = 0; j < N_COLUNAS; j++) {
      if (linha == i && coluna == j)
        matriz[i][j] = peca;
    }
  }
}

/* metodo auxiliar para buscar o mac do server */
int getMacServer() 
{
		int fd;
    struct ifreq ifr;
     
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , input_ifname , IFNAMSIZ-1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);

	strcpy(mac_server, ifr.ifr_hwaddr.sa_data);

    return 0;
}

/* metodo para o servidor */
int servidor() 
{     
    /* Client and Server socket structures */
    struct sockaddr_in client, server;

    /* File descriptors of client and server */
    int serverfd, clientfd;
	char buffer[BUFFER_LENGTH];

    fprintf(stdout, "Iniciando o server para o game\n");

    /* Cria um descritor de socket do tipo RAW */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Problema ao criar o socket:");
        return EXIT_FAILURE;
    }

    /* Preparando configurações para o server */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memset(server.sin_zero, 0x0, 8);

	/* bind do socket para a porta */
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Socket bind erro:");
        return EXIT_FAILURE;
    }

	puts("Esperando jogadores ... \n");
   /* Sends the message to the client */
    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Client connected.\nWaiting for client message ...\n");

        /* Communicates with the client until bye message come */
        do {

            /* Zeroing buffers */
            memset(buffer, 0x0, BUFFER_LENGTH);

            /* Receives client message */
            int message_len;
            if((message_len = recv(clientfd, buffer, BUFFER_LENGTH, 0)) > 0) {
                buffer[message_len - 1] = '\0';
                printf("Client says: %s\n", buffer);
            }


            /* 'bye' message finishes the connection */
            if(strcmp(buffer, "bye") == 0) {
                send(clientfd, "bye", 3, 0);
            } else {
                send(clientfd, "yep\n", 4, 0);
            }

        } while(strcmp(buffer, "bye"));
    }

    /* Client connection Close */
    close(clientfd);
    /* Close the local socket */
    close(serverfd);
     
    return 0;


}

/* metodo para realizar validação sobre entrada do usuario */
void usage(char *exec)
{
    printf("%s <interface de rede> <porta>\n", exec);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
			usage(argv[0]);
    }
    else
    {
			/* obtendo interface de rede */
			input_ifname = argv[1];
			/* obtendo o mac do servidor */
			strcpy(mac_server, argv[2]);
   
			/* rodando o servidor para iniciar o game */
			servidor();
    }
    return 0;
}
