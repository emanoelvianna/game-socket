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
int jogadas = 0;
char mensagem_partida[100];

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

/* método para verificar jogada */
bool jogada_valida(unsigned char linha, unsigned char coluna)
{
	if (linha >= 0 && linha < N_LINHAS)
	{
		if (coluna >= 0 && coluna < N_COLUNAS)
		{
			if (matriz[linha][coluna] == '.')
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

/* metodo para verificar quando acabar a partida */
void atualizarPartida()
{
	estrutura_pacote pacote;
	/* verificar linhas */
	if (matriz[1][1] == 'X' && matriz[1][2] == 'X' && matriz[1][3] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[2][1] == 'X' && matriz[2][2] == 'X' && matriz[2][3] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[3][1] == 'X' && matriz[3][2] == 'X' && matriz[3][3] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[1][1] == 'O' && matriz[1][2] == 'O' && matriz[1][3] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[2][1] == 'O' && matriz[2][2] == 'O' && matriz[2][3] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	}
	else if (matriz[3][1] == 'O' && matriz[3][2] == 'O' && matriz[3][3] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	}
	/* verificar colunas */
	else if (matriz[1][1] == 'X' && matriz[2][1] == 'X' && matriz[3][1] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[1][2] == 'X' && matriz[2][2] == 'X' && matriz[3][2] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[1][3] == 'X' && matriz[2][3] == 'X' && matriz[3][3] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[1][1] == 'O' && matriz[2][1] == 'O' && matriz[3][1] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	}
	else if (matriz[1][2] == 'O' && matriz[2][2] == 'O' && matriz[3][2] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	}
	else if (matriz[1][3] == 'O' && matriz[2][3] == 'O' && matriz[3][3] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	} /* verificar diagonais */
	else if (matriz[1][1] == 'X' && matriz[2][2] == 'X' && matriz[3][3] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[1][3] == 'X' && matriz[2][2] == 'X' && matriz[3][1] == 'X')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada X ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada X ganho!");
	}
	else if (matriz[1][1] == 'O' && matriz[2][2] == 'O' && matriz[3][3] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	}
	else if (matriz[1][3] == 'O' && matriz[2][2] == 'O' && matriz[3][1] == 'O')
	{
		pacote.acabou = true;
		printf("Servidor: partida finalizada O ganho!\n");
		/* envia mensagem ao cliente com atualização da partida */
		sprintf(mensagem_partida, "Servidor: partida finalizada O ganho!");
	}
	else
	{
		/* partida chegou ao final empatada */
		if (jogadas == 9)
		{
			isRunning = false;
			pacote.acabou = true;
			printf("Servidor: partida empatada!\n");
			/* envia mensagem ao cliente com atualização da partida */
			sprintf(mensagem_partida, "Servidor: partida empatada!");
		}
	}
}

/* metodo para o servidor */
int servidor()
{
	int fd;
	unsigned char buffer[BUFFER_SIZE];
	struct ifreq ifr;
	char ifname[IFNAMSIZ];
	int igual = 0;

	/* definição dos jogadores  */
	bool jogador1 = false;
	bool jogador2 = false;

	//pacotes que serao enviados para os jogadores
	estrutura_pacote pacote_para_jogador1;
	estrutura_pacote pacote_para_jogador2;

	strcpy(ifname, input_ifname);

	/* Cria um descritor de socket do tipo RAW */
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0)
	{
		fprintf(stderr, "Erro ao tentar criar o socket!");
		exit(1);
	}

	/* Obtem o indice da interface de rede */
	strcpy(ifr.ifr_name, ifname);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	/* Obtem as flags da interface */
	if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	/* Coloca a interface em modo promiscuo */
	ifr.ifr_flags |= IFF_PROMISC;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		perror("ioctl");
		exit(1);
	}

	/* aguarda entrada de jogadores */
	printf("--------------------------------------------\n");
	printf("----- Jogo da velha utilizando sockets -----\n");
	printf("--------------------------------------------\n");
	printf(" \nServidor: Partida esperando jogadores...\n");
	/* adicionando jogador 1 */
	bool preciso_conectar_jogador = true;
	while (preciso_conectar_jogador)
	{
		estrutura_pacote pacote;
		recv(fd, (char *)&pacote, sizeof(pacote), 0x0);
		//O SERVIDOR NAO PROCESSA INFORMACAO CASO ELE TENHA MANDADO (PACOTE.SOURCE = PORTA_SERVIDOR)
		if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.source_port != PORTA_SERVIDOR)
		{
			jogador1 = true;
			//MONTANDO BASE DO PACOTE DE ENVIO PARA O JOGADOR1
			//MONTANDO PACOTE ETHERNET
			memcpy(mac_jogador1, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
			memcpy(&pacote_para_jogador1.target_ethernet_address, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
			memcpy(pacote_para_jogador1.source_ethernet_address, mac_server, ETHERNET_ADDR_LEN);
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
			envia_pacote(pacote_para_jogador1);
			printf("Servidor: Jogador1 entrou na partida!\n");
			printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);
			printf("MAC do pacote (copiado): %s\n", mac_jogador1);
			verifica_check_sum(pacote);
			printf("\n");
			preciso_conectar_jogador = false;
		}
	}
	/* adicionando jogador 2 */
	preciso_conectar_jogador = true;
	while (preciso_conectar_jogador)
	{
		estrutura_pacote pacote;
		recv(fd, (char *)&pacote, sizeof(pacote), 0x0);
		//O SERVIDOR NAO PROCESSA INFORMACAO CASO ELE TENHA MANDADO (PACOTE.SOURCE = PORTA_SERVIDOR)
		if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.source_port != PORTA_SERVIDOR)
		{
			//verifica se o mac origem capturado e diferente do mac do jogador1
			if (pacote.source_port != porta_jogador1)
			{
				memcpy(mac_jogador2, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
				memcpy(&pacote_para_jogador2.target_ethernet_address, pacote.source_ethernet_address, ETHERNET_ADDR_LEN);
				memcpy(pacote_para_jogador2.source_ethernet_address, mac_server, ETHERNET_ADDR_LEN);
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
				envia_pacote(pacote_para_jogador2);
				printf("Servidor: Jogador2 entrou na partida!\n");
				printf("MAC do pacote (original): %s\n", pacote.source_ethernet_address);
				printf("MAC do pacote (copiado): %s\n", mac_jogador2);
				verifica_check_sum(pacote);
				printf("\n");
				preciso_conectar_jogador = false;
			}
		}
	}

	/* inicia a matriz para começar a partida */
	iniciarMatriz();
	/* laço do jogo */
	printf("\nServidor: Iniciando a partida\n\n");

	int jogadorQueDeveJogar = 1;
	while (isRunning)
	{
		/* verificar jogadas */
		estrutura_pacote pacote;
		/* se ocorreu alguma jogada atualiza matriz e escreve em tela */
		recv(fd, (char *)&pacote, sizeof(pacote), 0x0);
		if (pacote.ethernet_type == ETHERTYPE && pacote.protocol == UDP_PROTOCOL && pacote.source_port != PORTA_SERVIDOR)
		{
			/* verificar qual jogador deve realizar a jogada */
			if (jogadorQueDeveJogar == 1)
			{
				if (pacote.source_ethernet_address == mac_jogador1)
				{
					if (jogada_valida(pacote.linha, pacote.coluna))
					{
						adicionarJogada(pacote.linha, pacote.coluna, 'X');
						atualizarPartida();
						desenhaMatriz();
						/* controla a quantidade de jogadas */
						jogadas = jogadas + 1;
						jogadorQueDeveJogar = 2;
					}
					else
					{
						//TODO: mensagem de erro de jogada invalida!
					}
				}
			}
			else if (jogadorQueDeveJogar == 2)
			{
				if (pacote.source_ethernet_address == mac_jogador2, 'O')
				{
					if (jogada_valida(pacote.linha, pacote.coluna))
					{
						adicionarJogada(pacote.linha, pacote.coluna, 'O');
						atualizarPartida();
						desenhaMatriz();
						/* controla a quantidade de jogadas */
						jogadas = jogadas + 1;
						jogadorQueDeveJogar = 1;
					}
					else
					{
						//TODO: mensagem de erro de jogada invalida!
					}
				}
			}
		};
	}
	printf("Servidor: O jogo acabou! =]\n");

	close(fd);
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