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

estrutura_pacote recebe_pacote(unsigned short porta_origem, unsigned short porta_destino);

char *input_ifname;
unsigned char mac_local[ETHERNET_ADDR_LEN];
unsigned char mac_servidor[ETHERNET_ADDR_LEN];
unsigned short porta_origem;
unsigned short porta_destino;
extern int errno;
int jogada_linha;
int jogada_coluna;
char matriz[N_LINHAS][N_COLUNAS];
unsigned char meu_simbolo;
char* ip_origem;
char* ip_destino;

void definir_jogada()
{
	int linha_aux;
	int coluna_aux;

	printf("faça sua jogada linha coluna:\n");
	scanf("%d %d", &linha_aux, &coluna_aux);
	//printf("\n");

	jogada_linha = linha_aux;
	jogada_coluna = coluna_aux;
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
	//pacotes que sera para o servidor
	estrutura_pacote pacote_para_servidor;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//MONTANDO BASE DO PACOTE DE ENVIO PARA O SERVIDOR servidor    
	//MONTANDO PACOTE ETHERNET
	strcpy(pacote_para_servidor.source_ethernet_address, mac_local);
	strcpy(pacote_para_servidor.target_ethernet_address, mac_servidor);
	pacote_para_servidor.ethernet_type = ETHERTYPE;
	//MONTANDO PACOTE IPV4
	pacote_para_servidor.version = 5;
	pacote_para_servidor.ihl = 4;
	pacote_para_servidor.tos = 0;
	pacote_para_servidor.tlen = sizeof(estrutura_pacote);
	pacote_para_servidor.id = htonl(54321);
	pacote_para_servidor.flags_offset = 0;
	pacote_para_servidor.ttl = 255;
	pacote_para_servidor.protocol = UDP_PROTOCOL;	
	//***
	//OS IPS DEVEM SER TRATADOS (PARAMETRO NO MAIN DO CLIENTE)
	char ip_source_para_servidor[32];
	strcpy(ip_source_para_servidor, ip_origem);
	pacote_para_servidor.src = inet_addr(ip_source_para_servidor);
	char ip_destination_para_servidor[32];
	strcpy(ip_destination_para_servidor, ip_destino);
	pacote_para_servidor.dst = inet_addr(ip_destination_para_servidor);	
	//***	
	//MONTANDO O PACOTE UDP
	pacote_para_servidor.source_port = porta_origem;
	pacote_para_servidor.destination_port = porta_destino;
	pacote_para_servidor.size = SIZE_PACOTE_UDP;
	pacote_para_servidor.checksumudp = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
	pacote_para_servidor.checksumip = 0;
	pacote_para_servidor.checksumip = calcula_checksum(pacote_para_servidor);
	//printf("Checksum: %d \n", pacote_para_servidor.checksumip);

	//envia pacote para o servidor (solicita conexao no jogo)
	envia_pacote(pacote_para_servidor);     

	//aguarda conexao no jogo
	estrutura_pacote pacote = recebe_pacote(porta_destino, porta_origem);
	verifica_check_sum(pacote); 
	meu_simbolo = pacote.mensagem[0];
	printf("Estou na partida, meu simbolo é: %c!\n", pacote.mensagem[0]);   

	//printf("\n");

	//LACO DO JOGO
	printf("\nIniciando a partida\n\n");
	bool preciso_fazer_jogada = true;
	while(preciso_fazer_jogada == true)
	{
		//espera receber pacote do servidor
		estrutura_pacote pacote_recebido = recebe_pacote(porta_destino, porta_origem);
		verifica_check_sum(pacote_recebido); 

		//imprime mensagem recebida do servidor
		printf("%s\n",pacote_recebido.mensagem);

		//verifica se o jogo acabou
		if(pacote_recebido.acabou == true)
		{            
			preciso_fazer_jogada = false;
			break;
		}
		//manda a jogada para o servidor
		else
		{
			//digita a jogada no terminal
			definir_jogada();

			//atualiza pacote de envio com a jogada definida
			pacote_para_servidor.jogada_linha = jogada_linha;
			pacote_para_servidor.jogada_coluna = jogada_coluna;

			//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
			pacote_para_servidor.checksumip = 0;
			pacote_para_servidor.checksumip = calcula_checksum(pacote_para_servidor);
			//printf("Checksum: %d \n", pacote_para_servidor.checksumip);

			//envia pacote com a jogada para o servidor
			envia_pacote(pacote_para_servidor); 
		}
	}
	//printf("\nO JOGO ACABOU! =]\n");	
}

/* metodo para realizar validação sobre entrada do usuario */
void usage(char *exec)
{
	printf("%s <interface_de_rede> <endereco_mac_servidor> <porta> <ip_origem> <ip_destino>\n", exec);
}

/* 
 * retorna um pacote de acordo com os parametros
 *
 * porta_origem = porta origem desejada do pacote [o argumento porta_origem deve ser 0 se não importa qual e a origem do pacote]
 */
estrutura_pacote recebe_pacote(unsigned short porta_origem, unsigned short porta_destino)
{
	int fd;
	unsigned char buffer[BUFFER_SIZE];
	struct ifreq ifr;
	char ifname[IFNAMSIZ];

	strcpy(ifname, input_ifname);

	//Cria um descritor de socket do tipo RAW
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0)
	{
		fprintf(stderr, "Erro ao tentar criar o socket!");
		exit(1);
	}

	//Obtem o indice da interface de rede
	strcpy(ifr.ifr_name, ifname);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	//Obtem as flags da interface
	if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	//Coloca a interface em modo promiscuo
	ifr.ifr_flags |= IFF_PROMISC;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	//se eu quero receber um pacote para mim, nao importando a origem (conectar jogador)
	if(porta_origem == 0)
	{
		estrutura_pacote pacote;
		while(true)
		{        
			recv(fd, (char *)&pacote, sizeof(pacote), 0x0);
			if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.source_port != pacote.destination_port && pacote.destination_port == porta_destino)
			{     
				return pacote;           	      
			}
		}
		return pacote;

	}
	//se eu quero receber um pacote para mim, sendo a origem especifica (jogador ja conectado)
	else
	{
		estrutura_pacote pacote;
		while(true)
		{        
			recv(fd, (char *)&pacote, sizeof(pacote), 0x0);

			if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.destination_port == porta_destino && pacote.source_port == porta_origem && pacote.source_port != pacote.destination_port)	        
			{     
				return pacote;           	      
			}
		}
		return pacote;
	}    
}

int main(int argc, char *argv[])
{
	if (argc < 6)
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
		/* obtendo a porta origem do cliente */
		porta_origem = atoi(argv[3]);
		porta_destino = PORTA_SERVIDOR;
		/* obtendo ips */
		ip_origem = argv[4];
		ip_destino = argv[5];
		cliente();
	}
}
