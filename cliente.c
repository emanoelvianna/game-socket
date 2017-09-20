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
unsigned char mac_server[ETHERNET_ADDR_LEN];
extern int errno;
unsigned char aux = 0;
unsigned char jogada_x;
unsigned char jogada_y;

/* metodo auxiliar para buscar o mac do server */
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

void usage(char *exec)
{
	printf("%s <interface de rede local> <endereço mac servidor> <porta>\n", exec);
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		usage(argv[0]);
	}
	else
	{
		estrutura_pacote pacote;
		unsigned char buffer[BUFFER_LEN];

		/* obtendo interface de rede */
		input_ifname = argv[1];
		/* obtendo o mac local */
		getMac();

		/* configuracoes para o socket */
		int sockFd = 0, retValue = 0;
		struct sockaddr_ll destAddr;
		char dummyBuf[50];
		short int etherTypeT = htons(0x800);

		/* montando o pacote IPv4 - inicio */
		strcpy(pacote.source_ethernet_address, mac_local);
		printf("Mac origem: %02x%02x%02x%02x%02x%02x\n",
			   pacote.source_ethernet_address[0],
			   pacote.source_ethernet_address[1],
			   pacote.source_ethernet_address[2],
			   pacote.source_ethernet_address[3],
			   pacote.source_ethernet_address[4],
			   pacote.source_ethernet_address[5]);

		strcpy(pacote.target_ethernet_address, argv[2]);
		printf("Mac destion: %02x%02x%02x%02x%02x%02x\n",
			   pacote.target_ethernet_address[0],
			   pacote.target_ethernet_address[1],
			   pacote.target_ethernet_address[2],
			   pacote.target_ethernet_address[3],
			   pacote.target_ethernet_address[4],
			   pacote.target_ethernet_address[5]);

		pacote.ethernet_type = ETH_P_IP;
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

		/* Cabecalho Ethernet */
		memcpy(buffer, pacote.target_ethernet_address, ETHERNET_ADDR_LEN);
		memcpy((buffer + ETHERNET_ADDR_LEN), mac_local, ETHERNET_ADDR_LEN);
		memcpy((buffer + (2 * ETHERNET_ADDR_LEN)), &(etherTypeT), sizeof(etherTypeT));

		/* Add some data */
		memcpy((buffer + ETHERTYPE_LEN + (2 * ETHERNET_ADDR_LEN)), dummyBuf, 50);

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
}