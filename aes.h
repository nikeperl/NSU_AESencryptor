#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 16
#define NUM_ROUNDS 10

void keyExpansion(uint8_t key[BLOCK_SIZE], uint8_t roundKeys[NUM_ROUNDS + 1][BLOCK_SIZE]);
void encryptBlock(uint8_t block[BLOCK_SIZE], uint8_t roundKeys[NUM_ROUNDS + 1][BLOCK_SIZE]);
void decryptBlock(uint8_t block[BLOCK_SIZE], uint8_t roundKeys[NUM_ROUNDS + 1][BLOCK_SIZE]);