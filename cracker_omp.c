// gcc test.c -o test -lcrypto -fopenmp
// ./test

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include <omp.h>

#define ORANGE "\033[0;33m"
#define RED "\x1B[31m"
#define NO_COLOR "\033[0m"

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

            if (strcmp(argv[1], hash_candidato) == 0) {
                #pragma omp critical
                {
                    if (!encontrado) {  // Check again within the critical section
                        encontrado = 1;
                        printf(ORANGE "[PASSWORD]: " NO_COLOR "%s\n", local_password);
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