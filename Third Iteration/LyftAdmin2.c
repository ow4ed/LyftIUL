#include "defines.h"

int readNrLinhasFicheiro (char ficheiro[] ){//esta funcao le o num de linhas que um ficheiro tem
    FILE *f = fopen(ficheiro, "r");

    if(!f){
        perror("Erro na leitura do ficheiro");
        exit(1);
    }

    char input[512];
    int i=0;
    while(fgets(input,512,f) != NULL){
        i++;
    }
    fclose(f);
    return i;
}

void obter_substring(char linha[], char resultado[], char separador, int indice) {// funcao que serve para obter um bocado de uma string dado um separador
    int i;
    int j=0;
    int ind=0;
    for (i=0; linha[i] != '\0'; i++) {
        if ( linha[i] == separador ) {
            ind++;
        } else if (ind == indice) {
            resultado[j++]=linha[i];
        }
    }
    resultado[j]='\0';
}

void ler_condutores(){//esta funcao le o ficheiro condutores.txt e armazena cada condutor em memoria
    FILE *f = fopen("condutores.txt","r");

    if(!f){// se houver erro na aberturar do ficheiro condutores.txt
        perror("Erro na leitura do ficheiro condutores.txt");
        exit(1);
    }
	
    int id = shmget(0x82247,0,0 );
	ShMem * estrutura = (ShMem *) shmat(id,0,0);
	exit_on_null(estrutura,"LyftAdmin: erro no shmat");
	Tcondutor * c = (Tcondutor *) &(estrutura->inicio);

    char input[512];
    char tmp[512];
    int i=0;

	int idSem = semget(0x82247,5,0);
    int res = semop(idSem,&DOWN,1);
	exit_on_error(res,"LyftAdmin: semdown");

    while(fgets(input,512,f) !=NULL){//para cada linha do ficheiro condutores.txt
        obter_substring(input,tmp,':',0);
        c[i].numero= atoi(tmp);
        obter_substring(input,c[i].nome,':',1);
        obter_substring(input,c[i].turma,':',2);
        obter_substring(input,c[i].telemovel,':',3);
        obter_substring(input,c[i].email,':',4);
        obter_substring(input,c[i].tipo,':',5);
        obter_substring(input,c[i].marca,':',6);
        obter_substring(input,c[i].matricula,':',7);
        obter_substring(input,tmp,':',8);
        c[i].viagens=atoi(tmp);
        obter_substring(input,tmp,':',9);
        c[i].pontos=atoi(tmp);
        obter_substring(input,tmp,':',10);
        c[i].saldo=atof(tmp);
        c[i].activo=0;
        c[i].disponivel_desde=0;
        c[i].PID=0;
        i++;
    }
	res = semop(idSem,&UP,1);
	exit_on_error(res,"LyftAdmin: semup");
    fclose(f);
}

void ler_passageiros(){//esta funcao le o ficheiro passageiros.txt e armazena cada passageiro em memoria
    FILE *f = fopen("passageiros.txt","r");

    if(!f){//se houver um erro ao abrir o ficheiro passageiros.txt
        perror("Erro na leitura do ficheiro passageiros.txt");
        exit(1);
    }

	int id = shmget(0x82247,0,0);
	ShMem * estrutura = (ShMem *)shmat(id,0,0);
	exit_on_null(estrutura,"LyftAdmin: erro no shmat");
	Tcondutor * c = (Tcondutor *)&(estrutura->inicio);
	int numCond = estrutura->numCondutores;
	Tpassageiro * p = (Tpassageiro * )&(c[numCond]);

    char input[512];
    char tmp[512];
    int i=0;

	int idSem = semget(0x82247,5,0);
    int res = semop(idSem,&DOWN,1);
    exit_on_error(res,"LyftAdmin: semdown");

    while(fgets(input,512,f) != NULL){//para cada linha do ficheiro passageiros.txt
        obter_substring(input,tmp,':',0);
        p[i].numero=atoi(tmp);
        obter_substring(input,p[i].nome,':',1);
        obter_substring(input,p[i].turma,':',2);
        obter_substring(input,p[i].telemovel,':',3);
        obter_substring(input,p[i].email,':',4);
        strcpy(p[i].c_credito,"\0");
        i++;
    }
	res = semop(idSem,&UP,1);
	exit_on_error(res,"LyftAdmin: semup");
    fclose(f);
}

