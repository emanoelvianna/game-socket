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
#include <time.h>
/* utilizando os utilitarios */
#include "estrutura.h"

char matriz[N_LINHAS][N_COLUNAS];
char *input_ifname;
unsigned char mac_server[ETHERNET_ADDR_LEN];
unsigned char mac_jogador1[ETHERNET_ADDR_LEN];
unsigned char mac_jogador2[ETHERNET_ADDR_LEN];
unsigned short porta_jogador1;
unsigned short porta_jogador2;
bool isRunning = true;

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
void enviarMensagem(unsigned char *mac_destino, unsigned char simbolo_aux, unsigned short porta_destino_aux)
{
	estrutura_pacote pacote;
	unsigned char buffer[BUFFER_SIZE];

	/* configuracoes para o socket */
	int sockFd = 0, retValue = 0;
	struct sockaddr_ll destAddr;
	char dummyBuf[50];
	short int etherTypeT = htons(0x800);

	/* montando o pacote Ethernet - inicio */
	memcpy(pacote.target_ethernet_address, mac_destino, ETHERNET_ADDR_LEN);
	memcpy(pacote.source_ethernet_address, mac_server, ETHERNET_ADDR_LEN);
	pacote.ethernet_type = ETH_P_IP;
	/* montando o pacote Ethernet - fim */

	/* montando o pacote IPv4 - inicio */
	pacote.version = 5;
	pacote.ihl = 4;
	pacote.tos = 0;
	pacote.tlen = sizeof(estrutura_pacote);
	pacote.id = htonl(54321);
	pacote.flags_offset = 0;
	pacote.ttl = 255;
	pacote.protocol = UDP_PROTOCOL;
	pacote.checksumip = 0;
	char ip_source[32];
	strcpy(ip_source, "192.168.1.10");
	pacote.src = inet_addr(ip_source);
	char ip_destination[32];
	strcpy(ip_destination, "192.168.1.10");
	pacote.dst = inet_addr(ip_destination);
	pacote.checksumip = calcula_checksum(pacote);
	printf("Checksum: %d \n", pacote.checksumip);
	/* motando pacote UDP */
	pacote.source_port = PORTA_SERVIDOR;
	pacote.destination_port = porta_destino_aux;
	pacote.size = SIZE_PACOTE_UDP;
	pacote.checksumudp = 0;
	/* montando pacote de dados */
	strcpy(pacote.mensagem, "voce acabou de entrar no jogo! =]");
	pacote.meu_simbolo = simbolo_aux;
	pacote.NAO_DEVO_SER_LIDO_PELO_SERVIDOR = true;

	/* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
	if ((sockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		printf("Erro na criacao do socket.\n");
		exit(1);
	}

	/* Identicacao de qual maquina (MAC) deve receber a mensagem enviada no socket. */
	destAddr.sll_family = htons(PF_PACKET);
	destAddr.sll_protocol = htons(ETH_P_ALL);
	destAddr.sll_halen = 6;
	destAddr.sll_ifindex = 2; /* indice da interface pela qual os pacotes serao enviados. Eh necess�rio conferir este valor. */
	memcpy(&(destAddr.sll_addr), pacote.target_ethernet_address, ETHERNET_ADDR_LEN);

	/* Add some data */
	//memcpy((buffer + ETHERTYPE_LEN + (2 * ETHERNET_ADDR_LEN)), dummyBuf, 50);
	if ((retValue = sendto(sockFd, &pacote, sizeof(pacote), 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0)
	{
		printf("ERROR! sendto() \n");
		exit(1);
	}
	else
	{
		printf("Send success (%d).\n", retValue);
	}
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
	bool jogador3 = false;

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

	/* aguarda entrada de jogadores */
	printf(" Esperando jogadores ... \n");
	/* adicionando jogador 1 */
	bool preciso_conectar_jogador = true;
	while (preciso_conectar_jogador)
	{
		estrutura_pacote pacote;
		recv(fd, (char *)&pacote, sizeof(pacote), 0x0);
		//O SERVIDOR NAO PROCESSA INFORMACAO CASO ELE TENHA MANDADO (PACOTE.SOURCE = PORTA_SERVIDOR)
		if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.source_port != PORTA_SERVIDOR)
		{

			jogador1 = true;
			memcpy(mac_jogador1, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
			porta_jogador1 = pacote.source_port;
			enviarMensagem(mac_jogador1, 'X', pacote.source_port);
			printf("Servidor: Jogador1 entrou na partida!\n");
			printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);
			printf("MAC do pacote (copiado): %s\n", mac_jogador1);
			verifica_check_sum(pacote);
			printf("\n");
			preciso_conectar_jogador = false;
		}
	}
	//CONECTANDO JOGADOR2!!!
	preciso_conectar_jogador = true;
	while (preciso_conectar_jogador)
	{
		estrutura_pacote pacote;
		recv(fd, (char *)&pacote, sizeof(pacote), 0x0);
		//O SERVIDOR NAO PROCESSA INFORMACAO CASO ELE TENHA MANDADO (PACOTE.SOURCE = PORTA_SERVIDOR)
		if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.source_port != PORTA_SERVIDOR)
		{
			//verifica se o mac origem capturado e diferente do mac do jogador1
			if (pacote.source_port != porta_jogador1)
			{
				jogador2 = true;
				memcpy(mac_jogador2, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
				porta_jogador2 = pacote.source_port;
				enviarMensagem(mac_jogador2, 'O', pacote.source_port);
				printf("Servidor: Jogador2 entrou na partida!\n");
				printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);
				printf("MAC do pacote (copiado): %s\n", mac_jogador2);
				verifica_check_sum(pacote);
				printf("\n");
				preciso_conectar_jogador = false;
			}
		}
	}

	/* laço do jogo */
	printf("INICIANDO PARTIDA!!!\n");
	while (isRunning)
	{

		srand(time(NULL));
		int resultado = rand() % 5;
		if (resultado == 2)
		{
			isRunning = false;
		}
	}
	printf("O JOGO ACABOU! =]\n");

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