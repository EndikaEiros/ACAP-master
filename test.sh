#!/bin/bash

hashes=(
	"5feceb66ffc86f38d952786c6d696c79c2dbc239dd4e91b46729d73a27fb57e9"
	"c6f3ac57944a531490cd39902d0f777715fd005efac9a30622d5f5205e7f6894"
	"a99a48bb1efa750c94fc5164cf763b3cfb730593cf37f8872a090d7f672b4af2"
	"8adce0a3431e8b11ef69e7f7765021d3ee0b70fff58e0480cadb4c468d78105f"
	"bf34af1dd7e7e920420e08fa4b6c509c9740b13c1ef22429547ce1e49198a83b"
	"916cdd0f7304cad37bb6a9060091ed8e2c1647493cd13eefefe0dfc41583b192"
)

numbers=(2 4 8)

echo "Running..."

run_command() {
    local hash=$1
    local n=$2
    printf "\n--------------------\n" >> mpi.txt 2>&1
    printf "\nHASH: $hash - np: $n\n" >> mpi.txt 2>&1
    #mpirun -np $n ./cracker_mpi $hash >> mpi.txt 2>&1
    { time mpirun -np $n ./cracker_mpi $hash; } >> mpi.txt 2>&1
}

for hash in "${hashes[@]}"; do
    for n in "${numbers[@]}"; do
        run_command $hash $n
    done
done

echo "[OK]"
