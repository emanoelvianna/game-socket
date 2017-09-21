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

char matriz[N_LINHAS][N_COLUNAS];
char *input_ifname;
unsigned char mac_server[ETHERNET_ADDR_LEN];

/* metodo para incializar a matriz vazia */
void iniciarMatriz()
{
	int i, j;
	for (i = 0; i < N_LINHAS; i++)
	{
		for (j = 0; j < N_COLUNAS; j++)
		{
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
	for (i = 0; i < N_LINHAS; i++)
	{
		for (j = 0; j < N_COLUNAS; j++)
		{
			if (linha == i && coluna == j)
				matriz[i][j] = peca;
		}
	}
}

/* metodo auxiliar para buscar o endereço mac da maquina */
int getMac()
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, input_ifname, IFNAMSIZ - 1);
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	strcpy(mac_server, ifr.ifr_hwaddr.sa_data);

	return 0;
}

/* metodo para envio de mensagem para cliente */
void enviarMensagem(unsigned char *mac_destino)
{
	int sockFd = 0, retValue = 0;
	char buffer[BUFFER_SIZE], dummyBuf[50];
	struct sockaddr_ll destAddr;
	short int etherTypeT = htons(0x8200);

	/* htons: converte um short (2-byte) integer para standard network byte order. */
	if ((sockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		printf("Erro na criacao do socket para enviar mensagem.\n");
		exit(1);
	}

	/* Identicacao de qual maquina (MAC) deve receber a mensagem enviada no socket. */
	destAddr.sll_family = htons(PF_PACKET);
	destAddr.sll_protocol = htons(ETH_P_ALL);
	destAddr.sll_halen = 6;
	destAddr.sll_ifindex = 2; /* indice da interface pela qual os pacotes serao enviados. Eh necess�rio conferir este valor. */
	memcpy(&(destAddr.sll_addr), mac_destino, ETHERNET_ADDR_LEN);

	/* Cabecalho Ethernet */
	memcpy(buffer, mac_destino, ETHERNET_ADDR_LEN);
	memcpy((buffer + ETHERNET_ADDR_LEN), mac_server, ETHERNET_ADDR_LEN);
	memcpy((buffer + (2 * ETHERNET_ADDR_LEN)), &(etherTypeT), sizeof(etherTypeT));

	/* Add some data */
	memcpy((buffer + ETHERTYPE_LEN + (2 * ETHERNET_ADDR_LEN)), dummyBuf, 50);

	/* Envia pacotes de 64 bytes */
	if ((retValue = sendto(sockFd, buffer, 64, 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0)
	{
		printf("ERROR! sendto() \n");
		exit(1);
	}
	printf("Send success (%d).\n", retValue);
}

/* metodo para o servidor */
int servidor()
{
	int fd;
	unsigned char buffer[BUFFER_SIZE];
	struct ifreq ifr;
	char ifname[IFNAMSIZ];
	int igual = 0;

	/* definição dos jogadores  */
	bool jogador1 = false;
	bool jogador2 = false;

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

	if (jogador1 == false && jogador2 == false)
	{
		printf(" Esperando jogadores ... \n");
	}

	while (1)
	{
		estrutura_pacote pacote;

		/* Recebe pacotes */
		recv(fd, (char *)&pacote, sizeof(pacote), 0x0);

		/* verifica se é um pacote IPv4 */
		if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL)
		{
			/* verifica se o mac destino é o server */
			if (strcmp(pacote.target_ethernet_address, mac_server) != 0)
			{
				/* adicionando jogadores a partida */
				if (jogador1 == false)
				{
					jogador1 = true;
					printf("Servidor: Jogador 1 entrou na partida!\n");
					printf("MAC do jogador 1: %s\n", pacote.source_ethernet_address);
					/* enviando mensagem ao jogador*/
					//enviarMensagem(pacote.source_ethernet_address);
				}
				else if (jogador2 == false)
				{
					jogador2 = true;
					printf("Servidor: Jogador 2 entrou na partida!\n");
					printf("MAC do jogador 2: %s\n", pacote.source_ethernet_address);
					/* enviando mensagem ao jogador*/
					//enviarMensagem(pacote.source_ethernet_address);
				}
				else
				{
					printf("Servidor: Quantidade de jogadores já ultrapassada!\n");
				}
			}
		}
	}

	close(fd);
}

/* metodo para realizar validação sobre entrada do usuario */
void usage(char *exec)
{
	printf("%s <interface de rede>\n", exec);
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
		getMac();
		/* rodando o servidor para iniciar o game */
		servidor();
	}
	return 0;
}
