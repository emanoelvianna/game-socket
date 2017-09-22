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

bool jogada_valida(unsigned char linha, unsigned char coluna)
{
	if(linha>=0 && linha<N_LINHAS)
	{
		if(coluna>=0 && coluna<N_COLUNAS)
		{
			if(matriz[linha][coluna] == '.')
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

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

/* metodo para o servidor */
int servidor()
{
	// definicao dos jogadores
	bool jogador1 = false;
	bool jogador2 = false;

	char ifname[IFNAMSIZ];
	strcpy(ifname, input_ifname);

	//pacotes que serao enviados para os jogadores
	estrutura_pacote pacote_para_jogador1;
	estrutura_pacote pacote_para_jogador2;

	//AGUARDA CONECTAR JOGADORES
	printf(" Esperando jogadores ... \n");
	//CONECTANDO JOGADOR1!!!
	bool preciso_conectar_jogador = true;
	while(preciso_conectar_jogador )
	{
		//recebe pacote (so segue em frente se recebeu)
		estrutura_pacote pacote = recebe_pacote(0, PORTA_SERVIDOR);

		jogador1 = true;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//MONTANDO BASE DO PACOTE DE ENVIO PARA O JOGADOR1    
		//MONTANDO PACOTE ETHERNET
		memcpy(mac_jogador1, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
		memcpy(&pacote_para_jogador1.target_ethernet_address, pacote.source_ethernet_address,ETHERNET_ADDR_LEN);
		memcpy(pacote_para_jogador1.source_ethernet_address, mac_server,ETHERNET_ADDR_LEN);
		pacote_para_jogador1.ethernet_type = ETHERTYPE;
		//MONTANDO PACOTE IPV4
		pacote_para_jogador1.version = 5;
		pacote_para_jogador1.ihl = 4;
		pacote_para_jogador1.tos = 0;
		pacote_para_jogador1.tlen = sizeof(estrutura_pacote);
		pacote_para_jogador1.id = htonl(54321);
		pacote_para_jogador1.flags_offset = 0;
		pacote_para_jogador1.ttl = 255;
		pacote_para_jogador1.protocol = UDP_PROTOCOL;
		pacote_para_jogador1.checksumip = 0;
		//***
		//OS IPS DEVEM SER TRATADOS (PARAMETRO NO MAIN DO CLIENTE) TODO
		//char ip_source_para_jogador1[32];
		//strcpy(ip_source_para_jogador1, "192.168.1.10");
		//pacote_para_jogador1.src = inet_addr(ip_source_para);
		//char ip_destination_para_jogador1[32];
		//strcpy(ip_destination_para_jogador1, "192.168.1.10");
		//pacote_para_jogador1.dst = inet_addr(ip_destination_para_jogador1);	
		//***			
		//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
		pacote_para_jogador1.checksumip = calcula_checksum(pacote_para_jogador1);
		printf("Checksum: %d \n", pacote_para_jogador1.checksumip);
		//MONTANDO PACOTE UDP
		pacote_para_jogador1.source_port = PORTA_SERVIDOR;
		porta_jogador1 = pacote.source_port;
		pacote_para_jogador1.destination_port = porta_jogador1;
		pacote_para_jogador1.size = SIZE_PACOTE_UDP;
		pacote_para_jogador1.checksumudp = 0;
		//MONTANDO OS DADOS (PAYLOAD) DO PACOTE   
		strcpy(pacote_para_jogador1.mensagem, "X");
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		envia_pacote(pacote_para_jogador1);  
		printf("Servidor: Jogador1 entrou na partida!\n");
		printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);  
		printf("MAC do pacote (copiado): %s\n", mac_jogador1);		
		verifica_check_sum(pacote);
		printf("\n"); 
		preciso_conectar_jogador = false;  
	}
	//CONECTANDO JOGADOR2!!!
	preciso_conectar_jogador = true;
	while(preciso_conectar_jogador )
	{
		//recebe pacote (so segue em frente se recebeu)
		estrutura_pacote pacote = recebe_pacote(0, PORTA_SERVIDOR);

		//verifica se o mac origem capturado e diferente do mac do jogador1
		if (pacote.source_port != porta_jogador1)
		{
			jogador2 = true;

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//MONTANDO BASE DO PACOTE DE ENVIO PARA O JOGADOR2    
			//MONTANDO PACOTE ETHERNET
			memcpy(mac_jogador2, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
			memcpy(&pacote_para_jogador2.target_ethernet_address, pacote.source_ethernet_address,ETHERNET_ADDR_LEN);
			memcpy(pacote_para_jogador2.source_ethernet_address, mac_server,ETHERNET_ADDR_LEN);
			pacote_para_jogador2.ethernet_type = ETHERTYPE;
			//MONTANDO PACOTE IPV4
			pacote_para_jogador2.version = 5;
			pacote_para_jogador2.ihl = 4;
			pacote_para_jogador2.tos = 0;
			pacote_para_jogador2.tlen = sizeof(estrutura_pacote);
			pacote_para_jogador2.id = htonl(54321);
			pacote_para_jogador2.flags_offset = 0;
			pacote_para_jogador2.ttl = 255;
			pacote_para_jogador2.protocol = UDP_PROTOCOL;
			pacote_para_jogador2.checksumip = 0;
			//***
			//OS IPS DEVEM SER TRATADOS (PARAMETRO NO MAIN DO CLIENTE) TODO
			//char ip_source_para_jogador2[32];
			//strcpy(ip_source_para_jogador2, "192.168.1.10");
			//pacote_para_jogador2.src = inet_addr(ip_source_para);
			//char ip_destination_para_jogador2[32];
			//strcpy(ip_destination_para_jogador2, "192.168.1.10");
			//pacote_para_jogador2.dst = inet_addr(ip_destination_para_jogador2);	
			//***			
			//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
			pacote_para_jogador2.checksumip = calcula_checksum(pacote_para_jogador2);
			printf("Checksum: %d \n", pacote_para_jogador2.checksumip);
			//MONTANDO PACOTE UDP
			pacote_para_jogador2.source_port = PORTA_SERVIDOR;
			porta_jogador2 = pacote.source_port;
			pacote_para_jogador2.destination_port = porta_jogador2;
			pacote_para_jogador2.size = SIZE_PACOTE_UDP;
			pacote_para_jogador2.checksumudp = 0;
			//MONTANDO OS DADOS (PAYLOAD) DO PACOTE   
			strcpy(pacote_para_jogador2.mensagem, "O");
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			envia_pacote(pacote_para_jogador2);  
			printf("Servidor: Jogador2 entrou na partida!\n");
			printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);  
			printf("MAC do pacote (copiado): %s\n", mac_jogador2);		
			verifica_check_sum(pacote);
			printf("\n"); 
			preciso_conectar_jogador = false;
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

	//close(fd);
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
