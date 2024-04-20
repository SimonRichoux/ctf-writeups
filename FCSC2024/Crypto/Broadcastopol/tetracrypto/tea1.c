// Copyright 2023, Midnight Blue.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// this code was taken and adapted from https://github.com/MidnightBlueLabs/TETRA_crypto

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "tea1.h"


const uint16_t g_awTea1LutA[8] = { 0xDA86, 0x85E9, 0x29B5, 0x2BC6, 0x8C6B, 0x974C, 0xC671, 0x93E2 };
const uint16_t g_awTea1LutB[8] = { 0x85D6, 0x791A, 0xE985, 0xC671, 0x2B9C, 0xEC92, 0xC62B, 0x9C47 };
const uint8_t g_abTea1Sbox[256] = {
    0x9B, 0xF8, 0x3B, 0x72, 0x75, 0x62, 0x88, 0x22, 0xFF, 0xA6, 0x10, 0x4D, 0xA9, 0x97, 0xC3, 0x7B,
    0x9F, 0x78, 0xF3, 0xB6, 0xA0, 0xCC, 0x17, 0xAB, 0x4A, 0x41, 0x8D, 0x89, 0x25, 0x87, 0xD3, 0xE3,
    0xCE, 0x47, 0x35, 0x2C, 0x6D, 0xFC, 0xE7, 0x6A, 0xB8, 0xB7, 0xFA, 0x8B, 0xCD, 0x74, 0xEE, 0x11,
    0x23, 0xDE, 0x39, 0x6C, 0x1E, 0x8E, 0xED, 0x30, 0x73, 0xBE, 0xBB, 0x91, 0xCA, 0x69, 0x60, 0x49,
    0x5F, 0xB9, 0xC0, 0x06, 0x34, 0x2A, 0x63, 0x4B, 0x90, 0x28, 0xAC, 0x50, 0xE4, 0x6F, 0x36, 0xB0,
    0xA4, 0xD2, 0xD4, 0x96, 0xD5, 0xC9, 0x66, 0x45, 0xC5, 0x55, 0xDD, 0xB2, 0xA1, 0xA8, 0xBF, 0x37,
    0x32, 0x2B, 0x3E, 0xB5, 0x5C, 0x54, 0x67, 0x92, 0x56, 0x4C, 0x20, 0x6B, 0x42, 0x9D, 0xA7, 0x58,
    0x0E, 0x52, 0x68, 0x95, 0x09, 0x7F, 0x59, 0x9C, 0x65, 0xB1, 0x64, 0x5E, 0x4F, 0xBA, 0x81, 0x1C,
    0xC2, 0x0C, 0x02, 0xB4, 0x31, 0x5B, 0xFD, 0x1D, 0x0A, 0xC8, 0x19, 0x8F, 0x83, 0x8A, 0xCF, 0x33,
    0x9E, 0x3A, 0x80, 0xF2, 0xF9, 0x76, 0x26, 0x44, 0xF1, 0xE2, 0xC4, 0xF5, 0xD6, 0x51, 0x46, 0x07,
    0x14, 0x61, 0xF4, 0xC1, 0x24, 0x7A, 0x94, 0x27, 0x00, 0xFB, 0x04, 0xDF, 0x1F, 0x93, 0x71, 0x53,
    0xEA, 0xD8, 0xBD, 0x3D, 0xD0, 0x79, 0xE6, 0x7E, 0x4E, 0x9A, 0xD7, 0x98, 0x1B, 0x05, 0xAE, 0x03,
    0xC7, 0xBC, 0x86, 0xDB, 0x84, 0xE8, 0xD1, 0xF7, 0x16, 0x21, 0x6E, 0xE5, 0xCB, 0xA3, 0x1A, 0xEC,
    0xA2, 0x7D, 0x18, 0x85, 0x48, 0xDA, 0xAA, 0xF0, 0x08, 0xC6, 0x40, 0xAD, 0x57, 0x0D, 0x29, 0x82,
    0x7C, 0xE9, 0x8C, 0xFE, 0xDC, 0x0F, 0x2D, 0x3C, 0x2E, 0xF6, 0x15, 0x2F, 0xAF, 0xE1, 0xEB, 0x3F,
    0x99, 0x43, 0x13, 0x0B, 0xE0, 0xA5, 0x12, 0x77, 0x5D, 0xB3, 0x38, 0xD9, 0xEF, 0x5A, 0x01, 0x70};
    
