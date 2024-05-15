// mpicc -o cracker_mpi cracker_mpi.c -lcrypto
// mpirun -np 8 ./cracker_mpi 283c49894a43c19acffae9055811b469f342cc1aaab9f2adc30febf354fd463d

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define NO_COLOR "\033[0m"
#define BLUE "\x1B[34m"

#define MAX_PASSWORD_LENGTH 6 // Longitud máxima de la contraseña
#define CHAR_SET_LENGTH 26    // Longitud del conjunto de caracteres del abecedario reducido 
//#define CHAR_SET_LENGTH 62    // Longitud del conjunto de caracteres del abecedario mayusculas, minusculas y numeros 

//char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // Conjunto de caracteres posibles
char charset[] = "abcdefghijklmnopqrstuvwxyz"; // Conjunto de caracteres posibles

int encontrado = 0;
int newencontrado = 0;

char* number_to_sequence(int number) {
    static char sequence[6] = {0}; // static array to store the resulting sequence

    for (int i = 0; number > 0; i++) {
        int pos = (number - 1) % CHAR_SET_LENGTH;
        sequence[i] = charset[pos];
        number = (number - pos - 1) / CHAR_SET_LENGTH;
    }

    return sequence;
}

unsigned long long potencia(int base, int exponente) {
    unsigned long long resultado = 1;
    for (int i = 0; i < exponente; i++) {
        resultado *= base;
    }
    return resultado;
}

unsigned long long calcularCombinaciones(int longitud) {
    unsigned long long combinaciones = 0;
    for (int i = 1; i <= longitud; i++) {
        combinaciones += potencia(CHAR_SET_LENGTH, i);
    }
    return combinaciones;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    MPI_Status status;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    if (argc != 2) {
        if (rank == 0) {
            printf(RED "[ERROR]" NO_COLOR);
            printf(" Usage: %s <cadena>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    char password[MAX_PASSWORD_LENGTH + 1];
    int length;

    char *hash_objetivo = argv[1];
    unsigned char hash_candidato[SHA256_DIGEST_LENGTH];
    unsigned char hash[SHA256_DIGEST_LENGTH];

    int total_passwords = calcularCombinaciones(MAX_PASSWORD_LENGTH);
    
    if (rank == 0) {
        printf(BLUE "[INFO] " NO_COLOR);
        printf("SHA-256 hash: %s \n", hash_objetivo);
        
        printf(BLUE "[INFO] " NO_COLOR);
        printf("Possible passwords combinations: %i \n", total_passwords);
	
        printf("Cracking password...\n");
    }

    // Calcular el rango de contraseñas para cada proceso
    int passwords_per_process = total_passwords / size;
    int start_index[size];
    int end_index[size];

    for (int i = 0; i < size; i++) {
        start_index[i] = rank * passwords_per_process;
        end_index[i] = start_index[i] + passwords_per_process - 1;
    }

    start_index[size - 1] += total_passwords % size;
    end_index[size - 1] = total_passwords;

    for (int idx = start_index[rank]; idx <= end_index[rank]; idx++) {

        char *local_password = number_to_sequence(idx);

        if (encontrado == 0){

            SHA256((unsigned char *)local_password, strlen(local_password), hash);

            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                sprintf(&hash_candidato[i * 2], "%02x", hash[i]);
            }
            
            if (strcmp(hash_objetivo, hash_candidato) == 0) {

                newencontrado = 1;
                MPI_Bcast(&newencontrado, 1, MPI_INT, 0, MPI_COMM_WORLD);

                end = clock();
                double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf(GREEN "[OK]" NO_COLOR);
                printf("Password: %s\n", local_password);
                
                printf(BLUE "[INFO] " NO_COLOR);
                printf("Time taken: %f \n\n\n", time_taken);
				
		freopen("/dev/null", "w", stderr); // No devolver nada
                MPI_Abort(MPI_COMM_WORLD, 0); // Parar todos los procesos

                
            }

        }else{
            printf("Password encontrado\n");
            break;
        }

        MPI_Allreduce(&encontrado, &newencontrado, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        encontrado = newencontrado;
    }

    MPI_Finalize();

    return 0;
}
