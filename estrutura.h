#ifndef HEADER_MASTER
#define HEADER_MASTER
/* declaracao das constantes */
#define PORTA_SERVIDOR 4242 /* definindo a porta de conexão  */
#define BUFFER_SIZE 1500 /* definicao do tamanho do buffer */

/* definicao do tamanho da matriz */
#define N_LINHAS 3
#define N_COLUNAS 3

#define ETHERNET_ADDR_LEN 6 /* indicando o tamanho do mac endereco fisico */
#define IP_ADDR_LEN 4 /* indicando o tamanho do mac endereco logico */
#define ETHERTYPE_LEN 2
#define UDP_PROTOCOL 17
#define ETHERTYPE 0x0800 /* indicando que é do tipo IPv4 */	 
#define UDP_PACKAGE_SIZE 50 /* definicao do tamanho do dado UDP */

const char* INTERFACE_DE_REDE = "enp0s31f6";

/* tamanho dos dados do pacote ethernet + ip */
const int SIZE_PACOTE_IP = (5 * sizeof(unsigned char)) + (4 * sizeof(unsigned short)) + (2 * sizeof(unsigned int)) + (2 * ETHERNET_ADDR_LEN * sizeof(unsigned char)) + (1 * sizeof(unsigned short));

/* tamanho dos dados do pacote udp (DEFINIR CONSTANTE APENAS QUANDO FICAR DETERMINADA A ÁREA EXATA DE DADOS!!!) */
const int SIZE_PACOTE_UDP = 0;

/* declaracao das estruturas para os jogadores */
unsigned char jogador1_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_jogador1
unsigned char jogador1_protocol_address[IP_ADDR_LEN];       // endereco_logico_jogador1
unsigned short jogador1_porta_origem;

unsigned char jogador2_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_jogador2
unsigned char jogador2_protocol_address[IP_ADDR_LEN];       // endereco_logico_jogador2
unsigned short jogador2_porta_origem;

typedef struct
{
	/* Cabeçalho Ethernet */
	unsigned char target_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
	unsigned char source_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
	unsigned short ethernet_type;                             // tipo_protocolo_ethernet

	/* Pacote IPv4 */
	unsigned char version:4;     // versao (nibble)
	unsigned char ihl:4;         // tamanho_cabecalho (nibble)
	unsigned char tos;           // tipo_servico
	unsigned short tlen;         // comprimento_pacote
	unsigned short id;           // identificacao
	unsigned short flags;        // flags
	unsigned short flags_offset; // deslocamento
	unsigned char ttl;           // tempo_de_vida
	unsigned char protocol;      // identificacao_protocolo
	unsigned short checksumip;   // check sum
	unsigned int src;          // endereco_fisico_origem
	unsigned int dst;          // endereco_fisico_destino

	/* Pacote UDP */
	unsigned short source_port;      // porta_origem
	unsigned short destination_port; // porta_destino
	unsigned short size;             // tamanho_pacote
	unsigned short checksumudp;

	/* Dados */
	bool o_jogo_acabou;
	unsigned char jogada_linha;
	unsigned char jogada_coluna;
	unsigned char tabuleiro[3][3];  
	char mensagem[100];  
} estrutura_pacote;

unsigned short in_cksum(unsigned short *addr,int len)
{
	register int sum = 0;
	u_short answer = 0;
	register u_short *w = addr;
	register int nleft = len;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
	sum += (sum >> 16);                     /* add carry */
	answer = ~sum;                          /* truncate to 16 bits */
	return(answer);
}

unsigned short calcula_checksum(estrutura_pacote pacote_aux)
{
	char datagrama[BUFFER_SIZE];
	memset (datagrama, 0, BUFFER_SIZE);
	memcpy(datagrama, &pacote_aux, sizeof(BUFFER_SIZE));
	return (unsigned short) in_cksum( (unsigned short*) datagrama, SIZE_PACOTE_IP);
}

void verifica_check_sum(estrutura_pacote pacote_aux)
{
	unsigned short checksum_recalculated = calcula_checksum(pacote_aux);
	printf("Checksum (recebido=%d) (recalculado=%d) \n", pacote_aux.checksumip,checksum_recalculated);
	if(pacote_aux.checksumip == checksum_recalculated)
	{
		printf("Checksum correto! =]\n");
	}
	else
	{
		printf("Checksum incorreto! =[\n");
	}
}

bool envia_pacote_aux(estrutura_pacote pacote)
{
	unsigned char buffer[BUFFER_SIZE];

	//configuracoes para o socket
	int sockFd = 0, retValue = 0;
	struct sockaddr_ll destAddr;
	char dummyBuf[50];
	short int etherTypeT = htons(0x800);		

	//Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet
	if ((sockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		//ERRO NA CRIACAO DO SOCKET
		return false;
	}

	//Identicacao de qual maquina (MAC) deve receber a mensagem enviada no socket.
	destAddr.sll_family = htons(PF_PACKET);
	destAddr.sll_protocol = htons(ETH_P_ALL);
	destAddr.sll_halen = 6;
	destAddr.sll_ifindex = 2; /* indice da interface pela qual os pacotes serao enviados. Eh necess�rio conferir este valor. */
	memcpy(&(destAddr.sll_addr), pacote.target_ethernet_address, ETHERNET_ADDR_LEN);

	if ((retValue = sendto(sockFd, &pacote, sizeof(pacote), 0, (struct sockaddr *)&(destAddr), sizeof(struct sockaddr_ll))) < 0)
	{
		//O PACOTE NAO FOI ENVIADO
		return false;
	}
	else
	{
		//O PACOTE FOI ENVIADO
		return true;
	}
}

/* tenta enviar pacote ate conseguir */
bool envia_pacote(estrutura_pacote pacote)
{
	bool pacote_foi_enviado = envia_pacote_aux(pacote);

	while(pacote_foi_enviado == false)
	{
		pacote_foi_enviado = envia_pacote_aux(pacote); 
	}

	return true;
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

	strcpy(ifname, INTERFACE_DE_REDE);

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

#endif