void start(){
    int nrLinhasFicheiroCondutores = readNrLinhasFicheiro("condutores.txt");//num de linhas no ficheiro condutores.txt
    int nrLinhasFicheiroPassageiros = readNrLinhasFicheiro("passageiros.txt");//num de linhas no ficheiro passageiros.txt
	
	int idShM = shmget(0x82247,2*sizeof(int)+nrLinhasFicheiroCondutores*sizeof(Tcondutor)+nrLinhasFicheiroPassageiros*sizeof(Tpassageiro),IPC_CREAT | IPC_EXCL | 0666);//cria a memoria partilhada se nao existir
	int idqueue = msgget(0x82247,0666 | IPC_CREAT | IPC_EXCL);//cria a fila de mensagens se nao existir
	int idSem = semget(0x82247,5,IPC_CREAT | 0666 | IPC_EXCL);//cria o conjunto dos semaforos se nao existirem
	
	idSem = semget(0x82247,5,0);
	for ( int s =0;s<5;s++){//inicializar os semaforos a 1, independentemente dos semaforos ja terem sido criados antes ou agora
		int status = semctl(idSem,s,SETVAL,1);
		exit_on_error(status,"LyftAdmin: Erro ao dar reset ao semaforo\n");
	}
	
	if( idqueue <0){// se ja existir a fila de mensagens, a mesma vai ser apagada e criada de novo para garantir que nao tinha lixo armazenado
		idqueue = msgget(0x82247, 0666);
		int pp = msgctl(idqueue,IPC_RMID,NULL);//remover a antiga
		if(pp == -1 ){
			exit_on_error(pp,"Erro ao removar a fila de mensagens no LyftAdmin");
		}
		idqueue = msgget(0x82247,0666 | IPC_CREAT | IPC_EXCL);// criar uma nova limpa
	}
	if(idShM >=0){//caso da shm ser criada agora e nao existia antes
		printf("Entrei na zona em que se assume que a shared memory foi criada agora \n");
		ShMem * estrutura = (ShMem * ) shmat(idShM,0,0);
		estrutura->numCondutores = nrLinhasFicheiroCondutores;
		estrutura->numPassageiros = nrLinhasFicheiroPassageiros;
		ler_condutores();
		ler_passageiros();
	}

	int nfr = fork();
	if(nfr==0){//o processo filho que esta à espera de mensagens 
		while(1){
			MsgViagem m;
			int status = msgrcv(idqueue,&m,sizeof(m.dados),1,0);//espera receber uma mensagem
			exit_on_error(status,"Erro ao receber uma mensagem no LyftAdmin");
			
			int idShMemory= shmget(0x82247,0,0);
			ShMem * estrutura = (ShMem * ) shmat(idShMemory,0,0);
			exit_on_null(estrutura,"LyftAdmin: erro shmat");
			Tcondutor * c = (Tcondutor * )&(estrutura->inicio);
			int numCondutores=estrutura->numCondutores;
	
			int res = semop(idSem,&DOWN,1);
			if(m.dados.pid_condutor>0 && m.dados.pid_passageiro>0 && m.dados.valor>0){// caso da mensagem recebida ser um recibo
				int i=0;
                for(i=0;i<numCondutores;i++){
                    if(c[i].PID==m.dados.pid_condutor){
                        c[i].saldo=c[i].saldo+m.dados.valor;
                        c[i].pontos=c[i].pontos+(int)m.dados.pontos;
                        c[i].disponivel_desde=time(NULL);
						c[i].viagens=c[i].viagens+1;
                        c[i].activo=1;
                        printf("Foi atualizado o saldo e os pontos do condutor\n");
                        break;
                    }
				}
			}
			else {// caso da mensagem ser um pedido de transporte de um passageiro
				long baixo = 999999999999;
                int i=0;
				int ativos=0;
				int posBaixo=-1;
				for(int s=0;s<numCondutores;s++){//procura quantos condutores estao ativos
					if(c[s].activo==1){
						ativos++;
						break;
					}
				}
				if(ativos==0){// se nao existirem condutores ativos, o passageiro que pediu transporte vai ficar a espera que algum condutor fique ativo
					m.tipo=m.dados.pid_passageiro;// o LyftAdmin vai enviar para esse passageiro uma mensagem que vai fazer com que o passageiro espere
					m.dados.pid_condutor=-1;
					status=msgsnd(idqueue,&m,sizeof(m.dados),0);
					exit_on_error(status,"LyftAdmin: Erro ao enviar mensagem para o Passageiro para dizer que nao ha condutores disponiveis");	
				}
				else{
					printf("Enviando a mensagem para o Condutor\n");
					for(i=0;i<numCondutores;i++){//descobrir o condutor que vai ficar com o transporte
						if(c[i].activo==1 && c[i].disponivel_desde<baixo ){
							baixo=c[i].disponivel_desde;
							posBaixo=i;//indice do condutor com o disponivel desde mais baixo e que esteja ativo
						}
					}
					c[posBaixo].activo=0;
					//enviar mensagem para o condutor
					m.tipo=c[posBaixo].PID;
					printf("%d\n",c[posBaixo].PID);
					status = msgsnd(idqueue,&m,sizeof(m.dados),0);//envia a mensagem para o condutor
					exit_on_error(status,"LyftAdmin: Erro ao enviar mensagem para o Condutor");
				}
			}
			res = semop(idSem,&UP,1);
            exit_on_error(res,"LyftAdmin: up");
		}
	}
}

