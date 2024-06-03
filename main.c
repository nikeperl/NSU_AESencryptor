#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "aes.h"

int main() {
    char inputFilename[256];
    char outputFilename[256];
    uint8_t key[BLOCK_SIZE];
    int operationChoice;

    // Ввод путей к файлам
    printf("Введите путь к входному файлу: ");
    scanf("%255s", inputFilename);
    printf("Введите путь к выходному файлу: ");
    scanf("%255s", outputFilename);

    // Ввод выбора операции
    printf("Выберите операцию (1 - шифрование, 2 - дешифрование): ");
    scanf("%d", &operationChoice);

    if (operationChoice == 1) {
        // Генерация ключа
        printf("Ваш ключ из %d байт: ", BLOCK_SIZE);
        srand(time(NULL)); // Устанавливаем в качестве базы текущее время
        for (int i = 0; i < BLOCK_SIZE; i++) {
            key[i] = rand() % 256;
            printf("%2x ", key[i]);
        }
    }
    else if (operationChoice == 2) {
        // Ввод ключа
        printf("Введите ключ из %d байт (1f c5 ...): ", BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            unsigned int byte;
            scanf("%2x", &byte);  // Ввод каждого байта в формате двух шестнадцатеричных цифр
            key[i] = (uint8_t)byte;
        }
    }
    else {
        fprintf(stderr, "Неверный выбор операции\n");
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

    // Массив ключей раунда
    uint8_t roundKeys[NUM_ROUNDS + 1][BLOCK_SIZE];

    // Генерация ключей раунда
    keyExpansion(key, roundKeys);

    uint8_t block[BLOCK_SIZE];
    size_t length;

    // Чтение и обработка файла по блокам
    while ((length = fread(block, 1, BLOCK_SIZE, inputFile)) > 0) {

        // Обработка блока данных
        if (operationChoice == 1) {
            // Если считано меньше, чем BLOCK_SIZE, зануляем оставшуюся часть блока
            if (length < BLOCK_SIZE) {
                memset(block + length, 0, BLOCK_SIZE - length);
                length = BLOCK_SIZE;
            }
            // Шифрование блока
            encryptBlock(block, roundKeys);
        } else if (operationChoice == 2) {
            // Дешифрование блока
            decryptBlock(block, roundKeys);
            // Если это последняя операция (дешифрование), удаляем нули
            while (length > 0 && block[length - 1] == 0) {
                length--;
            }
        } else {
            fprintf(stderr, "Неверный выбор операции\n");
            fclose(inputFile);
            fclose(outputFile);
            return EXIT_FAILURE;
        }

        // Запись обработанного блока в выходной файл
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