const uint8_t g_abTea1InvSbox[256] = {
    0xA8, 0xFE, 0x82, 0xBF, 0xAA, 0xBD, 0x43, 0x9F, 0xD8, 0x74, 0x88, 0xF3, 0x81, 0xDD, 0x70, 0xE5, 
    0x0A, 0x2F, 0xF6, 0xF2, 0xA0, 0xEA, 0xC8, 0x16, 0xD2, 0x8A, 0xCE, 0xBC, 0x7F, 0x87, 0x34, 0xAC, 
    0x6A, 0xC9, 0x07, 0x30, 0xA4, 0x1C, 0x96, 0xA7, 0x49, 0xDE, 0x45, 0x61, 0x23, 0xE6, 0xE8, 0xEB, 
    0x37, 0x84, 0x60, 0x8F, 0x44, 0x22, 0x4E, 0x5F, 0xFA, 0x32, 0x91, 0x02, 0xE7, 0xB3, 0x62, 0xEF, 
    0xDA, 0x19, 0x6C, 0xF1, 0x97, 0x57, 0x9E, 0x21, 0xD4, 0x3F, 0x18, 0x47, 0x69, 0x0B, 0xB8, 0x7C, 
    0x4B, 0x9D, 0x71, 0xAF, 0x65, 0x59, 0x68, 0xDC, 0x6F, 0x76, 0xFD, 0x85, 0x64, 0xF8, 0x7B, 0x40, 
    0x3E, 0xA1, 0x05, 0x46, 0x7A, 0x78, 0x56, 0x66, 0x72, 0x3D, 0x27, 0x6B, 0x33, 0x24, 0xCA, 0x4D, 
    0xFF, 0xAE, 0x03, 0x38, 0x2D, 0x04, 0x95, 0xF7, 0x11, 0xB5, 0xA5, 0x0F, 0xE0, 0xD1, 0xB7, 0x75, 
    0x92, 0x7E, 0xDF, 0x8C, 0xC4, 0xD3, 0xC2, 0x1D, 0x06, 0x1B, 0x8D, 0x2B, 0xE2, 0x1A, 0x35, 0x8B, 
    0x48, 0x3B, 0x67, 0xAD, 0xA6, 0x73, 0x53, 0x0D, 0xBB, 0xF0, 0xB9, 0x00, 0x77, 0x6D, 0x90, 0x10, 
    0x14, 0x5C, 0xD0, 0xCD, 0x50, 0xF5, 0x09, 0x6E, 0x5D, 0x0C, 0xD6, 0x17, 0x4A, 0xDB, 0xBE, 0xEC, 
    0x4F, 0x79, 0x5B, 0xF9, 0x83, 0x63, 0x13, 0x29, 0x28, 0x41, 0x7D, 0x3A, 0xC1, 0xB2, 0x39, 0x5E, 
    0x42, 0xA3, 0x80, 0x0E, 0x9A, 0x58, 0xD9, 0xC0, 0x89, 0x55, 0x3C, 0xCC, 0x15, 0x2C, 0x20, 0x8E, 
    0xB4, 0xC6, 0x51, 0x1E, 0x52, 0x54, 0x9C, 0xBA, 0xB1, 0xFB, 0xD5, 0xC3, 0xE4, 0x5A, 0x31, 0xAB, 
    0xF4, 0xED, 0x99, 0x1F, 0x4C, 0xCB, 0xB6, 0x26, 0xC5, 0xE1, 0xB0, 0xEE, 0xCF, 0x36, 0x2E, 0xFC, 
    0xD7, 0x98, 0x93, 0x12, 0xA2, 0x9B, 0xE9, 0xC7, 0x01, 0x94, 0x2A, 0xA9, 0x25, 0x86, 0xE3, 0x08
};


uint64_t tea1_expand_iv(uint32_t dwShortIv) {
    uint32_t dwXorred = dwShortIv ^ 0x96724FA1;
    dwXorred = (dwXorred << 8) | (dwXorred >> 24); // rotate left -> translated to single rol instruction
    uint64_t qwIv = ((uint64_t)dwShortIv << 32) | dwXorred;
    return (qwIv >> 8) | (qwIv << 56); // rotate right
}

uint8_t tea1_state_word_to_newbyte(uint16_t wSt, const uint16_t *awLut) {
    uint8_t bSt0 = wSt;
    uint8_t bSt1 = wSt >> 8;
    uint8_t bDist;
    uint8_t bOut = 0;

    for (int i = 0; i < 8; i++) {
        // taps on bit 7,0 for bSt0 and bit 1,2 for bSt1
        bDist = ((bSt0 >> 7) & 1) | ((bSt0 << 1) & 2) | ((bSt1 << 1) & 12);
        if (awLut[i] & (1 << bDist)) {
            bOut |= 1 << i;
        }

        // rotate one position
        bSt0 = ((bSt0 >> 1) | (bSt0 << 7));
        bSt1 = ((bSt1 >> 1) | (bSt1 << 7));
    }

    return bOut;
}

