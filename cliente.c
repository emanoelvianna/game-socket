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
unsigned char jogada_linha;
unsigned char jogada_coluna;
char matriz[N_LINHAS][N_COLUNAS];
unsigned char meu_simbolo;

void definir_jogada()
{
    int linha_aux;
    int coluna_aux;

    printf("digite a jogada -> linha coluna:");
    scanf("%d %d", &linha_aux, &coluna_aux);
    printf("\n");

    jogada_linha = linha_aux + '0';
    jogada_coluna = coluna_aux + '0';
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
	pacote_para_servidor.checksumip = 0;
	//***
	//OS IPS DEVEM SER TRATADOS (PARAMETRO NO MAIN DO CLIENTE) TODO
	//char ip_source_para_servidor[32];
	//strcpy(ip_source_para_servidor, "192.168.1.10");
	//pacote_para_servidor.src = inet_addr(ip_source_para);
	//char ip_destination_para_servidor[32];
	//strcpy(ip_destination_para_servidor, "192.168.1.10");
	//pacote_para_servidor.dst = inet_addr(ip_destination_para_servidor);	
	//***			
	//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
	pacote_para_servidor.checksumip = calcula_checksum(pacote_para_servidor);
	printf("Checksum: %d \n", pacote_para_servidor.checksumip);
	//MONTANDO O PACOTE UDP
	pacote_para_servidor.source_port = porta_origem;
	pacote_para_servidor.destination_port = porta_destino;
	pacote_para_servidor.size = SIZE_PACOTE_UDP;
	pacote_para_servidor.checksumudp = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//envia pacote para o servidor (solicita conexao no jogo)
	envia_pacote(pacote_para_servidor);     

	//aguarda conexao no jogo
	estrutura_pacote pacote = recebe_pacote(porta_destino, porta_origem);
    meu_simbolo = pacote.mensagem[0];
	printf("FUI CONECTADO AO JOGO E MEU SIMBOLO EH: %c (confirmando: %c)!\n", pacote.mensagem[0], meu_simbolo);   
	verifica_check_sum(pacote);  
	printf("\n");

	//LACO DO JOGO
	printf("INICIANDO PARTIDA!!!\n");
    bool preciso_fazer_jogada = true;
	while(preciso_fazer_jogada == true)
	{
        //espera receber pacote do servidor
        estrutura_pacote pacote_recebido = recebe_pacote(porta_destino, porta_origem);

        //imprime mensagem recebida do servidor
        printf("%s\n",pacote_recebido.mensagem);

        //verifica se o jogo acabou
        if(pacote_recebido.o_jogo_acabou == true)
        {            
            preciso_fazer_jogada = false;
        }
        //manda a jogada para o servidor
        else
        {
            //digita a jogada no terminal
            definir_jogada();

            //atualiza pacote de envio com a jogada definida
            pacote_para_servidor.jogada_linha = jogada_linha;
            pacote_para_servidor.jogada_coluna = jogada_coluna;

            //envia pacote com a jogada para o servidor
	        envia_pacote(pacote_para_servidor); 
        }
	}
	printf("O JOGO ACABOU! =]\n");	
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
		/* obtendo interface de rede */
		input_ifname = argv[1];
		/* obtendo o mac local */
		getMac();
		/* obtendo o mac do servidor */
		strcpy(mac_servidor, argv[2]);
		/* obtendo a porta origem do cliente */
		porta_origem = atoi(argv[3]);
		porta_destino = PORTA_SERVIDOR;
		cliente();
	}
}
