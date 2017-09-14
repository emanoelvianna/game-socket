#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#define BUFFSIZE 1518

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

unsigned char buff1[BUFFSIZE]; // buffer de recepcao
unsigned char buff_aux;
int position_choosed_by_player = 0;

int sockd;
int on;
struct ifreq ifr;

int main(int argc,char *argv[])
{
	/* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
	/* De um "man" para ver os parametros.*/
	/* htons: converte um short (2-byte) integer para standard network byte order. */
	if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) 
	{
		printf("Erro na criacao do socket.\n");
		exit(1);
	}

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
	strcpy(ifr.ifr_name, "enp2s0");
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
	{	
		//printf("erro no ioctl!");
	}
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

	// recepcao de pacotes
	while (1) 
	{
		recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);

		//trata apenas pacotes IP (tipo 0x0800) com UDP (0X11)
		if(buff1[12] == 8 && buff1[13] == 0 && buff1[23] == 17 && buff1[42] == true)
		{
			// impressao do conteudo - exemplo Endereco Destino e Endereco Origem
			printf("Destination MAC Address: %02x:%02x:%02x:%02x:%02x:%02x \n", buff1[0],buff1[1],buff1[2],buff1[3],buff1[4],buff1[5]);
			printf("Source MAC Address: %02x:%02x:%02x:%02x:%02x:%02x \n", buff1[6],buff1[7],buff1[8],buff1[9],buff1[10],buff1[11]);
			printf("Ether Type: %02x%02x \n", buff1[12], buff1[13]);
			printf("Version: %02x \n", buff1[14] >> 4);
			buff_aux = 0;
			buff_aux = buff1[14] << 4;
			buff_aux = buff_aux >> 4;
			printf("IHL: %02x \n", buff_aux);
			printf("Type of Service: %02x \n", buff1[15]);
			printf("Total Length: %02x%02x \n", buff1[16], buff1[17]);
			printf("Identification: %02x%02x \n", buff1[18], buff1[19]);
			printf("Flags + Fragment Offset: %02x%02x \n", buff1[20], buff1[21]);
			printf("Time to Live: %02x \n", buff1[22]);
			printf("Protocol: %02x \n", buff1[23]);
			printf("Header Checksum: %02x%02x \n", buff1[24], buff1[25]);
			printf("Source Address: %02x%02x%02x%02x \n", buff1[26], buff1[27], buff1[28], buff1[29]);
			printf("Destination Address: %02x%02x%02x%02x \n", buff1[30], buff1[31], buff1[32], buff1[33]);
			printf("Source Port: %02x%02x \n", buff1[34], buff1[35]);
			printf("Destination Port: %02x%02x \n", buff1[36], buff1[37]);
			printf("Length: %02x%02x \n", buff1[38], buff1[39]);
			printf("Checksum: %02x%02x \n", buff1[40], buff1[41]);
			buff_aux = buff1[42];
			printf("Datagram of the game: %s\n", buff_aux ? "true" : "false");
			buff_aux = buff1[43];
			position_choosed_by_player = buff_aux - '0';
			printf("Position choosed by player: %d \n", position_choosed_by_player);
			printf("\n");
		}                
	}
}
