#pragma once

#define MAX_NUMBER_OF_RECORDS 7     // Количество сохраняемых рекордов
#define _ERROR -1                   // Возвращаемое значение в случае ошибки
#define _OK 0                       // Возвращаемое значение в случае успеха

struct hexSymbols {                 // структура для перевода HEX - строки в число (и обратно)

    char symbols[17];               // строка для перевода числа в HEX - строку (и наоборот)
    uint8_t bytesInNum;             // количество байт в числе number
    
    union {
        uint64_t number;            // число, переводимое в HEX - строку / расшифрованное из HEX - строки 
        uint8_t bytes[8];           // доступ к отдельным байтам числа number
    } un;
};

struct string {                     // структура для хранения информации о рекорде

    char date[11];                  // формат даты для вывода на экран (формат DD.MM.YYYY)
    char time[6];                   // формат времени для вывода на экран (формат XX:YY)
    uint8_t rec;                    // достигнутый уровень

    union {
        char str[21];               // сохраняемая в файле строка формата XXXXXXXXXXXXXXXX.YY.E (где X - время UTC, Y - рекорд, E - знак конца строки)
        struct {                    // структура для быстрого доступа к элементам строки 
            char date[16];          // шестнадцатеричное представление даты в формате time_t (16 символов);
            char dot_1;             // разделитель 1
            char level[2];          // шестнадцатиричное представление рекордного уровня (2 символа);
            char dot_2;             // разделитель 2
            char endOfStr;          // знак конца строки
        } st;
    } format;
};

/* вызов функции: выводящей на экран рекорд */
int recordsTable_begin (uint8_t newResult);

/* Функции чтения из файла */
int readRecordsFile(struct string * recordsTable, uint8_t newResult);
int decodingString (struct string * record);

/* Функции записи в файл */
void writeRecordsFile(struct string * recordsTable, int elements);

/* Функции вывода на экран */
void printRecordsTable (struct string * recordsTable, int elements, int isRec);

/* Функции сравнения и сортировки */
int isRecord(struct string * recordsTable, uint8_t newResult);
void sortResults(struct string * recordsTable, uint8_t newResult, uint8_t position);

/* Функции записи в строку формата string.format.str */
void toHex (struct hexSymbols * st);
void writeDate (struct string * record);
void writeRecord (struct string * record, uint8_t newRec);
void getDoneString (struct string * record, uint8_t newRec);
void returnTemplate (struct string * record);

/* Функции чтения из строки формата string.format.str */
void fromHex (struct hexSymbols * st);
void readDate (struct string * record, struct hexSymbols * date);
void readRecord (struct string * record);

