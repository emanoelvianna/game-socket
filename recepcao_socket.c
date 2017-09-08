#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netinet/in_systm.h>

#define BUFFSIZE 1518 /* definicao do tamanho do buffer */
/* definicao do tamanho da matriz */
#define N_LINHAS 3
#define N_COLUNAS 3

char matriz [N_LINHAS][N_COLUNAS];
unsigned char buff1[BUFFSIZE]; // buffer de recepcao
unsigned char buff_aux;
int sockd;
int on;
struct ifreq ifr;

/* metodo para incializar a matriz vazia */
void iniciarMatriz() {
  int i, j;
  for (i = 0; i < N_LINHAS; i++) {
    for (j = 0; j < N_COLUNAS; j++) {
      matriz[i][j] = '.';
    }
  }
}

/* metodo para desenhar a matriz */

void print() {
  printf("   1   2   3   \n");
  printf(" 1 %c | %c | %c \n", matriz[0][0], matriz[0][1], matriz[0][2]);
  printf("  ---|---|--- \n ");
  printf("2 %c | %c | %c \n", matriz[1][0], matriz[1][1], matriz[1][2]);
  printf("  ---|---|--- \n ");
  printf("3 %c | %c | %c \n", matriz[2][0], matriz[2][1], matriz[2][2]);
  printf("\n");
}

/* metodo para adicionar uma jogada a matriz */
void adicionarJogada(int linha, int coluna, char peca) {
  int i, j;
  for (i = 0; i < N_LINHAS; i++) {
    for (j = 0; j < N_COLUNAS; j++) {
      if (linha == i && coluna == j)
        matriz[i][j] = peca;
    }
  }
}

int main(int argc, char * argv[]) {
  iniciarMatriz();
  /* configuracoes para o socket */
  if ((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
    printf("Erro na criacao do socket.\n");
    exit(1);
  }

  /* O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo */
  strcpy(ifr.ifr_name, "enp2s0");
  if (ioctl(sockd, SIOCGIFINDEX, & ifr) < 0) {
    printf("erro no ioctl!");
  }
  ioctl(sockd, SIOCGIFFLAGS, & ifr);
  ifr.ifr_flags |= IFF_PROMISC;
  ioctl(sockd, SIOCSIFFLAGS, & ifr);

  /* recepcao de pacotes */
  while (1) {
    print();
	adicionarJogada(0, 0, 'x');
	adicionarJogada(1, 1, 'o');
  }
}