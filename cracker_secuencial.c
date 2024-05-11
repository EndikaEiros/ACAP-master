/* --------- Compilation --------- */

// gcc cracker_secuencial.c -o cracker_secuencial -lcrypto


/* ---------- Includes ----------- */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

#define ORANGE "\033[0;33m"
#define RED "\x1B[31m"
#define NO_COLOR "\033[0m"

#define MAX_PASSWORD_LENGTH 6 // Longitud máxima de la contraseña
#define CHAR_SET_LENGTH 26 // Longitud del conjunto de caracteres

/* ----------- SHA-256 ----------- */

char charset[] = "abcdefghijklmnopqrstuvwxyz";
int charset_index(char c);
int generate_next_password(char *password, int length);

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

/* -------- Main function -------- */

int main(int argc, char *argv[]) {

	clock_t start, end;
	double cpu_time_used;
	
	start = clock();
	
	if (argc != 2) {
	
        printf(RED "[ERROR]" NO_COLOR);
		printf(" Usage: %s <cadena>\n", argv[0]);
		return 1;
	}

	int total_passwords = calcularCombinaciones(MAX_PASSWORD_LENGTH);

	printf("%i \n", total_passwords);

	char password[MAX_PASSWORD_LENGTH + 1]; 
 	int length;

	char *hash_objetivo = argv[1];
	unsigned char hash_candidato[SHA256_DIGEST_LENGTH];
	unsigned char hash[SHA256_DIGEST_LENGTH];

 	for (int idx = 0; idx <= total_passwords; idx++) {
		char *local_password = number_to_sequence(idx);

		if (encontrado == 0){

        	SHA256((unsigned char *)local_password, strlen(local_password), hash);

        	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
             sprintf(&hash_candidato[i * 2], "%02x", hash[i]);
        	}
            
        	if (strcmp(hash_objetivo, hash_candidato) == 0) {
				end = clock();
				encontrado = 1;
				double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
				printf(ORANGE "[PASSWORD]: " NO_COLOR);
				printf("%s\n", local_password);
				printf("Time taken: %f \n", time_taken);
				break;
			}
		}
	}
	return 0;
}

