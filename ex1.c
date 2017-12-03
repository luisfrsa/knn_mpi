#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	int i,id, size;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	printf("argumentos:\n");
	for (i=1;i<argc;i++)
		printf("argv[%d] = %s \n",i,argv[i]);
	printf("\n");
	printf("processo:%d dos %d inicializados\n",id,size);
	MPI_Finalize();
}