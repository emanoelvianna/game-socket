#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/in.h>
/* utilizando os utilitarios */
#include "estrutura.h"

#define ETHERTYPE_LEN 2
#define MAC_ADDR_LEN 6
#define BUFFER_LEN 1518 //TODO: rever o tamanho do buffer!

typedef unsigned char MacAddress[MAC_ADDR_LEN];
extern int errno;

/* método para mostrar quais são os parametros esperados */
void usage(char *exec)
{
	printf("%s <IP origem> <MAC origem> <IP destino> <MAC destino> <Porta>\n", exec);
}

/* imprime informações para comecar o jogo */
void informacoesIniciais()
{
}

int main(int argc, char *argv[])
{
	if (argc < 6)
	{
		usage(argv[0]);
	}
	else
	{
		struct estrutura_pacote pacote;
		unsigned char buffer[BUFFER_LEN];

		printf("MAC destino: %s\n", argv[2]);

		/** montando o pacote
		
		strcpy(source_protocol_address, argv[1]);
		printf("IP origem: %s\n", source_protocol_address);
		strcpy(source_hardware_address, argv[2]);
		printf("MAC origem: %s\n", source_hardware_address);

		strcpy(target_protocol_address, argv[3]);
		printf("IP destino: %s\n", target_protocol_address);
		strcpy(target_hardware_address, argv[4]);
		printf("MAC destino: %s\n", target_hardware_address);

		strcpy(pacote.source_port, argv[4]);
		printf("Porta: %04x\n", ntohs(pacote.source_port));

		**/

		/* configuracoes para o socket */
		int sockFd = 0, retValue = 0;
		struct sockaddr_ll destAddr;
		char dummyBuf[50];
		short int etherTypeT = htons(0x8200);

		/* Configura MAC Origem e Destino */
		MacAddress localMac = {0x00, 0x0B, 0xCD, 0xA8, 0x6D, 0x91};
		MacAddress destMac = {0x00, 0x17, 0x9A, 0xB3, 0x9E, 0x16};

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
		memcpy(&(destAddr.sll_addr), destMac, MAC_ADDR_LEN);

		/* Cabecalho Ethernet */
		memcpy(buffer, destMac, MAC_ADDR_LEN);
		memcpy((buffer + MAC_ADDR_LEN), localMac, MAC_ADDR_LEN);
		memcpy((buffer + (2 * MAC_ADDR_LEN)), &(etherTypeT), sizeof(etherTypeT));

		/* Add some data */
		memcpy((buffer + ETHERTYPE_LEN + (2 * MAC_ADDR_LEN)), dummyBuf, 50);

		while (1)
		{
			/* Envia pacotes de 64 bytes */
			if ((retValue = sendto(sockFd, buffer, 64, 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0)
			{
				printf("ERROR! sendto() \n");
				exit(1);
			}
			//printf("Send success (%d).\n", retValue);
		}
	}
}
