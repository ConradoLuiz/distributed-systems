/************************************************************************/
/* Nome do Arq : cliente.c                                              */
/* Funcao      : cliente TCP                                            */
/************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

//------------------------------------------------------------------------
int s;
struct sockaddr_in BCEnderServidor;
struct sockaddr_in BCEnderCliente;
int TamNomeSoquete;
char *ptrNomeEstacao;
int TamNomeEstacao;
char Nome[64];
struct hostent *ptrDadosDoHost;
struct hostent DadosDoHost;
int Tam1,Tam2;
char Msg1[256]="Por favor, poderia prestar-me um servico?";
int PORTA;
FILE * arq;

//------------------------------------------------------------------------
int main(){
  int result;
  int i;
//   union wait CausaTerminacao;

    printf("\n\n\n");
    printf("[Cliente] Processo cliente comecou a rodar.\n");
    printf("[cliente] Quer usar o arquivo de configuracao?(s): ");
    gets(Nome);
    if(strlen(Nome)==0){
       arq = fopen("./server.data","r");
       fscanf(arq,"%s%d",&Nome,&PORTA);
       fclose(arq);
    }else{
       printf("[Cliente] Qual o nome do servidor ? ");
       scanf("%s",Nome);
       printf("\n[Cliente] Qual a porta do servi�o ? ");
       scanf("%d",&PORTA);
       arq = fopen("./server.data","w");
       fprintf(arq,"%s\n%d\n",Nome,PORTA);
       fclose(arq);
    }
    printf("\n[Cliente] Usando o servidor %s e a porta %d.\n",Nome,PORTA);

    if((s = socket(PF_INET,SOCK_STREAM,0))<0){
       perror("[Cliente] Erro na criacao do socket: ");
       exit(0);
    }
 
    ptrDadosDoHost=gethostbyname(Nome);
    bzero((char *)&BCEnderServidor,sizeof(BCEnderServidor));
    BCEnderServidor.sin_family = AF_INET;
    BCEnderServidor.sin_port = htons(PORTA);
   
    bcopy(ptrDadosDoHost->h_addr,
          &BCEnderServidor.sin_addr.s_addr, 
          ptrDadosDoHost->h_length);

    result = connect(s,(struct sockaddr *)&BCEnderServidor,
                                           sizeof(BCEnderServidor));
    if(result<0){
       perror("[Cliente] Erro no connect: ");
       exit(0);
    }

    write(s,Msg1,strlen(Msg1));
    shutdown(s,1);

    for(i=0;i<80;i++)Msg1[i]=0;
    result = read(s,Msg1,60);
    printf("[Cliente] Recebeu: %s\n",Msg1);
    shutdown(s,2);
}





