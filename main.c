#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "aes.h"

int main() {
    char inputFilename[256];
    char outputFilename[256];
    uint8_t key[BLOCK_SIZE];
    int operationChoice;

    // ���� ����� � ������
    printf("������� ���� � �������� �����: ");
    scanf("%255s", inputFilename);
    printf("������� ���� � ��������� �����: ");
    scanf("%255s", outputFilename);

    // ���� ������ ��������
    printf("�������� �������� (1 - ����������, 2 - ������������): ");
    scanf("%d", &operationChoice);

    if (operationChoice == 1) {
        // ��������� �����
        printf("��� ���� �� %d ����: ", BLOCK_SIZE);
        srand(time(NULL)); // ������������� � �������� ���� ������� �����
        for (int i = 0; i < BLOCK_SIZE; i++) {
            key[i] = rand() % 256;
            printf("%2x ", key[i]);
        }
    }
    else if (operationChoice == 2) {
        // ���� �����
        printf("������� ���� �� %d ���� (1f c5 ...): ", BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            unsigned int byte;
            scanf("%2x", &byte);  // ���� ������� ����� � ������� ���� ����������������� ����
            key[i] = (uint8_t)byte;
        }
    }
    else {
        fprintf(stderr, "�������� ����� ��������\n");
        return EXIT_FAILURE;
    }

    FILE* inputFile = fopen(inputFilename, "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "fopen input file");
        return EXIT_FAILURE;
    }

    FILE* outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL) {
        fprintf(stderr, "fopen output file");
        fclose(inputFile);
        return EXIT_FAILURE;
    }

    // ������ ������ ������
    uint8_t roundKeys[NUM_ROUNDS + 1][BLOCK_SIZE];

    // ��������� ������ ������
    keyExpansion(key, roundKeys);

    uint8_t block[BLOCK_SIZE];
    size_t length;

    // ������ � ��������� ����� �� ������
    while ((length = fread(block, 1, BLOCK_SIZE, inputFile)) > 0) {

        // ��������� ����� ������
        if (operationChoice == 1) {
            // ���� ������� ������, ��� BLOCK_SIZE, �������� ���������� ����� �����
            if (length < BLOCK_SIZE) {
                memset(block + length, 0, BLOCK_SIZE - length);
                length = BLOCK_SIZE;
            }
            // ���������� �����
            encryptBlock(block, roundKeys);
        } else if (operationChoice == 2) {
            // ������������ �����
            decryptBlock(block, roundKeys);
            // ���� ��� ��������� �������� (������������), ������� ����
            while (length > 0 && block[length - 1] == 0) {
                length--;
            }
        } else {
            fprintf(stderr, "�������� ����� ��������\n");
            fclose(inputFile);
            fclose(outputFile);
            return EXIT_FAILURE;
        }

        // ������ ������������� ����� � �������� ����
        if (fwrite(block, 1, length, outputFile) != length) {
            fprintf(stderr, "fwrite");
            fclose(inputFile);
            fclose(outputFile);
            return EXIT_FAILURE;
        }
    }

    if (ferror(inputFile)) {
        fprintf(stderr, "fread");
    }

    fclose(inputFile);
    fclose(outputFile);
    return EXIT_SUCCESS;
}