void cria_lyftadmin_pid(){// sempre que o programa LyftAdmin é executado esta funcao ira colocar o seu pid no ficheiro lyftadmin.pid para ser utilizador pelo LyftTask
	int pid = getpid();
	FILE *f;
    f=fopen("lyftadmin.pid","w");
	if(f!=NULL){
		fprintf(f,"%d",pid);
	}
    fclose(f);
}

void mostralistacondutores(){//funcao que serve para listar todos os condutores 
	printf("-------------------------Condutores--------------------------\n");
	
	int idShm=shmget(0x82247,0,0);//id memoria partilhada 
	ShMem * estrutura = (ShMem * ) shmat(idShm,0,0);
    Tcondutor * c = (Tcondutor * )&(estrutura->inicio);
	int totalCondutores = estrutura->numCondutores;
	int idSem = semget(0x82247,5,0);
	exit_on_error(idSem,"LyftAdmin: semget");

	int ativos=0;
	struct tm st;
    time_t t;
	int semRes = semop(idSem,&DOWN,1);
    exit_on_error(semRes,"LyftAdmin: semdown");
	for(int i=0;i<totalCondutores; i++){
		t=c[i].disponivel_desde;
		st = * gmtime ( &t );
		if(c[i].activo == 1){//para contar quantos condutores estao ativos
			ativos++;	
		}
	printf ("condutor: %3d %-5d %-20s %-4s %-9s %-20s %-10s %-20s %-15s %-3d %-5d %-.1f %-d %d-%02d-%02d %-d\n", i, c[i].numero,c[i].nome,c[i].turma,c[i].telemovel,c[i].email,c[i].tipo,c[i].marca,c[i].matricula,c[i].viagens,c[i].pontos,c[i].saldo,c[i].activo,1900+st.tm_year,st.tm_mon+1,st.tm_mday,c[i].PID );//os valores diferentes em %3d,%5d,%20d,etc sao simplesmente para que na listagem dos condutores o texto resultado fique formatado
	}
	semRes = semop(idSem,&UP,1);
	exit_on_error(semRes,"LyftAdmin: semup");
	printf("Existem %d condutores no total.\n\n\n",totalCondutores);
	printf("Existem %d condutores ativos neste momento\n\n\n",ativos);
}

