/* --------- Compilation --------- */

// mpicc -o cracker_mpi cracker_mpi.c -lcrypto

/* ---------- Execution ---------- */

// time mpirun -np 8 ./cracker_mpi 283c49894a43c19acffae9055811b469f342cc1aaab9f2adc30febf354fd463d

/* ---------- Includes ----------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <mpi.h>

/* ----------- Defines ----------- */

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define NO_COLOR "\033[0m"
#define BLUE "\x1B[34m"

#define MAX_PASSWORD_LENGTH 6 // Longitud máxima de la contraseña
#define CHAR_SET_LENGTH 36 // Longitud del conjunto de caracteres

/* ---------- Functions ---------- */

char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
int charset_index(char c);
int generate_next_password(char *password, int length);
unsigned long long potencia(int base, int exponente);
unsigned long long calcularCombinaciones(int longitud);
char* number_to_sequence(int number);

char* number_to_sequence(int number) {
    static char sequence[6] = {0};

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

/* ------------ Main ------------- */

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            printf(RED "[ERROR]" NO_COLOR);
            printf(" Usage: %s <cadena>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    char *hash_objetivo = argv[1];

    unsigned long long total_passwords = calcularCombinaciones(MAX_PASSWORD_LENGTH);
    unsigned long long passwords_per_process = total_passwords / size;

    char password[MAX_PASSWORD_LENGTH + 1];
    unsigned char hash_candidato[SHA256_DIGEST_LENGTH];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    if (rank == 0) {
        printf(BLUE "[INFO] " NO_COLOR);
        printf("SHA-256 hash: %s \n", hash_objetivo);
        
        printf(BLUE "[INFO] " NO_COLOR);
        printf("Possible passwords combinations: %lli \n", total_passwords);
	
        printf("Cracking password...\n");
    }

    int found = 0;

    for (unsigned long long idx = rank * passwords_per_process; idx < (rank + 1) * passwords_per_process; idx++) {
        if (found == 0) {
            char *local_password = number_to_sequence(idx);
            SHA256((unsigned char *)local_password, strlen(local_password), hash);

            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                sprintf(&hash_candidato[i * 2], "%02x", hash[i]);
            }

            if (strcmp(hash_objetivo, hash_candidato) == 0) {

                found = 1;
                
                if (rank == 0) {
                
                    printf(GREEN "[OK]: " NO_COLOR);
                    printf("Password: %s\n", local_password);
				
					freopen("/dev/null", "w", stderr); // No devolver nada
                	MPI_Abort(MPI_COMM_WORLD, 0); // Parar todos los procesos

                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}
