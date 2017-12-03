#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define CENTRAL 0
#define TAG 1
int main(int argc, char *argv[]){
	MPI_Init(&argc,&argv);
	int i,min,max,x,id,size,rec;
	MPI_Status st;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	srand(time(NULL)+id);
	x=rand()%100;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if(id == CENTRAL){
		min=max=x;
		printf("processo central %d",x);
		for(i=0;i<size;i++){
			if(i!=CENTRAL){
				MPI_Recv(&rec,1,MPI_INT,i,TAG,
							MPI_COMM_WORLD,&st);
				printf("recebido %d do processo %d\n",rec,i);
				if(rec > max){
					max = rec;
				}else if (rec < min){
					min = rec;
				}
			}
		}
		for(i=0;i<size;i++){
			if(i!=CENTRAL){
				MPI_Send(&max,1,MPI_INT,i,TAG,
							MPI_COMM_WORLD);
				MPI_Send(&min,1,MPI_INT,i,TAG,
							MPI_COMM_WORLD);
				
			}
		}
	}else{
		MPI_Send(&x,1,MPI_INT,CENTRAL,TAG,MPI_COMM_WORLD);
		MPI_Recv(&max,1,MPI_INT,CENTRAL,TAG,MPI_COMM_WORLD,&st);
		MPI_Recv(&min,1,MPI_INT,CENTRAL,TAG,MPI_COMM_WORLD,&st);
	}
	printf("id:%d min:%d max%d\n",id,min,max);
	MPI_Finalize();
}