void mostralistapassageiros(){//funcao que serve para listar todos os passageiros
	printf("-------------------------Passageiros-------------------------\n");
	
	int idSem = semget(0x82247,5,0);
	int idShm=shmget(0x82247,0,0);//id memoria partilhada
    ShMem * estrutura = (ShMem *)shmat(idShm,0,0);
    Tcondutor * c = (Tcondutor *)&(estrutura->inicio);
    int totalCondutores = estrutura->numCondutores;
    Tpassageiro * p = (Tpassageiro *)&(c[totalCondutores]);
	int totalPassageiros = estrutura->numPassageiros;
	int res = semop(idSem,&DOWN,1);
	exit_on_error(res,"LyftAdmin: semdown");
	for(int i=0;i<totalPassageiros;i++){
	printf("passageiro: %3d %5d %-20s %-5s %-9s %-20s %-20s\n",i,p[i].numero,p[i].nome,p[i].turma,p[i].telemovel,p[i].email,p[i].c_credito);//os valores diferentes %3d,%5d,%-20s,etc sao apenas para formatar a listagem dos passageiros
	}
	res = semop(idSem,&UP,1);
	exit_on_error(res,"LyftAdmin: semup");
	printf("Existem %d passageiros.\n\n\n",totalPassageiros);
}

void escrever_passageiros(){//esta funcao apaga o ficheiro passageiros.txt e cria-o de novo a partir da informacao guardada em memoria
    remove("/home/a82247/projeto/passageiros.txt");//remove o ficheiro passageiros.txt
    FILE *f = fopen("/home/a82247/projeto/passageiros.txt","w");//cria de novo o ficheiro passageiros.txt
    if(NULL==f){
        perror("Erro na criacao de passageiros.txt\n");
    }
	int idSem = semget(0x82247,5,0);
	int idShMemory = shmget(0x82247,0,0);
    ShMem * estrutura = (ShMem *)shmat(idShMemory,0,0);
    Tcondutor * c = (Tcondutor *)&(estrutura->inicio);
    int numCond = estrutura->numCondutores;
	int numPass = estrutura->numPassageiros;
    Tpassageiro * p = (Tpassageiro *)&(c[numCond]);

	
    printf("Vou escrever nos passageiros.txt\n");
    int k;
	int res = semop(idSem,&DOWN,1);//enquanto se escreve para o ficheiro passageiros.txt nao queremos que alguem esteja a manipular a memoria partilhada
	exit_on_error(res,"LyftAdmin: semdown");
    for(k=0;k<numPass;k++){//este ciclo percorre os passageiros em memoria e escreve-os no ficheiro passageiros.txt
         fprintf(f,"%d:%s:%s:%s:%s:%s\n",p[k].numero,p[k].nome,p[k].turma,p[k].telemovel,p[k].email,p[k].c_credito);
    }
	res= semop(idSem,&UP,1);
	exit_on_error(res,"LyftAdmin: semup");
    fclose(f);
}

void escrever_condutores(){//esta funcao apaga o ficheiro condutores.txt e cria-o de novo a partir da informacao guardada em memoria
    remove("/home/a82247/projeto/condutores.txt");//remove o ficheiro condutores.txt
    FILE *f = fopen("/home/a82247/projeto/condutores.txt","w");// cria de novo o ficheiro condutores.txt
    if(NULL==f){
        perror("Erro na criacao de condutores.txt\n");
    }
	
	int idSem = semget(0x82247,5,0);	
	int idShMemory= shmget(0x82247,0,0);//id memoria partilhada
	ShMem * estrutura = (ShMem * ) shmat(idShMemory,0,0);
    Tcondutor * c = (Tcondutor * )&(estrutura->inicio);
	int numCondutores = estrutura->numCondutores;
    
	printf("Vou escrever nos condutores.txt\n");
    int i;
	int res = semop(idSem,&DOWN,1);//enquanto se escreve no ficheiro condutores.txt nao queremos que alguem esteja a manipular a memoria partilhada
    exit_on_error(res,"LyftAdmin: semdown");
    for(i=0;i<numCondutores;i++){//este ciclo percorre os condutores guardados em memoria e escreve-os no ficheiro condutores.txt
        fprintf (f,"%d:%s:%s:%s:%s:%s:%s:%s:%d:%d:%.1f:%d:%d:%d\n", c[i].numero,c[i].nome,c[i].turma,c[i].telemovel,c[i].email,c[i].tipo,c[i].marca,c[i].matricula,c[i].viagens,c[i].pontos,c[i].saldo,c[i].activo,c[i].disponivel_desde,c[i].PID );
    }
	res= semop(idSem,&UP,1);
    exit_on_error(res,"LyftAdmin: semup");
    fclose(f);
}


