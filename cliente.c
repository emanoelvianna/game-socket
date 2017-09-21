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

char *input_ifname;
unsigned char mac_local[ETHERNET_ADDR_LEN];
unsigned char mac_servidor[ETHERNET_ADDR_LEN];
extern int errno;
unsigned char jogada_x;
unsigned char jogada_y;

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

	strcpy(mac_local, ifr.ifr_hwaddr.sa_data);

	return 0;
}

/* metodo para o cliente */
void cliente()
{
	estrutura_pacote pacote;
	/* configuracoes para o socket */
	struct sockaddr_in server;
	int len = sizeof(server);
	int sockFd; 
	int retValue;
	struct sockaddr_ll destAddr;
	int slen;
	/* buffer de entrada da mensagem do servidor */
	char buffer_in[BUFFER_SIZE];

	/* montando o cabecalho Ethernet */
	strcpy(pacote.source_ethernet_address, mac_local);
	strcpy(pacote.target_ethernet_address, mac_servidor);
	pacote.ethernet_type = ETH_P_IP;
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
	srand(time(NULL));
	pacote.jogada_x = rand() % 3;
	pacote.jogada_y = rand() % 3;
	pacote.checksumip = calcula_checksum(pacote);
	printf("Checksum: %d \n", pacote.checksumip);
	/* montando o pacote IPv4 - fim */

	/* montando o pacote UDP - inicio */

	/* montando o pacote UDP - fim */

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

	/* conectando com o servidor */
	if (connect(sockfd, (struct sockaddr *)&server, len) == -1)
	{
		perror("Can't connect to server");
	}

	/* recebe a mensagem do servidor */
	if ((slen = recv(sockfd, buffer_in, BUFFER_SIZE, 0)) > 0)
	{
		buffer_in[slen + 1] = '\0';
		fprintf(stdout, "Server says: %s\n", buffer_in);
	}

	/* capturando mensagem do servidor */
	while (true)
	{
		/* Zeroing the buffers */
		memset(buffer_in, 0x0, BUFFER_SIZE);

		/* enviando mensagem ao servidor */
		if ((retValue = sendto(sockFd, &pacote, sizeof(pacote), 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0)
		{
			printf("ERROR! sendto() \n");
			exit(1);
		}

		/* recebe uma resposta do servidor */
		slen = recv(sockfd, buffer_in, BUFFER_SIZE, 0);
		printf("Servidor: %s\n", buffer_in);

		if (strcmp(buffer_in, "fim") == 0)
			break;
	}

	close(sockfd);
}

/* metodo para realizar validação sobre entrada do usuario */
void usage(char *exec)
{
	printf("%s <interface de rede> <endereço mac servidor>\n", exec);
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		usage(argv[0]);
	}
	else
	{
		/* obtendo interface de rede */
		input_ifname = argv[1];
		/* obtendo o mac local */
		getMac();
		/* obtendo o mac do servidor */
		strcpy(mac_servidor, argv[2]);
		cliente();
	}
}