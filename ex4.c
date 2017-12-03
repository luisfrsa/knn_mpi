#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>  
#include <time.h>   
#define TAG 1

int main(int argc, char *argv[]){
	int id, size,i,val,max,min, local_val;
	MPI_Init(&argc,&argv);
	MPI_Status st;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	srand(time(NULL)+id);//gerar um valor aleatório baseado no tempo e no id;
	local_val = rand()%1000;
	if(id != 0){ //se nao for o processo responsável por calcular o mínimo e o máximo
		// ele apenas envia seu valor e recebe o mínimo e o máximo
		MPI_Send(&local_val,1,MPI_INT,0,TAG,MPI_COMM_WORLD);
		MPI_Recv(&max,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);
		MPI_Recv(&min,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);
	}else{//caso seja o processo responsável
		max = min = local_val;
		printf("valor local %d = %d\n",id,local_val);
		for(i=1;i<size;i++){//para todos os processos da aplicação
			MPI_Recv(&val,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);//recebe valor
			printf("recebido %d do processador %d\n",val,i);
			if(val<min){//verifica se valor recebido é menor que o mínimo atual
				min = val;
			}else if(val > max){//verifica se valor recebido é maior que o máximo atual
				max = val;
			}
		}
		for(i=1;i<size;i++){//envia mínimo e máximo para todos processos
			MPI_Send(&max,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
			MPI_Send(&min,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
		}
	}//fim do else
	//todos processos mostram valor mínimo e máximo;
	printf("id: %d, Maior = %d e Menor = %d\n",id,max,min);
	MPI_Finalize();
	return 0;
}