uint8_t tea1_reorder_state_byte(uint8_t bStByte) {
    // simple re-ordering of bits
    uint8_t bOut = 0;
    bOut |= ((bStByte << 6) & 0x40);
    bOut |= ((bStByte << 1) & 0x20);
    bOut |= ((bStByte << 2) & 0x08);
    bOut |= ((bStByte >> 3) & 0x14);
    bOut |= ((bStByte >> 2) & 0x01);
    bOut |= ((bStByte >> 5) & 0x02);
    bOut |= ((bStByte << 4) & 0x80);
    return bOut;
}

int32_t tea1_init_key_register(const uint8_t *lpKey) {
    int32_t dwResult = 0;
    for (int i = 0; i < 10; i++) {
        dwResult = (dwResult << 8) | g_abTea1Sbox[((dwResult >> 24) ^ lpKey[i] ^ dwResult) & 0xff];
    }
    return dwResult;
}

void tea1_inner(uint64_t qwIvReg, uint32_t dwKeyReg, uint32_t dwNumKsBytes, uint8_t *lpKsOut) {
    
    uint32_t dwNumSkipRounds = 54;
    // printf("\n    : dwKeyReg %08X qwIvReg %016lX\n", dwKeyReg, qwIvReg);
    for (int i = 0; i < dwNumKsBytes; i++) {
        for (int j = 0; j < dwNumSkipRounds; j++) {
            
            // Step 1: Derive a non-linear feedback byte through sbox and feed back into key register
            uint8_t bSboxOut = g_abTea1Sbox[((dwKeyReg >> 24) ^ dwKeyReg) & 0xff];
            dwKeyReg = (dwKeyReg << 8) | bSboxOut;

            // Step 2: Compute 3 bytes derived from current state
            uint8_t bDerivByte12 = tea1_state_word_to_newbyte((qwIvReg >>  8) & 0xffff, g_awTea1LutA);
            uint8_t bDerivByte56 = tea1_state_word_to_newbyte((qwIvReg >> 40) & 0xffff, g_awTea1LutB);
            uint8_t bReordByte4  = tea1_reorder_state_byte((qwIvReg >> 32) & 0xff);

            // Step 3: Combine current state with state derived values, and xor in key derived sbox output
            uint8_t bNewByte = (bDerivByte56 ^ (qwIvReg >> 56) ^ bReordByte4 ^ bSboxOut) & 0xff;
            uint8_t bMixByte = bDerivByte12;

            // Step 4: Update lfsr: leftshift 8, feed/mix in previously generated bytes
            qwIvReg = ((qwIvReg << 8) ^ ((uint64_t)bMixByte << 32)) | bNewByte;
            
            // printf("       %02X %02X %02X -> %02X %02X\n", bDerivByte12, bDerivByte56, bReordByte4, bNewByte, bMixByte);
            // printf("%2d %2d: dwKeyReg %08X qwIvReg %016lX\n", i, j, dwKeyReg, qwIvReg);
        }

        lpKsOut[i] = (qwIvReg >> 56);
        // printf("Generated %02X (proper)\n", lpKsOut[i]);
        dwNumSkipRounds = 19;
    }
}

int my_tea1_inner_target(uint64_t qwIvReg, uint32_t dwKeyReg, uint32_t dwNumKsBytes, uint8_t *target, uint8_t* F_2,  uint8_t* F_1,  uint8_t* B) {
    
    uint32_t dwNumSkipRounds = 54;
    // printf("\n    : dwKeyReg %08X qwIvReg %016lX\n", dwKeyReg, qwIvReg);
    for (int i = 0; i < dwNumKsBytes; i++) {
        for (int j = 0; j < dwNumSkipRounds; j++) {
            
            // Step 1: Derive a non-linear feedback byte through sbox and feed back into key register
            uint8_t bSboxOut = g_abTea1Sbox[((dwKeyReg >> 24) ^ dwKeyReg) & 0xff];
            dwKeyReg = (dwKeyReg << 8) | bSboxOut;

            // Step 2: Compute 3 bytes derived from current state
            uint8_t bDerivByte12 = F_2[(qwIvReg >>  8) & 0xffff];
            uint8_t bDerivByte56 = F_1[(qwIvReg >> 40) & 0xffff];
            uint8_t bReordByte4  = B[(qwIvReg >> 32) & 0xff];

            // Step 3: Combine current state with state derived values, and xor in key derived sbox output
            uint8_t bNewByte = (bDerivByte56 & 0xff) ^ ((qwIvReg >> 56) & 0xff) ^ (bReordByte4 & 0xff) ^ (bSboxOut & 0xff);
            uint8_t bMixByte = bDerivByte12;

            // Step 4: Update lfsr: leftshift 8, feed/mix in previously generated bytes
            qwIvReg = ((qwIvReg << 8) ^ ((uint64_t)bMixByte << 32)) | bNewByte;
            
            // printf("       %02X %02X %02X -> %02X %02X\n", bDerivByte12, bDerivByte56, bReordByte4, bNewByte, bMixByte);
            // printf("%2d %2d: dwKeyReg %08X qwIvReg %016lX\n", i, j, dwKeyReg, qwIvReg);
        }

        if (target[i] != (qwIvReg >> 56)){
            return 0;
        }
        // printf("Generated %02X (proper)\n", lpKsOut[i]);
        dwNumSkipRounds = 19;
    }
    return 1;
}

