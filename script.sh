#!/bin/bash
declare -a array_data=("59" "154" "256" "1379" "1601")
#declare -a array_data=("59")
declare -a array_mpi=("1" "2" "4" "8" "10" "15" "20")
for data in "${array_data[@]}"
do
	for mpi in "${array_mpi[@]}"
	do
    	echo "----Relizando teste com entrada "$data " e com " $mpi " mpi ";
		(time mpirun -np $mpi trab $data) &> saida/saida_data_.$data._mpi_.$mpi.txt;

	done
done