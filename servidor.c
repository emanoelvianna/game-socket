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
#include <unistd.h>
/* utilizando os utilitarios */
#include "estrutura.h"

estrutura_pacote recebe_pacote(unsigned short porta_origem, unsigned short porta_destino);

char matriz[N_LINHAS][N_COLUNAS];
char *input_ifname;
unsigned char mac_server[ETHERNET_ADDR_LEN];
unsigned char mac_jogador1[ETHERNET_ADDR_LEN];
unsigned char mac_jogador2[ETHERNET_ADDR_LEN];
unsigned short porta_jogador1;
unsigned short porta_jogador2;
bool isRunning = true;
int quem_venceu = EMPATE;
int jogadas = 0;
//pacotes que serao enviados para os jogadores
estrutura_pacote pacote_para_jogador1;
estrutura_pacote pacote_para_jogador2;
char* ip_servidor;

void finalizar_jogo()
{
	//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
	pacote_para_jogador1.checksumip = 0;
	pacote_para_jogador1.checksumip = calcula_checksum(pacote_para_jogador2);
	//printf("Checksum: %d \n", pacote_para_jogador1.checksumip);

	//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
	pacote_para_jogador2.checksumip = 0;
	pacote_para_jogador2.checksumip = calcula_checksum(pacote_para_jogador2);
	//printf("Checksum: %d \n", pacote_para_jogador2.checksumip);

	if(quem_venceu == JOGADOR1_VENCEU)
	{
		pacote_para_jogador1.acabou = true;
		pacote_para_jogador2.acabou = true;

		printf("\n\nServidor: partida finalizada [jogador1 -> X] ganhou!\n");

		strcpy(pacote_para_jogador1.mensagem, "\n\nEu ganhei!!! =]");
		strcpy(pacote_para_jogador2.mensagem, "\n\nEu perdi!!! =[");

		envia_pacote(pacote_para_jogador1);
		envia_pacote(pacote_para_jogador2);

	}
	else if(quem_venceu == JOGADOR2_VENCEU)
	{
		pacote_para_jogador1.acabou = true;
		pacote_para_jogador2.acabou = true;

		printf("\n\nServidor: partida finalizada [jogador2 -> O] ganhou!\n");

		strcpy(pacote_para_jogador1.mensagem, "\n\nEu perdi!!! =[");
		strcpy(pacote_para_jogador2.mensagem, "\n\nEu ganhei!!! =]");

		envia_pacote(pacote_para_jogador1);
		envia_pacote(pacote_para_jogador2);
	}
	else if(quem_venceu == EMPATE)
	{
		pacote_para_jogador1.acabou = true;
		pacote_para_jogador2.acabou = true;

		printf("\n\nServidor: partida empatada!\n");

		strcpy(pacote_para_jogador1.mensagem, "\n\nEu empatei!!! :S");
		strcpy(pacote_para_jogador2.mensagem, "\n\nEu empatei!!! :S");

		envia_pacote(pacote_para_jogador1);
		envia_pacote(pacote_para_jogador2);
	}
}

/* metodo para verificar quando acabar a partida */
void atualizarPartida()
{
	/* verificar linhas */
	if (matriz[0][0] == 'X' && matriz[0][1] == 'X' && matriz[0][2] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[1][0] == 'X' && matriz[1][1] == 'X' && matriz[1][2] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[2][0] == 'X' && matriz[2][1] == 'X' && matriz[2][2] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][0] == 'O' && matriz[0][1] == 'O' && matriz[0][2] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	else if (matriz[1][0] == 'O' && matriz[1][1] == 'O' && matriz[1][2] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	else if (matriz[2][0] == 'O' && matriz[2][1] == 'O' && matriz[2][2] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	/* verificar colunas */
	else if (matriz[0][0] == 'X' && matriz[1][0] == 'X' && matriz[2][0] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][1] == 'X' && matriz[1][1] == 'X' && matriz[2][1] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][2] == 'X' && matriz[1][2] == 'X' && matriz[2][2] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][0] == 'O' && matriz[1][0] == 'O' && matriz[2][0] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][1] == 'O' && matriz[1][1] == 'O' && matriz[2][1] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][2] == 'O' && matriz[1][2] == 'O' && matriz[2][2] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	} /* verificar diagonais */
	else if (matriz[0][0] == 'X' && matriz[1][1] == 'X' && matriz[2][2] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][2] == 'X' && matriz[1][1] == 'X' && matriz[2][0] == 'X')
	{
		quem_venceu = JOGADOR1_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][0] == 'O' && matriz[1][1] == 'O' && matriz[2][2] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	else if (matriz[0][2] == 'O' && matriz[1][1] == 'O' && matriz[2][0] == 'O')
	{
		quem_venceu = JOGADOR2_VENCEU;
		isRunning = false;
	}
	else
	{
		/* partida chegou ao final empatada */
		if (jogadas == 9)
		{
			quem_venceu = EMPATE;
			isRunning = false;
		}
	}
}

