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
unsigned short porta_origem;
unsigned short porta_destino;
extern int errno;
unsigned char jogada_x;
unsigned char jogada_y;
char matriz[N_LINHAS][N_COLUNAS];
bool isRunning = true;

/*
void jogada()
{
    int jogada_linha;
    int jogada_coluna;

    printf("jogar:[linha][coluna] ");
    scanf("%d %d", &jogada_linha, &jogada_coluna);

    while(! (jogada_valida(jogada_linha, jogada_coluna)) )
    {
        printf("jogada invalida! digite outra coordenada\n");
        printf("jogar:[linha][coluna] ");
        scanf("%d %d", &jogada_linha, &jogada_coluna);
        printf("jogada escolhida:[%d][%d]\n", jogada_linha, jogada_coluna);
    }
    printf("jogada valida!!!\n");
    //INT + '0' = CONVERTE INT PARA CHAR
    jogada_x = jogada_linha + '0';
    jogada_y = jogada_coluna + '0';
    printf("jogada escolhida:[%c][%c]\n", jogada_x, jogada_y);
}
*/

void enviar_jogada()
{
    //define jogada
    //jogada();

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

	//MONTANDO O PACOTE ETHERNET
	strcpy(pacote.source_ethernet_address, mac_local);
	strcpy(pacote.target_ethernet_address, mac_servidor);
	pacote.ethernet_type = ETH_P_IP;
	//MONTANDO O PACOTE IPV4
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
	//MONTANDO O PACOTE UDP
    pacote.source_port = porta_origem;
    pacote.destination_port = porta_destino;
    pacote.size = SIZE_PACOTE_UDP;
    pacote.checksumudp = 0;
    //MONTANDO DADOS DE ENVIO
    pacote.jogada_x = jogada_x;
	pacote.jogada_y = jogada_y;

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

	//MONTANDO O PACOTE ETHERNET
	strcpy(pacote.source_ethernet_address, mac_local);
	strcpy(pacote.target_ethernet_address, mac_servidor);
	pacote.ethernet_type = ETH_P_IP;
	//MONTANDO O PACOTE IPV4
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
	//MONTANDO O PACOTE UDP
    pacote.source_port = porta_origem;
    pacote.destination_port = porta_destino;
    pacote.size = SIZE_PACOTE_UDP;
    pacote.checksumudp = 0;

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

	if ((retValue = sendto(sockFd, &pacote, sizeof(pacote), 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0)
	{
		printf("ERROR! sendto() \n");
		exit(1);
	}
	else
	{
		printf("Send success (%d).\n", retValue);
	}

    //AGUARDA ENTRAR NO JOGO
    bool preciso_conectar = true;
    while(preciso_conectar)
    {
        estrutura_pacote pacote_recebido;
	    recv(sockFd, (char *)&pacote_recebido, sizeof(pacote), 0x0);
        if (pacote_recebido.ethernet_type == ETHERTYPE && pacote_recebido.protocol == UDP_PROTOCOL && pacote_recebido.destination_port == porta_origem)
        {
            
            printf("FUI CONECTADO AO JOGO E MEU SIMBOLO EH: %c!\n", pacote_recebido.mensagem[0]);   
            verifica_check_sum(pacote_recebido);  
            printf("\n");
            preciso_conectar = false;   
        }
    }    

    //LACO DO JOGO
    printf("INICIANDO PARTIDA!!!\n");
    while(isRunning)
    {



        srand(time(NULL));
	    int resultado = rand() % 5;
        if(resultado == 2){isRunning = false;}
    }
    printf("O JOGO ACABOU! =]\n");	

	close(sockFd);
}

/* metodo para realizar validação sobre entrada do usuario */
void usage(char *exec)
{
	printf("%s <interface de rede> <endereço mac servidor> <porta>\n", exec);
}

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		usage(argv[0]);
	}
	else
	{
        //iniciarMatriz();

		/* obtendo interface de rede */
		input_ifname = argv[1];
		/* obtendo o mac local */
		getMac();
		/* obtendo o mac do servidor */
		strcpy(mac_servidor, argv[2]);
        /* obtendo a porta origem do cliente */
        porta_origem = htons(atoi(argv[3]));
		cliente();
	}
}
