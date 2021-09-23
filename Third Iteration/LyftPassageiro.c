#include "defines.h"

void start(){

	int pid = getpid();

	int idShm = shmget(0x82247,0,0);//id da memoria partilhada
	exit_on_error(idShm,"LyftPassageiro: Erro a aceder a memoria partilhada dos passageiros");
	int idSem = semget(0x82247,5,0);//id do conjunto de semaforos
	exit_on_error(idSem,"LyftPassageiro: semget");

	ShMem * estrutura = (ShMem *)shmat(idShm,0,0);
	Tcondutor * c = (Tcondutor *)&(estrutura->inicio);
	int totalCondutores = estrutura->numCondutores;
	Tpassageiro * p = (Tpassageiro *)&(c[totalCondutores]); 
	exit_on_null(p,"LyftPassageiro: Erro no atach");
	
	char tmp [100];// declaracao da variavel tmp que será usada em vários fgets
	printf("Número Passageiro?\n");
	fgets(tmp,100,stdin);
	int num = atoi(tmp);
	int totalPassageiros = estrutura->numPassageiros;
	int encontrou=0;
	
	int res = semop(idSem,&DOWN,1);
	exit_on_error(res,"LyftAdmin: semdown");
	for(int i=0;i<totalPassageiros && encontrou==0;i++){//procura do passageiro com o numero dado
		if(	p[i].numero==num){// se encontrou um passageiro com o numero dado
			printf("O aluno com este numero é: %s.\n",p[i].nome);
			encontrou=1;
			break;
		}
	}	
	res= semop(idSem,&UP,1);
	exit_on_error(res,"LyftAdmin: semup");
	
	if(encontrou==0){
		exit_on_error(-1,"Não existe nenhum aluno com esse número\n");
	}

	int status;
	int idMsg = msgget(0x82247,0);//id da fila de mensagens
    exit_on_error(idMsg, "LyftPassageiro:Erro no msget!");

	MsgViagem m;
	m.tipo=1;//"caixa de correio" onde o LyftAdmin esta à espera de mensagens
	m.dados.pid_passageiro=pid;
	printf("Local de encontro?: ");
	myfgets(tmp,100,stdin);//a variavel tmp foi declarada anteriormente
	strcpy(m.dados.local_encontro,tmp);
	m.dados.data=time(NULL);

	printf("À espera de um condutor.........o meu pid é %d\n",pid);
	int pid_condutor_aux = -1;
	int contador=0;
	while(pid_condutor_aux==-1 && contador <15){// o LyftAdmin devolve uma mensagem com pid_condutor a -1 caso não tenha encontrado um condutor para satisfazer o pedido deste passageiro enviado anteriormente
		printf("A espera de um condutor.....\n");	
		status=msgsnd(idMsg,&m,sizeof(m.dados),0);//envia a mensagem para o LyftAdmin
	    exit_on_error(status,"Erro ao enviar mensagem no LyftPassageiro");
		
		status = msgrcv(idMsg,&m,sizeof(m.dados),pid,0);//espera pela mensagem do condutor ou LyftAdmin
		exit_on_error(status,"erro ao receber mensagem no LyftPassageiro");

		m.dados.pid_passageiro=pid;
		pid_condutor_aux=m.dados.pid_condutor;
		if(pid_condutor_aux==-1){
			sleep(5);//fica a espera 5 segundos para enviar uma mensagem para o lyftAdmin, na esperanca de que desta tentativa haja um condutor disponivel
			m.tipo=1;
			contador++;
		}
	}
	if(contador==15){// se as tentativas encontrar o condutor tiverem exedido o limite
		printf("Não encontrei nenhum condutor\n");
		exit(0);
	}

	printf("Pid condutor: %d, está em %s\n",pid_condutor_aux,m.dados.local_encontro);
	
}	



int main(){
	start();
}
