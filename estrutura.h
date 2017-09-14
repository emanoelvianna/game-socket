
/* declaracao das constantes */
#define ETHERNET_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define ETHERTYPE 0x0800 /** indicando que é do tipo IPv4 **/

#define BUFFER_SIZE 1500 /** tamanho do buffer de leitura do pacote ethernet+ipv4+udp **/

/* declaracao das estruturas para os jogadores */
unsigned char jogador1_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_jogador1
unsigned char jogador1_protocol_address[IP_ADDR_LEN];       // endereco_logico_jogador1

unsigned char jogador2_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_jogador2
unsigned char jogador2_protocol_address[IP_ADDR_LEN];       // endereco_logico_jogador2

struct estrutura_pacote
{
    /* Cabeçalho Ethernet */
    unsigned char target_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
    unsigned char source_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
    unsigned short ethernet_type;                             // tipo_protocolo_ethernet

    /* Pacote IPv4 */
    unsigned char version;       // versao
    unsigned char ihl;           // tamanho_cabecalho
    unsigned char tos;           // tipo_servico
    unsigned short tlen;         // comprimento_pacote
    unsigned short id;           // identificacao
    unsigned short flags;        // flags
    unsigned short flags_offset; // deslocamento
    unsigned char ttl;           // tempo_de_vida
    unsigned char protocol;      // identificacao_protocolo
    unsigned short chsum;
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
};
