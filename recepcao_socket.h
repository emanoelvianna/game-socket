/* declaracao das estruturas */
struct estrutura_pacote_udp
{
    /* Cabeçalho Ethernet */
    unsigned char target_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
    unsigned char source_ethernet_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
    unsigned short ethernet_type;                             // tipo_protocolo_ethernet
    /* Pacote ARP */
    unsigned short hardware_type; // tipo_hardware
    unsigned short protocol_type; // tipo_protocolo

    unsigned char hardware_address_length; // comprimento_endereco_mac
    unsigned char protocol_address_length; // comprimento_endereco_logico

    unsigned short arp_options; // tipo_da_operacao

    unsigned char source_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_origem
    unsigned char source_protocol_address[IP_ADDR_LEN];       // endereco_logico_origem

    unsigned char target_hardware_address[ETHERNET_ADDR_LEN]; // endereco_fisico_destino
    unsigned char target_protocol_address[IP_ADDR_LEN];       // endereco_logico_destino

    unsigned char padding[ARP_PADDING_SIZE]; // dados_de_preenchimento
};