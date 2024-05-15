#!/bin/bash

hashes=(
    "6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b"
    "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb" 
    "e3b98a4da31a127d4bde6e43033f66ba274cab0eb7eb1c70ec41402bf6273dd8"
)

numbers=(2 4 8)

run_command() {
    local hash=$1
    local n=$2
    printf "\nHASH: $hash - np: $n" >> mpi.txt 2>&1
    mpirun -np $n ./cracker_mpi $hash >> mpi.txt 2>&1 &
    { time mpirun -np $n ./cracker_mpi $hash; } >> mpi.txt 2>&1
}

for hash in "${hashes[@]}"; do
    for n in "${numbers[@]}"; do
        run_command $hash $n
    done
done

echo "[OK]"

