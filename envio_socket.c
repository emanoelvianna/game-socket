#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
#include "envio_socket.h"

#define ETHERTYPE_LEN 2
#define MAC_ADDR_LEN 6
#define BUFFER_LEN 1518 //TODO: rever o tamanho do buffer!

typedef unsigned char MacAddress[MAC_ADDR_LEN];
extern int errno;
unsigned char aux = 0;
unsigned char jogada;

void usage(char* exec) 
{
	printf("%s <Ip> <Porta>\n", exec);
}

int main(int argc, char * argv[])
{
	if (argc < 3) 
	{
		usage(argv[0]);
	} 
	else 
	{
		struct estrutura_pacote pacote;
		unsigned char buffer[BUFFER_LEN];

		/* configuracoes para o socket */		   	
		int sockFd = 0, retValue = 0; 
		struct sockaddr_ll destAddr;
		char dummyBuf[50];
		short int etherTypeT = htons(0x800);

        /* Configura MAC Origem e Destino */
        MacAddress localMac = {0x50, 0xB7, 0xC3, 0x42, 0x6F, 0xA1};
		MacAddress destMac = {0x50, 0xB7, 0xC3, 0x42, 0x6F, 0xA1};
		
		/* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
		if((sockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) 
		{
			printf("Erro na criacao do socket.\n");
			exit(1);
		}

		/* Identicacao de qual maquina (MAC) deve receber a mensagem enviada no socket. */
		destAddr.sll_family = htons(PF_PACKET);
		destAddr.sll_protocol = htons(ETH_P_ALL);
		destAddr.sll_halen = 6;
		destAddr.sll_ifindex = 2;  /* indice da interface pela qual os pacotes serao enviados. Eh necess�rio conferir este valor. */
		memcpy(&(destAddr.sll_addr), destMac, MAC_ADDR_LEN);

		/* Cabecalho Ethernet */
		memcpy(buffer, destMac, MAC_ADDR_LEN);
		memcpy((buffer+MAC_ADDR_LEN), localMac, MAC_ADDR_LEN);
		memcpy((buffer+(2*MAC_ADDR_LEN)), &(etherTypeT), sizeof(etherTypeT));

		/* Add some data */
		memcpy((buffer+ETHERTYPE_LEN+(2*MAC_ADDR_LEN)), dummyBuf, 50);

        //CONTINUACAO DOS DADOS NO PACOTE
        buffer[23] = 17;
        buffer[42] = true;
        jogada = '9';
        buffer[43] = jogada;

		//while(1) 
		//{
			/* Envia pacotes de 64 bytes */
			//if((retValue = sendto(sockFd, buffer, 64, 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0) 
            if((retValue = sendto(sockFd, buffer, 64, 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0) 			    
            {
				printf("ERROR! sendto() \n");
				exit(1);
			}
            else
            {
                printf("Send success (%d).\n", retValue);
            }
			
		//}
        
	}
}