void trata_sinal(int signal){//esta funcao trata os diferentes sinais que o processo LyftAdmin recebe
	if ( signal == SIGUSR1 ){// no caso do sinal ser o SIGUSR1 vao ser atualizadas as informacoes nas estruturas em memoria
		ler_condutores();//vai ser atualizada a informacao guardada em memoria
	}
	if ( signal == SIGTERM ){//no caso do sinal ser o SIGTERM os condutores e passageiros, que estao gravados em memoria, serao escritos nos ficheiros condutores.txt e passageiros.txt,respetivamente
		escrever_condutores();
		escrever_passageiros();
		exit(0);//apos essa escrita o programa LyftAdmin termina
	}
	if( signal == SIGALRM){//ao receber o sinal SIGALARM é executado o LyftTask
       printf("Tratamento do SIGALARM\n");
       int t=fork();
       if( t==0 ){
          execl("/home/a82247/projeto/LyftTask", NULL);
          printf("Executei o lifttask\n");
       }
       else{
         wait(NULL);
         alarm(ALARMTIMER);//o alarm volta a ser armado para daqui a 60s 
         printf("Enviado novo alarm\n");
      }
    }
}



int exists(int num,int pc){// esta funcao procura na zona da memoria partilhada dos passageiros ou na zona de memoria partilhada dos condutores consoante o valor de pc é 0 ou 1, respetivamente, e devolve o numero da linha em que esta o numero passado como argumento.Se nao existir o numero na lista desejada é devolvido o valor -1
	int encontrou=-1;
	int i;
	int id = shmget(0x82247,0,0);
    ShMem * estrutura = (ShMem *) shmat(id,0,0);
    int numCond = estrutura->numCondutores;
	int numPass = estrutura->numPassageiros;
    Tcondutor * c = (Tcondutor * ) &(estrutura->inicio);
    Tpassageiro * p = (Tpassageiro * ) &(c[numCond]);

	if(pc==0){//se o valor de pc for 0, o num vai ser procurado na zona da memoria partilhada onde estao os passageiros
		Tpassageiro * p = (Tpassageiro * ) &(c[numCond]);
		int k;
		for(k=0;k<numPass;k++){
			if(p[k].numero==num){//caso encontre esse numero, é devolvida a posicao em que esta o passageiro com esse numero
				encontrou=0;
				return k;
			}
		}
		return -1;//se nao encontrar o num é devolvido -1
	}
	if(pc==1){//se o valor de pc for 1, o num vai ser procurado na zona de memoria partilhada onde estao os condutores
		for(i=0;i<numCond;i++){
			if(c[i].numero==num){//caso encontre esse numero, é devolvida a posicao em que esta o condutor com esse numero
				encontrou=0;
				return i;
			}
		}
		return -1;//se nao encontrar o num é devolvido -1
	}
}

