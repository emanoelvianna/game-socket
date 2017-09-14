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
#define BUFFSIZE 1518
/* definicao do tamanho da matriz */
#define N_LINHAS 3
#define N_COLUNAS 3

char matriz [N_LINHAS][N_COLUNAS];
char *input_ifname;
unsigned char *mac_server;
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

    mac_server = (unsigned char *)ifr.ifr_hwaddr.sa_data;
     
    return 0;
}

/* metodo para o servidor */
int servidor() 
{
	int fd;
	unsigned char buffer[BUFFER_SIZE];
	struct ifreq ifr;
	char ifname[IFNAMSIZ];

	strcpy(ifname, input_ifname);

	/* Cria um descritor de socket do tipo RAW */
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0)
	{
		fprintf(stderr, "Erro ao tentar criar o socket!");
		exit(1);
	}

	/* Obtem o indice da interface de rede */
	strcpy(ifr.ifr_name, ifname);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	/* Obtem as flags da interface */
	if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	/* Coloca a interface em modo promiscuo */
	ifr.ifr_flags |= IFF_PROMISC;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	getMacServer();

	if(jogador1 == false && jogador2 == false){
		printf(" Esperando jogadores ... \n");
	}

	while(1) {

		struct estrutura_pacote pacote;

		/* Recebe pacotes */
		if (recv(fd, (char *)&buffer, BUFFER_SIZE, 0) < 0)
		{
			perror("recv");
			close(fd);
			exit(1);
		}

		/* Copia o conteudo dos protocolos Ethernet, IPv4 e UDP */
		memcpy(&pacote, buffer, sizeof(buffer));
		pacote.ethernet_type = ntohs(pacote.ethernet_type);

		//TODO: verificar se o pacote trafegado é de um jogador
		//TODO: verificar se é um pacote udp
		//TODO: capturar jogada e adicionar a matriz

		/* verifica se é um pacote IPv4 */
		if (pacote.ethernet_type == ETHERTYPE)
		{
				/* verifica se o mac destino é o server */
				if(memcpy(&pacote.target_ethernet_address, mac_server, sizeof(mac_server))  == 0){
					/* adicionando jogadores a partida */
					if(jogador1 == false) {
						jogador1 = true;	
					} else if(jogador2 == false) {
						jogador2 = true;
					}
				}
		}

	}
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
			input_ifname = argv[1];
			servidor();
    }
    return 0;
}