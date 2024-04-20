#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moulinette.h"

#define NBTHREADS 8

/* Related to AES key schedule */

const uint8_t A[16][16] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0},
        {0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},
        {0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
        {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},
        {0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0}
};

const uint8_t A_inv[16][16] = {
        {0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0},
        {0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,1},
        {0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0},
        {1,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0},
        {0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0},
        {1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
        {0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
        {1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

void canonical_to_invariant (uint8_t input[16], uint8_t output[16]){
    for (int i=0; i<16; i++)
        for (int j=0; j<16; j++)
            if (A[i][j] == 1)
                output[i] ^= input[j];
}

void invariant_to_canonical (uint8_t input[16], uint8_t output[16]){
    for (int i=0; i<16; i++)
        for (int j=0; j<16; j++)
            if(A_inv[i][j] == 1)
                output[i] ^= input[j];
}

void pi_i(uint8_t input[16], uint8_t output[16], int i){
    i = i%4;
    uint8_t inter[16] = {0};
    canonical_to_invariant(input, inter);
    for (int j=0; j<16; j++)
        if ((4*i > j) || (j >= 4*(i+1)))
            inter[j] = 0;
    invariant_to_canonical(inter, output);
}

void key_pi_i(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t output[16], int i){
    const uint8_t *temp;
    switch (i%4) {
        case 0: temp = (const uint8_t[]){d,c,b,a,0,c,0,a,d,0,0,a,0,0,0,a}; break;
        case 1: temp = (const uint8_t[]){c,b,a,d,c,0,a,0,0,0,a,d,0,0,a,0}; break;
        case 2: temp = (const uint8_t[]){b,a,d,c,0,a,0,c,0,a,d,0,0,a,0,0}; break;
        default: temp = (const uint8_t[]){a,d,c,b,a,0,c,0,a,d,0,0,a,0,0,0}; break;
    }
    memcpy(output, temp, 16);
}

/* Related to tight schedule encryption algorithm */

const uint8_t S[256] = {
            0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
            0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
            0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
            0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
            0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
            0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
            0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
            0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
            0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
            0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
            0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
            0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
            0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
            0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
            0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
            0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

const uint8_t RCON[11] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

void xor(uint8_t x[16], uint8_t y[16]){
    for (int i=0; i<16; i++)
        x[i] ^= y[i];
}

void xor_4_bytes(uint8_t* x, uint8_t* y){
    for (int i=0; i<4; i++)
        x[i] ^= y[i];
}

void round_function(uint8_t x[16], uint8_t cst) {
    uint8_t a = x[12];
    uint8_t b = x[13];
    uint8_t c = x[14];
    uint8_t d = x[15];
    uint8_t t[4] = {S[b] ^ cst, S[c], S[d], S[a]};
    xor_4_bytes(x, t);
    xor_4_bytes(&x[4], &x[0]);
    xor_4_bytes(&x[8], &x[4]);
    xor_4_bytes(&x[12], &x[8]);
}

void expandKey(uint8_t key[16], uint8_t rk [11][16]){
    memcpy(rk[0], key, 16);
    for (int i = 1; i < 11; i++) {
        memcpy(rk[i], rk[i - 1], 16);
        round_function(rk[i], RCON[i]);
    }
}

void encrypt(uint8_t input[16], uint8_t output[16], uint8_t rk[11][16]){
    memcpy(output, input, 16);
    for (int i=0; i<10; i++){
        xor(output, rk[i]);
        for (int j=0; j<5; j++)
            round_function(output, 0);
    }
    xor(output, rk[10]);
}

/* utils */

int compare(uint8_t a[16], uint8_t b[16]){
    for (int i=0; i<16; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

void print_vector(uint8_t vector[16]){
    for (int i = 0; i < 16; i++) 
        printf("%hhu%s", vector[i], (i < 15) ? ", " : "\n");
}

int main(int argc, char* args[]){

    uint8_t plaintext[16] = {13, 250, 76, 96, 82, 251, 135, 239, 10, 143, 3, 247, 5, 221, 81, 1};
    uint8_t ciphertext[16] = {212, 237, 25, 224, 105, 65, 1, 182, 177, 81, 225, 28, 45, 185, 115, 191};
    uint8_t final_key[16] = {0};

    uint8_t ciphertext_pi_[4][16] = {{0}};
    for (int i=0; i<4; i++)
       pi_i(ciphertext, ciphertext_pi_[i], i);
    
    #pragma omp parallel for num_threads(NBTHREADS) shared(final_key)
    for (short a=0; a<256; a++){
        for (short b=0; b<256; b++){
            for (short c=0; c<256; c++){
                for (short d=0; d<256; d++){

                    uint8_t output[16]= {0};
                    uint8_t output_pi_[4][16] = {{0}};
                    uint8_t rk [11][16] = {{0}};
                    uint8_t key[16] = {a^b^c^d,a^b^c^d,a^b^c^d,a^b^c^d,a^c,a^c,a^c,a^c,a^d,a^d,a^d,a^d,a,a,a,a};
                    expandKey(key, rk);
                    encrypt(plaintext, output, rk);
                    
                    for (int i=0; i<4; i++){
                        pi_i(output, output_pi_[i], i);
                        if (compare(output_pi_[i], ciphertext_pi_[i])){
                            printf("key %d found!\n", (i+2)%4);
                            uint8_t key_i[16]= {0};
                            key_pi_i(a, b, c, d, key_i, i+2);
                            #pragma omp critical
                            xor(final_key, key_i);
                        }
                    }
                }  
            }
        }
    }
    printf("final key:\n");
    print_vector(final_key);
}