void menu(){//funcao que apresenta o menu de opcoes repetitivamente ate ser escolhido o 0
	int res=-1;
	int num;
	int pos;
	char tmp[50];
	char str[50];
	while(res!=0){
		printf("1) Imprimir memória\n");
		printf("2) Alterar passageiro\n");
		printf("3) Alterar condutor\n");
		printf("0) SAIR\n");
		fgets(tmp,100,stdin);
		res=atoi(tmp);
		if(res==1){
			mostralistacondutores();
			mostralistapassageiros();
			printf("\n");
        }
		int idSem = semget(0x82247,5,0);	
		if(res==2){
			int naorespondido=0;//este contador verifica se o utilizador fez de facto alguma alteracao no passageiro, caso o utilizador entrar para alterar um passageiro mas nao fizer nenhuma mudanca nos campos nao vai ser preciso atualizar o ficheiro passageiros.txt
			printf("Numero estudante passageiro?\n");
			fgets(tmp,100,stdin);
			num=atoi(tmp);
			pos=exists(num,0);
			
			int id = shmget(0x82247,0,0);
            ShMem * estrutura = (ShMem *) shmat(id,0,0);
            int numCond = estrutura->numCondutores;
            Tcondutor * c = (Tcondutor * ) &(estrutura->inicio);
            Tpassageiro * p = (Tpassageiro * ) &(c[numCond]);
	
			if(pos!=-1){
					int res = semop(idSem,&DOWN,1);
					exit_on_error(res,"LyftAdmin: semdown");		
					printf("Se desejar alterar um campo deve inserir o novo valor,senão deixar o espaço em branco para nao alterar\n");
					printf("Nome?\n");
					myfgets(str,50,stdin);		
					if(strcmp(str,"")!=0){//alterar o nome
						strcpy(p[pos].nome,str);
						printf("Foi alterado o nome\n");
					}
					else{ 
						printf("O nome nao foi alterado\n");
						naorespondido++;
					}
					printf("Turma?\n");
					myfgets(str,10,stdin);
					if(strcmp(str,"")!=0){//alterar a turma
						strcpy(p[pos].turma,str);
						printf("Foi alterada a turma\n");
					}
					else{
						printf("Nao foi alterada a turma\n");
						naorespondido++;
					}
					printf("Telemovel?\n");
					myfgets(str,15,stdin);
                    if(strcmp(str,"")!=0){//alterar o num telemovel
						strcpy(p[pos].telemovel,str);
						printf("Foi alterado o num telemovel\n");
					}
					else{
						printf("Nao foi alterado o num telemovel\n");
						naorespondido++;
					}
					printf("email?\n");
					myfgets(str,40,stdin);
                    if(strcmp(str,"")!=0){//alterar o email
						strcpy(p[pos].email,str);
						printf("Foi alterado o email\n");
					}
					else{ 
						printf("Nao foi alterado o email\n");
						naorespondido++;
					}
					printf("cartao_credito?\n");
					myfgets(str,20,stdin);
                    if(strcmp(str,"")!=0){//alterar o num c_credito
						strcpy(p[pos].c_credito,str);
						printf("Foi alterado o num de c_credito");
					}		
					else{
						printf("Nao foi alterado o num de c_credito\n");
						naorespondido++;
					}
					res= semop(idSem,&UP,1);
					exit_on_error(res,"LyftAdmin: semup");
					if(naorespondido==5){//caso nao tenha alterado nada
						printf("Nada foi alterado...\n");
					}else{
						escrever_passageiros();//para que as alteracoes que foram feitas em memoria sejam feitas tambem no ficheiro passageiros.txt
						printf("Foram gravadas as alteracoes\n");
					}
			}	
			else{//caso nao seja encontrado um passageiro com este numero
				printf("Não existe um passageiro com esse numero\n");
			}
				
		}
		 if(res==3){//alterar dados de um condutor
			int naorespondidas=0;
			printf("Numero estudante condutor?\n");
			fgets(tmp,100,stdin);
			num=atoi(tmp);
			pos=exists(num,1);
			if(pos!=-1){
				
				printf("Se desejar alterar um campo deve inserir o novo valor, senão deixar o espaço em branco para nao alterar\n");
				int id = shmget(0x82247,0,0);
				ShMem * estrutura = (ShMem *) shmat(id,0,0);
				int numCond = estrutura->numCondutores;
				Tcondutor * c = (Tcondutor * ) &(estrutura->inicio);
				
				int res = semop(idSem,&DOWN,1);
				exit_on_error(res,"LyftAdmin: semdown");	
				printf("Nome?\n");
				myfgets(str,50,stdin);
                if(strcmp(str,"")!=0){//alterar o nome	
					strcpy(c[pos].nome,str);
					printf("Foi alterado o nome\n");
				}
				else{
					printf("Nao foi alterado o nome\n");
					naorespondidas++;
				}
				printf("Turma?\n");
				myfgets(str,10,stdin);
                if(strcmp(str,"")!=0){//alterar a turma
					strcpy(c[pos].turma,str);
					printf("Foi alterada a turma\n");
				}
				else{
					printf("Nao foi alterada a turma\n");
					naorespondidas++;
				}
				printf("Num telemovel?\n");
				myfgets(str,15,stdin);
                if(strcmp(str,"")!=0){//alterar o num telemovel
					strcpy(c[pos].telemovel,str);
					printf("Foi alterado o num telemovel\n");	
				}
				else{
					printf("Nao foi alterado o num telemovel\n");
					naorespondidas++;
				}
				printf("Email?\n");
				myfgets(str,40,stdin);
                if(strcmp(str,"")!=0){//alterar email
					strcpy(c[pos].email,str);
					printf("Foi alterado o email\n");
				}
				else{
					printf("Nao foi alterado o email\n");
                    naorespondidas++;
				}
				printf("Tipo?\n");
				myfgets(str,20,stdin);
                if(strcmp(str,"")!=0){//alterar o tipo de veiculo
					strcpy(c[pos].tipo,str);	
					printf("Foi alterado o tipo\n");
				}
				else{
					printf("Nao foi alterado o tipo\n");
                    naorespondidas++;
				}
				printf("Marca?\n");
				myfgets(str,20,stdin);
                if(strcmp(str,"")!=0){//alterar a marca
					strcpy(c[pos].marca,str);
					printf("Foi alterada a marca\n");
				}
				else{
					printf("Nao foi alterada a marca\n");
                    naorespondidas++;
				}
				printf("Matricula?\n");
				myfgets(str,15,stdin);
                if(strcmp(str,"")!=0){//alterar a matricula
					strcpy(c[pos].matricula,str);
					printf("Foi alterada a matricula\n");	
				}
				else{
					printf("Nao foi alterada a matricula\n");
                    naorespondidas++;
				}
				printf("Activo?\n");
				myfgets(tmp,40,stdin);
                int activo=atoi(tmp);
                if(strcmp(tmp,"")!=0){//alterar ativo 
                    c[pos].activo=activo;
					printf("Foi alterado o ativo\n");
                }
                else{
					printf("Nao foi alterado o ativo\n");
                    naorespondidas++;
                }
				printf("Disponivel_desde?\n");
				myfgets(tmp,40,stdin);
                long disponivel_desde=atol(tmp);
                if(strcmp(tmp,"")!=0){//alterar o disponivel desde
                    c[pos].disponivel_desde=disponivel_desde;
					printf("Foi alterado o disponivel_desde\n");
                }
                else{
					printf("Nao foi alterado o disponivel_desde\n");
                    naorespondidas++;
                }
				printf("PID?\n");
				myfgets(tmp,40,stdin);
                int PID=atoi(tmp);
                if(strcmp(tmp,"")!=0){//alterar o pid
                    c[pos].PID=PID;
					printf("Foi alterado o PID\n");
                }
                else{
					printf("Nao foi alterado o PID\n");
                    naorespondidas++;
                }
				res= semop(idSem,&UP,1);
				exit_on_error(res,"LyftAdmin: semup");
				if(naorespondidas==10){//se nada for alterado nao vale a pena estar a atualizar o ficheiro condutores.txt
					printf("Nada foi alterado...\n");
				}
				else{
					escrever_condutores();//para atualizar as alteracoes feitas em memoria no ficheiro condutores.txt
					printf("Foram gravadas as alteracoes\n");
				}
			}
			else{//caso nao for encontrado um condutore com este numero
				printf("Não existe um condutor com esse numero\n");
			}
		}
    }
	exit(0);
}

int main(){
    printf("Pid do LyftAdmin:%d\n",getpid());
	
	start();//inicializa as memorias partilhadas se nao existirem. O programa fecha se esta funcao nao for bem sucedida

    signal(SIGUSR1, trata_sinal);//tratamento do sinal SIGUSR1
    signal(SIGTERM, trata_sinal);//tratamento do sinal SIGTERM
    signal(SIGALRM, trata_sinal);//tratamento do sinal SIGALARM
	
	cria_lyftadmin_pid();//guardar o pid do processo LyftAdmin2 no ficheiro lyftadmin.pid para ser utilizador pelo LyftTask

    alarm(ALARMTIMER);//primeiro sinal de alarm a ser enviado, os proximos seram enviados na funcao trata_sinal

    menu();// abertura do menu
}

