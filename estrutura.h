#ifndef HEADER_MASTER
#define HEADER_MASTER

/* declaracao das constantes */
#define BUFFER_LEN 1500
#define BUFFSIZE 1500
#define ARPHRD_ETHER 1
#define ARP_PADDING_SIZE 18
#define ARPOP_REPLY 2
#define ARPOP_REQUEST 1
#define ETH_P_IP 0x0800
#define ETHERTYPE 0x0800
#define ETHERNET_ADDR_LEN 6
#define ETHERTYPE_LEN 2
#define IP_ADDR_LEN 4
#define MAC_ADDR_LEN 6
#define UDP_PROTOCOL 17

/* TAMANHO DOS DADOS DO PACOTE ETHERNET + IP */
const int SIZE_PACOTE_IP = (5 * sizeof(unsigned char)) + (4 * sizeof(unsigned short)) + (2 * sizeof(unsigned int)) + (2 * ETHERNET_ADDR_LEN * sizeof(unsigned char)) + (1 * sizeof(unsigned short));

unsigned char jogador1_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_jogador1
unsigned char jogador1_protocol_address[IP_ADDR_LEN];       // endereco_logico_jogador1
unsigned char jogador2_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_jogador2
unsigned char jogador2_protocol_address[IP_ADDR_LEN];       // endereco_logico_jogador2

typedef struct
{
	/* Cabeçalho Ethernet */
	unsigned char target_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
	unsigned char source_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
	unsigned short ethernet_type;                             // tipo_protocolo_ethernet

	/* Pacote IPv4 */
	unsigned char version:4;        // versao (nibble)
	unsigned char ihl:4;            // tamanho_cabecalho (nibble)
	unsigned char tos;              // tipo_servico
	unsigned short tlen;            // comprimento_pacote
	unsigned short id;              // identificacao
	unsigned short flags_offset;    // flags + deslocamento
	unsigned char ttl;              // tempo_de_vida
	unsigned char protocol;         // identificacao_protocolo
	unsigned short checksumip;      // check sum
	unsigned int src;               // endereco_fisico_origem
	unsigned int dst;               // endereco_fisico_destino

	/* Pacote UDP */
	unsigned short source_port;         // porta_origem
	unsigned short destination_port;    // porta_destino
	unsigned short size;                // tamanho_pacote
	unsigned short checksumudp;         //checksum

	/* Dados */
	bool ganhei;
	bool perdi;
	bool jogada_valida;
	unsigned char jogada_x;
	unsigned char jogada_y;
	unsigned char tabuleiro[3][3];               
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
    char datagrama[BUFFSIZE];
	memset (datagrama, 0, BUFFSIZE);
	memcpy(datagrama, &pacote_aux, sizeof(BUFFSIZE));
	return (unsigned short) in_cksum( (unsigned short*) datagrama, SIZE_PACOTE_IP);
}

#endif
