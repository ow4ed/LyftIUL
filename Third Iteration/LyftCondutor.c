#include "defines.h"

int posCondutor;

int randomwait(){//devolve um numero inteiro de 1 a 20
	time_t t;
	srand((unsigned) time(&t));
	int i = (rand() % 20)+ 1;
	return i;
}

void start(){
	char tmp[100]; // usado pelo fgets para obter numeros do input
	int pid = getpid();	
	printf("O meu pid é:%d;\n", pid);

    int idShm = shmget(0x82247,0,0);
	exit_on_error(idShm,"LyftCondutor: Erro no shmget");
	int idSem = semget(0x82247,5,0);	
	exit_on_error(idSem,"LyftCondutor: semget");
	ShMem * estrutura = (ShMem *)shmat(idShm, 0,0);
	Tcondutor * c = (Tcondutor *)&(estrutura->inicio);
	exit_on_null(c,"LyftCondutor: Erro no atach");
 
	printf("Número Condutor?\n");
	fgets(tmp,100,stdin);
	int numCondutor = atoi(tmp);
	int encontrou = 0;
	int totalCondutores = estrutura->numCondutores;	

	int res = semop(idSem,&DOWN,1);
	exit_on_error(res,"LyftAdmin: semdown");
	for(int i=0;i<totalCondutores && encontrou==0; i++){//procurar na memoria partilhada se existe algum condutor com o numero dado
		if(c[i].numero==numCondutor){//se existir um condutor com o numero dado
			c[i].activo = 1;
			c[i].disponivel_desde = time(NULL);
			c[i].PID = pid;
			encontrou = 1;
			posCondutor = i;	
			printf("Nome do condutor: %s\n",c[i].nome);
			break;
		}
	}
	res= semop(idSem,&UP,1);
	exit_on_error(res,"LyftAdmin: semup");
	
	if(encontrou==0){//caso nao encontre nenhum condutor com o numero dado
		exit_on_error(-1,"Não existe nenhum condutor com esse número\n");
	}
	int idMsg = msgget(0x82247,0);
	exit_on_error(idMsg, "LyftCondutor: Erro no msget!");
	int status;
	MsgViagem m;
	MsgViagem mPassageiro;
	MsgViagem recibo;
	int valor=-1;
	int pontos=-1;
	while(1){//para receber pedidos de viagens enquanto nao fizer CTRL+C
		printf("À espera de um pedido de transporte......\n");
		status = msgrcv(idMsg, &m, sizeof(m.dados), pid, 0);//recebe uma mensagem do LyftAdmin
		if(status<0){// se falhar a rececao da mensagem o programa fecha e o condutor fica inativo
			c[posCondutor].activo=0;
			exit_on_error(status,"LyftCondutor: Erro no msgrcv");
		}
		printf( "PID passageiro = %d,local de encontro: %s\n", m.dados.pid_passageiro,m.dados.local_encontro);
		
		//enviar a mensagem para o passageiro
		char localizacao[100];
		printf("Morada do sítio onde está?\n");
	    myfgets(localizacao,100,stdin);
	    printf("Eu estou em: %s\n", localizacao);
	 
		mPassageiro.tipo = m.dados.pid_passageiro;
		mPassageiro.dados.pid_condutor = pid;
		strcpy(mPassageiro.dados.local_encontro,localizacao);

		status = msgsnd(idMsg, &mPassageiro, sizeof(mPassageiro.dados), 0);//envia a mensagem para o Passageiro
		if(status<0){// se falhar o envio da mensagem, o programa fecha e o condutor fica inativo
			c[posCondutor].activo=0;
			exit_on_error(status,"LyftCondutor: Erro no msgsnd para o Passageiro");
		}
		printf("Enviei ao passageiro a mensagem a dizer onde estou.\n");
	
		sleep(randomwait());//simulacao do tempo da viagem
	
		valor=-1;
		while(valor<0){//Aqui o passageiro insere o valor da viagem
			printf("Insira o valor da viagem:");
			fgets(tmp,10,stdin);
			valor = atof(tmp);
		}	
		pontos=-1;//aqui o passageiro insere os pontos que vai atribuir a esta viagem
		while(pontos<0 || pontos >10){//os pontos têm de ser entre 1 e 10
			printf("Insira os pontos da viagem entre(0-10):");
			fgets(tmp,10,stdin);
			pontos = atof(tmp);	
			if(pontos <0 || pontos >10){
				printf("TEM DE SER UM NUMERO ENTRE 0 e 10!\n");	
			}
		}
		
		// enviar o recibo para o LyftAdmin
		recibo.tipo=1;
		recibo.dados.pid_passageiro=m.dados.pid_passageiro;
		recibo.dados.pid_condutor=pid;
		recibo.dados.pontos=pontos;
		recibo.dados.valor=valor;
		status = msgsnd(idMsg,&recibo,sizeof(recibo.dados),0);//envio do recibo para o LyftAdmin
		if(status<0){// no caso de falhar o envio da mensagem o programa fecha e condutor fica inativo
			c[posCondutor].activo=0;
			exit_on_error(status,"LyftCondutor: Erro ao enviar o recibo para LyftAdmin");
		}
		printf("Enviado o recibo para LyftAdmin\n");	
	}

}	
void trata_sinal(int signal){
	int idSem = semget(0x82247,5,0);
	if( signal == SIGINT){//tratamento do CTRL+C
		printf("O condutor fez CTRL+C,o programa vai fechar e este condutor vai ficar inativo\n");
		int id = shmget(0x82247,0,0);
		ShMem * estrutura = (ShMem * ) shmat(id,0,0);
		Tcondutor * c = (Tcondutor * ) &(estrutura->inicio);
		
		int res = semop(idSem,&DOWN,1);
		exit_on_error(res,"LyftAdmin: semdown");	
		c[posCondutor].activo=0;//por este condutor como inativo
		res= semop(idSem,&UP,1);
		exit_on_error(res,"LyftAdmin: semup");
		exit(0);
	}
}

int main() {
	signal(SIGINT,trata_sinal);
	start();
}		