void print_tea_stream (uint64_t qwIvReg, uint32_t dwKeyReg, uint32_t dwNumKsBytes,  uint8_t* F_2,  uint8_t* F_1,  uint8_t* B){
    uint32_t dwNumSkipRounds = 54;
    // printf("\n    : dwKeyReg %08X qwIvReg %016lX\n", dwKeyReg, qwIvReg);
    for (int i = 0; i < dwNumKsBytes; i++) {
        for (int j = 0; j < dwNumSkipRounds; j++) {
            
            // Step 1: Derive a non-linear feedback byte through sbox and feed back into key register
            uint8_t bSboxOut = g_abTea1Sbox[((dwKeyReg >> 24) ^ dwKeyReg) & 0xff];
            dwKeyReg = (dwKeyReg << 8) | bSboxOut;

            // Step 2: Compute 3 bytes derived from current state
            uint8_t bDerivByte12 = F_2[(qwIvReg >>  8) & 0xffff];
            uint8_t bDerivByte56 = F_1[(qwIvReg >> 40) & 0xffff];
            uint8_t bReordByte4  = B[(qwIvReg >> 32) & 0xff];

            // Step 3: Combine current state with state derived values, and xor in key derived sbox output
            uint8_t bNewByte = (bDerivByte56 & 0xff) ^ ((qwIvReg >> 56) & 0xff) ^ (bReordByte4 & 0xff) ^ (bSboxOut & 0xff);
            uint8_t bMixByte = bDerivByte12;

            // Step 4: Update lfsr: leftshift 8, feed/mix in previously generated bytes
            qwIvReg = ((qwIvReg << 8) ^ ((uint64_t)bMixByte << 32)) | bNewByte;
            
            // printf("       %02X %02X %02X -> %02X %02X\n", bDerivByte12, bDerivByte56, bReordByte4, bNewByte, bMixByte);
            // printf("%2d %2d: dwKeyReg %08X qwIvReg %016lX\n", i, j, dwKeyReg, qwIvReg);
        }
        printf("%02X", (uint8_t)(qwIvReg >> 56));
        dwNumSkipRounds = 19;
    }
}

void tea1(uint32_t dwFrameNumbers, const uint8_t *lpKey, uint32_t dwNumKsBytes, uint8_t *lpKsOut) {
    
    // Initialize IV and key register
    uint64_t qwIvReg = tea1_expand_iv(dwFrameNumbers);
    uint32_t dwKeyReg = tea1_init_key_register(lpKey);

    // Invoke actual TEA1 core function
    tea1_inner(qwIvReg, dwKeyReg, dwNumKsBytes, lpKsOut);
}

int main(int argc, char* args[]){
    
    uint32_t key = 0;
    uint32_t iv = 0x0026b328;
    uint64_t qwIvReg = tea1_expand_iv(iv);
    
    // precomputations of non linear functions
    
    uint8_t* F_2 = (uint8_t*) malloc (sizeof(uint8_t)*65536);
    uint8_t* F_1 = (uint8_t*) malloc (sizeof(uint8_t)*65536);
    uint8_t* B = (uint8_t*) malloc (sizeof(uint8_t)*256);

    for (int i=0; i<65536; i++){
        F_2[i] = tea1_state_word_to_newbyte(i, g_awTea1LutA);
        F_1[i] = tea1_state_word_to_newbyte(i, g_awTea1LutB);
    }
    for (int i=0; i<256; i++){
        B[i] = tea1_reorder_state_byte(i);
    }

    uint8_t *target = (uint8_t[]){0xd2, 0x02, 0x97, 0x1e};
    for (long i=0; i<4294967296; i++){
        if (my_tea1_inner_target(qwIvReg, i, 4, target, F_2, F_1, B) == 1){
            key = i;
            break;
        }
    }
    // we found the key
    
    // the length of the encrypted file was precomputed -> 288812
    print_tea_stream(qwIvReg, key, 288812, F_2, F_1, B);

    free(F_2);
    free(F_1);
    free(B); 
}