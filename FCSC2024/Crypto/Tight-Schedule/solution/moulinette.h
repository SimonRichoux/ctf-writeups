#ifndef MOULINETTE_H
#define MOULINETTE_H

// Constants
const uint8_t A[16][16];
const uint8_t A_inv[16][16];
const uint8_t S[256];
const uint8_t RCON[11];

// Related to AES key schedule
void canonical_to_invariant (uint8_t input[16], uint8_t output[16]);
void invariant_to_canonical (uint8_t input[16], uint8_t output[16]);
void pi_i(uint8_t input[16], uint8_t output[16], int i);

// Related to tight schedule encryption algorithm
void xor(uint8_t x[16], uint8_t y[16]);
void round_function(uint8_t x[16], uint8_t cst);
void expandKey(uint8_t k[16], uint8_t rk[11][16]);
void encrypt(uint8_t input[16], uint8_t output[16], uint8_t rk [11][16]);

// utils
int compare(uint8_t a[16], uint8_t b[16]);
void print_vector(uint8_t vector[16]);

#endif