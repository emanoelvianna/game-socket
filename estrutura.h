#ifndef HEADER_MASTER
#define HEADER_MASTER
/* declaracao das constantes */

/* definindo a porta de conexão  */
#define PORT 4242
/* definicao do tamanho do buffer */
#define BUFFSIZE 1518
/* definicao do tamanho da matriz */
#define N_LINHAS 3
#define N_COLUNAS 3
#define BUFFER_LEN 1500
#define ETHERNET_ADDR_LEN 6 /* indicando o tamanho do mac endereco fisico */
#define ETHERTYPE_LEN 2
#define IP_ADDR_LEN 4       /* indicando o tamanho do mac endereco logico */
#define UDP_PROTOCOL 17
#define ETHERTYPE 0x0800 /** indicando que é do tipo IPv4 **/
#define BUFFER_SIZE 1500 /** tamanho do buffer de leitura do pacote ethernet+ipv4+udp **/

/* tamanho dos dados do pacote ethernet + ip */
const int SIZE_PACOTE_IP = (5 * sizeof(unsigned char)) + (4 * sizeof(unsigned short)) + (2 * sizeof(unsigned int)) + (2 * ETHERNET_ADDR_LEN * sizeof(unsigned char)) + (1 * sizeof(unsigned short));

/* declaracao das estruturas para os jogadores */
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
    unsigned char *payload_ip; // dados_ip
    size_t payload_size_ip;

    /* Pacote UDP */
    unsigned short source_port;      // porta_origem
    unsigned short destination_port; // porta_destino
    unsigned short size;             // tamanho_pacote
    unsigned short chechsum;
    unsigned char *payload_udp; // dados_udp
    size_t payload_size_udp;

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