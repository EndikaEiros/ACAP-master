// gcc cracker_omp.c -o cracker_omp -lcrypto -fopenmp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include <omp.h>

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define NO_COLOR "\033[0m"
#define BLUE "\x1B[34m"

#define MAX_PASSWORD_LENGTH 6
#define CHAR_SET_LENGTH 26

char charset[] = "abcdefghijklmnopqrstuvwxyz";

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

char* number_to_sequence(int number, char* sequence) {
    int index = 0;
    do {
        sequence[index++] = charset[(number - 1) % CHAR_SET_LENGTH];
        number = (number - 1) / CHAR_SET_LENGTH;
    } while (number > 0);
    sequence[index] = '\0'; // Null-terminate the sequence
    return sequence;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, RED "[ERROR]" NO_COLOR " Usage: %s <hash>\n", argv[0]);
        return 1;
    }

    int total_passwords = calcularCombinaciones(MAX_PASSWORD_LENGTH);
    volatile int encontrado = 0;
    
    char *hash_objetivo = argv[1];
        
    printf(BLUE "[INFO] " NO_COLOR);
    printf("SHA-256 hash: %s \n", hash_objetivo);
        
    printf(BLUE "[INFO] " NO_COLOR);
    printf("Possible passwords combinations: %i \n", total_passwords);
	
    printf("Cracking password...\n");

    #pragma omp parallel shared(encontrado)
    {
        char local_password[MAX_PASSWORD_LENGTH + 1] = {0};
        char hash_candidato[2 * SHA256_DIGEST_LENGTH + 1];
        unsigned char hash[SHA256_DIGEST_LENGTH];

        #pragma omp for nowait
        for (int idx = 1; idx <= total_passwords; idx++) {
            if (encontrado) continue; // Skip loop iteration if password is found

            number_to_sequence(idx, local_password);
            SHA256((unsigned char*)local_password, strlen(local_password), hash);
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                sprintf(&hash_candidato[i * 2], "%02x", hash[i]);
            }

            if (strcmp(hash_objetivo, hash_candidato) == 0) {
                #pragma omp critical
                {
                    if (!encontrado) {  // Check again within the critical section
                    
                        end = clock();
                        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
                    
                        encontrado = 1;
                        printf(GREEN "[OK]: " NO_COLOR);
			printf("Password: %s\n", local_password);
				
                    }
                }
            }
        }
    }

    if (!encontrado) {
        printf("Password not found.\n");
    }

    return 0;
}
