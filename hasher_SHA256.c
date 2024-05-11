/* --------- Compilation --------- */

// gcc hasher_SHA256.c -o hasher_SHA256 -lcrypto


/* ---------- Includes ----------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define ORANGE "\033[0;33m"
#define RED "\x1B[31m"
#define NO_COLOR "\033[0m"


/* ----------- SHA-256 ----------- */

void sha256(const unsigned char *str, size_t len, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    SHA256(str, len, hash);
}

/* -------- Main function -------- */

int main(int argc, char *argv[]) {

	// Comprobar número de parámetros
	if(argc != 2) {

		printf(RED "[ERROR]" NO_COLOR);
		printf(" Usage: %s <cadena>\n", argv[0]);
		return 1;
	}
	
	// Obetener hash
	unsigned char hash[SHA256_DIGEST_LENGTH];
	sha256((unsigned char *)argv[1], strlen(argv[1]), hash);
	
	// Mostrar hash
	printf(ORANGE "[SHA-256]: " NO_COLOR);

	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		printf("%02x", hash[i]);
	}
	
	printf("\n");
	
	return 0;
    
}