bool jogada_valida(int linha, int coluna)
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
	printf("   0   1   2   \n");
	printf(" 0 %c | %c | %c \n", matriz[0][0], matriz[0][1], matriz[0][2]);
	printf("  ---|---|--- \n ");
	printf("1 %c | %c | %c \n", matriz[1][0], matriz[1][1], matriz[1][2]);
	printf("  ---|---|--- \n ");
	printf("2 %c | %c | %c \n", matriz[2][0], matriz[2][1], matriz[2][2]);
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

	//AGUARDA CONECTAR JOGADORES
	printf("--------------------------------------------\n");
	printf("----- Jogo da velha utilizando sockets -----\n");
	printf("--------------------------------------------\n");
	printf(" \nServidor: Partida esperando jogadores...\n");
	//CONECTANDO JOGADOR1!!!
	bool preciso_conectar_jogador = true;
	while(preciso_conectar_jogador )
	{
		//recebe pacote (so segue em frente se recebeu)
		estrutura_pacote pacote = recebe_pacote(0, PORTA_SERVIDOR);
		verifica_check_sum(pacote); 

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
		//***
		//OS IPS DEVEM SER TRATADOS (PARAMETRO NO MAIN DO CLIENTE)
		char ip_source_para_jogador1[32];
		strcpy(ip_source_para_jogador1, ip_servidor);
		pacote_para_jogador1.src = inet_addr(ip_source_para_jogador1);
		pacote_para_jogador1.dst = pacote.src;//inet_addr(ip_destination_para_jogador1);	
		//***		
		//MONTANDO PACOTE UDP
		pacote_para_jogador1.source_port = PORTA_SERVIDOR;
		porta_jogador1 = pacote.source_port;
		pacote_para_jogador1.destination_port = pacote.source_port;
		pacote_para_jogador1.size = SIZE_PACOTE_UDP;
		pacote_para_jogador1.checksumudp = 0;
		//MONTANDO OS DADOS (PAYLOAD) DO PACOTE   
		pacote_para_jogador1.acabou = false;
		strcpy(pacote_para_jogador1.mensagem, "X");
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
		pacote_para_jogador1.checksumip = 0;
		pacote_para_jogador1.checksumip = calcula_checksum(pacote_para_jogador1);
		//printf("Checksum: %d \n", pacote_para_jogador1.checksumip);

		envia_pacote(pacote_para_jogador1);  

		printf("Servidor: Jogador1 entrou na partida!\n");
		//printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);  
		//printf("MAC do pacote (copiado): %s\n", mac_jogador1);		
		//verifica_check_sum(pacote);
		//printf("\n"); 
		preciso_conectar_jogador = false;  
	}
	//CONECTANDO JOGADOR2!!!
	preciso_conectar_jogador = true;
	while(preciso_conectar_jogador )
	{
		//recebe pacote (so segue em frente se recebeu)
		estrutura_pacote pacote = recebe_pacote(0, PORTA_SERVIDOR);
		verifica_check_sum(pacote); 

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
			//***
			//OS IPS DEVEM SER TRATADOS (PARAMETRO NO MAIN DO CLIENTE)
			char ip_source_para_jogador2[32];
			strcpy(ip_source_para_jogador2, ip_servidor);
			pacote_para_jogador2.src = inet_addr(ip_source_para_jogador2);
			pacote_para_jogador1.dst = pacote.src;	
			//***			
			//MONTANDO PACOTE UDP
			pacote_para_jogador2.source_port = PORTA_SERVIDOR;
			porta_jogador2 = pacote.source_port;
			pacote_para_jogador2.destination_port = porta_jogador2;
			pacote_para_jogador2.size = SIZE_PACOTE_UDP;
			pacote_para_jogador2.checksumudp = 0;
			//MONTANDO OS DADOS (PAYLOAD) DO PACOTE   
			pacote_para_jogador2.acabou = false;
			strcpy(pacote_para_jogador2.mensagem, "O");
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
			pacote_para_jogador2.checksumip = 0;
			pacote_para_jogador2.checksumip = calcula_checksum(pacote_para_jogador2);
			//printf("Checksum: %d \n", pacote_para_jogador2.checksumip);

			envia_pacote(pacote_para_jogador2);  

			printf("Servidor: Jogador2 entrou na partida!\n");
			//printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);  
			//printf("MAC do pacote (copiado): %s\n", mac_jogador2);		
			//verifica_check_sum(pacote);
			//printf("\n"); 
			preciso_conectar_jogador = false;
		}	
	}

	/* laço do jogo */
	printf("\nServidor: Iniciando a partida\n\n");
	/* inicia a matriz para começar a partida */
	printf("\n"); 
	iniciarMatriz();
	desenhaMatriz();

	//mensagem para os jogadores
	strcpy(pacote_para_jogador1.mensagem, "***********");
	strcpy(pacote_para_jogador2.mensagem, "***********");

	while(isRunning)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//aplica jogada do jogador1
		bool jogador1_precisa_jogar = true;
		//O LACO SO PARA QUANDO O JOGADOR MANDA UMA JOGADA VALIDA
		while(jogador1_precisa_jogar == true)
		{
			//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
			pacote_para_jogador1.checksumip = 0;
			pacote_para_jogador1.checksumip = calcula_checksum(pacote_para_jogador1);
			//printf("Checksum: %d \n", pacote_para_jogador2.checksumip);

			//envia pacote solicitando jogada ao jogador1
			envia_pacote(pacote_para_jogador1);

			//recebe pacote com jogada do jogador1
			estrutura_pacote pacote_recebido = recebe_pacote(porta_jogador1, PORTA_SERVIDOR);
			verifica_check_sum(pacote_recebido); 

			//verifica se jogada e valida
			bool jogada_permitida = jogada_valida(pacote_recebido.jogada_linha, pacote_recebido.jogada_coluna);

			//aplica jogada se e valida
			if(jogada_permitida == true)
			{
				jogadas = jogadas + 1;
				adicionarJogada(pacote_recebido.jogada_linha, pacote_recebido.jogada_coluna, 'X');
				strcpy(pacote_para_jogador1.mensagem, "Jogada valida! u.u");
				jogador1_precisa_jogar = false;
			}
			else
			{
				strcpy(pacote_para_jogador1.mensagem, "Jogada invalida! o.o");
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//verifica implicacoes em decorrencia da jogada
		desenhaMatriz();
		atualizarPartida();
		if(isRunning == false){break;}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//aplica jogada do jogador2
		bool jogador2_precisa_jogar = true;
		//O LACO SO PARA QUANDO O JOGADOR MANDA UMA JOGADA VALIDA
		while(jogador2_precisa_jogar == true)
		{
			//O CHECKSUM SEMPRE DEVE SER DEFINIDO ANTES DE ENVIAR UM PACOTE
			pacote_para_jogador2.checksumip = 0;
			pacote_para_jogador2.checksumip = calcula_checksum(pacote_para_jogador2);
			//printf("Checksum: %d \n", pacote_para_jogador2.checksumip);

			//envia pacote solicitando jogada ao jogador2
			envia_pacote(pacote_para_jogador2);

			//recebe pacote com jogada do jogador2
			estrutura_pacote pacote_recebido = recebe_pacote(porta_jogador2, PORTA_SERVIDOR);
			verifica_check_sum(pacote_recebido); 

			//verifica se jogada e valida
			bool jogada_permitida = jogada_valida(pacote_recebido.jogada_linha, pacote_recebido.jogada_coluna);

			//aplica jogada se e valida
			if(jogada_permitida == true)
			{
				jogadas = jogadas + 1;
				adicionarJogada(pacote_recebido.jogada_linha, pacote_recebido.jogada_coluna, 'O');
				strcpy(pacote_para_jogador2.mensagem, "Jogada valida! u.u");
				jogador2_precisa_jogar = false;
			}
			else
			{
				strcpy(pacote_para_jogador2.mensagem, "Jogada invalida! o.o");
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//verifica implicacoes em decorrencia da jogada
		desenhaMatriz();
		atualizarPartida();
		if(isRunning == false){break;}
	}

	//envia pacote de encerramento para os jogadores
	finalizar_jogo();

	//printf("\nServidor: O jogo acabou! =]\n");
}

/* metodo para realizar validação sobre entrada do usuario */
void usage(char *exec)
{
	printf("%s <interface_de_rede> <ip_servidor>\n", exec);
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
	if (argc < 3)
	{
		usage(argv[0]);
	}
	else
	{
		/* obtendo interface de rede */
		input_ifname = argv[1];
		/* obtendo o mac do servidor */
		getMac();
		/* obtendo ip do servidor */
		ip_servidor = argv[2];
		/* rodando o servidor para iniciar o game */
		servidor();
	}
	return 0;
}
