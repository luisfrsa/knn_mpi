#include <mpi.h>
#include <stdio.h>
#define TAG 1
int main(int argc, char *argv[]){
	MPI_Init(&argc,&argv);
	int id,val_enviado,val_recebido;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Status st;
	if(id==0){
		val_enviado=1000;
		MPI_Send(&val_enviado,1,MPI_INT,1,TAG,MPI_COMM_WORLD);
		MPI_Recv(&val_recebido,1,MPI_INT,1,TAG,MPI_COMM_WORLD,&st);
		printf("processo %d enviou %d e recebeu %d\n",
				id,val_enviado,val_recebido);
	}else if(id == 1){
		val_enviado=5000;
		MPI_Send(&val_enviado,1,MPI_INT,0,TAG,MPI_COMM_WORLD);
		MPI_Recv(&val_recebido,1,MPI_INT,0,TAG,MPI_COMM_WORLD,&st);
		printf("processo %d enviou %d e recebeu %d\n",
				id,val_enviado,val_recebido);

	}else{
		printf("processo %d nao recebeu msg\n",id);
	}
	MPI_Finalize();
}