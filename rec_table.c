
#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <time.h>

#include "rec_table.h"


/* Функция, запускающая работу с таблицей рекордов. Возвращает 0 в случае успеха и -1 в случае ошибки */

int recordsTable_begin (uint8_t newResult) {

    struct string recordsTable[MAX_NUMBER_OF_RECORDS] = { 0 };                  //  массив структур для записи значений

    int strNum = 0;                                                             // переменная для хранения количества записанных строк в массиве структур recordsTable
    int isRec = -1;                                                             // переменная для записи положения результата в таблице рекордов или -1 если результат не попал в таблицу


    if ( (strNum = readRecordsFile(recordsTable, newResult)) == _ERROR) {       // читаем строки из файла и записываем в массив строк

        return _ERROR;                                                          // в случае ошибки открытия файла возвращаем -1
    }  

    isRec = isRecord(recordsTable, newResult);                                  // проверяем результат на наличие рекорда, сортируем и возвращаем его номер в массиве структур
    
    if ( (isRec >= 0) && (strNum < MAX_NUMBER_OF_RECORDS)) {                    // если новый результат попадает в таблицу рекордов и в файле записано меньше максимального кол-ва рекордов

        strNum += 1;
    }

    printRecordsTable (recordsTable, strNum, isRec);                            // вывод на экран таблицы 
    
    if (isRec > -1) {                                                           // если имел место новый рекорд

        writeRecordsFile(recordsTable, strNum);                                 // перезаписываем файл
    }

    
    return _OK;
} 

/*__________________________Функции чтения из файла___________________________________*/
/* Функция открывает файл и записывает в массив структур recordsTable данные из файла
Возвращает: 
    1. 0, если файл пустой;
    2. Количество прочитанных строк, если структура успешно записана;
    3. -1 в случае ошибки открытия файла

 */
int readRecordsFile(struct string * recordsTable, uint8_t newResult) {

    int strNum = 0;                                                             // индекс элемента в массиве строк recordsTable
    FILE *f;                                                                    
    
    f = fopen("table.txt", "a+");                                               // открываем/ создаем файл

    if (f == NULL) {                                                            // проверка указателя

        return _ERROR;
    }

    while ( (fread(recordsTable[strNum].format.str, sizeof(char), 21, f) == 21) ) {       // записываем строку из файла           
                                                                                
        decodingString(&recordsTable[strNum]);                                  // расшифровываем строку

         if ( strNum == (MAX_NUMBER_OF_RECORDS - 1)) break;                     // выходим из цикла, если достигли последнего элемента в массиве структур

        strNum++;                                                               // увеличиваем номер элемента
    }

    fclose(f);                                                                  // закрываем файл
    return strNum;   
}
/* Функция расшифровывает строку ("вытаскивает" дату, время и рекорд)*/
int decodingString (struct string * record) {

    struct hexSymbols date;                                                     // структура для расшифровки времени в формате UTC
        date.un.number = 0;
        date.bytesInNum = 8;

    for (int i = 0; i < 16;  i++) {                                             // счетчик индексов

        date.symbols[i] = record->format.st.date[i];                            //  копируем значение времени в формате UTF в структуру date 
    }

    fromHex(&date);                                                             // расшифровываем UTC - время

    readDate (record, (struct hexSymbols *) &date);                             // Записываем строки с датой и временем в record->date и record->time
    readRecord (record);                                                        // расшифровываем значение рекорда и записываем его в record->rec

    return;
}

/*____________________________Функции записи в файл___________________________________*/
/* Функция записывает подготовленную строку в файл */
void writeRecordsFile(struct string * recordsTable, int elements) {
    
    FILE *f;                                                                    
    
    f = fopen("table.txt", "w+");                                              // перезаписываем файл

    if (f == NULL) {                                                           // проверка указателя

        return;                                                         
    }

    for (int i = 0; i < elements; i++) {                                        // счетчик элементов массива структур 

        //int ctr = 0;
        //ctr = 
        fwrite(&recordsTable[i].format.str[0], sizeof(char), 21, f);            // записываем подготовленную строку в файл
    }
    
    fclose(f);                                                                  // закрываем файл
    
    return;
}



/*____________________________Функции вывода на экран_________________________________*/
/*  Функция выводит таблицу с результатами на экран */
void printRecordsTable (struct string * recordsTable, int elements, int isRec) {

    clear();                                                                    // очищаем экран

    move(0, 0);                                                                 // переходим в верхнюю левую строку
    printw("Highest scores:\n\n");                                              
    printw("| RESULT   |   DATE\t| TIME  |\n");                                 // Шапка таблицы
    
    for (int i = 0; i < elements; i++) {                                        // счетчик индексов массива структур recordsTable

        printw("|    %d\t", recordsTable[i].rec);                               // вывод на экран рекорда 
        printw("   | %s\t", recordsTable[i].date);                              // вывод на экран даты рекорда 
        printw("| %s |", recordsTable[i].time);                                 // вывод на экран времени рекорда 

        if  (i == isRec) {                                                      // если текущий результат попал в таблицу рекордов

            printw("\t<---- YOUR RESULT");                                      // говорим пользователю о том, что текущий результат попал в таблицу рекордов

        }

        printw("\n");                                                           // переводим строку после запис очередного рекорда
    }
    refresh();                                                                  // обновляем
    getch();                                                                    // ждем ввода любого символа
    
    return;
}


/*_______________________ Функции сравнения и сортировки _____________________________*/
/* 
Функция сравнения текущего результата и результатов из таблицы рекордов.
Если текущее значение попадает в таблицу рекордов, массив структур и новый рекорд
отправляются в функцию сортировки.

Возвращает номер рекорда, если была сделана сортировка (текущий результат попал в таблицу рекордов)
Возвращает -1, если если новый результат не попал в таблицу рекордов
*/

int isRecord(struct string * recordsTable, uint8_t newResult) {                 

    for (int i = 0; i < MAX_NUMBER_OF_RECORDS; i++) {                           // счетчик элементов массива структур

        if (newResult > recordsTable[i].rec) {                                  // если текущий результат попадает в таблицу рекордов 

            sortResults(recordsTable, newResult, i);                            // отправляем на сортировку
            return i;                                                           // возвращаем индекс стрктуры с новым рекордом
        }  
    }
                        
    return -1;
}

/* Функция заполняет структуру для нового рекорда и добавляет его в массив структур */
void sortResults(struct string * recordsTable, uint8_t newResult, uint8_t position) {

    struct string newRecord = {0};                                              // структура для нового рекорда
        returnTemplate (&newRecord);                                            // добавляем разделители и знак конца строки
        getDoneString (&newRecord, newResult);                                  // заполняем строку

     for (int i = (MAX_NUMBER_OF_RECORDS - 1); i > position; i--) {             // счетчик индексов элементов в структуре (с последнего до следующего за индексом, в который вставим новый рекорд)

        recordsTable[i] =  recordsTable[i - 1];                                 // сдвигаем элементы структуры вправо
     }

     recordsTable[position] = newRecord;                                        // добавляем новый рекорд  в структуру

}
/*________________ Функции записи в строку формата string.format.str  ________________*/

/* Функция добавляет разделители в строку string.format.str */
void returnTemplate (struct string * record) {

        record->format.st.dot_1 = record->format.st.dot_2 = '.';
        record->format.st.endOfStr = '\n';
}
/* Функция добавляет инсорфмацию о дате/времени в строку string.format.str */
void getDoneString (struct string * record, uint8_t newRec) {
    
        writeDate(record);
        writeRecord(record, newRec);
}
/* Функция добавляет информацию о дате в структуру string.format.str */
void writeDate (struct string * record) {
    
    struct timespec ts;                                                     // структура для хранения информации о дате
        timespec_get(&ts, (int) 1);                                         // заполняем структуру
    
    struct hexSymbols dateToHex;                                            // структура для перевода числа в HEX - строку
        dateToHex.bytesInNum = sizeof(time_t);                              // указываем количество байт в числа
        dateToHex.un.number = (uint64_t) ts.tv_sec;                         // добавляем число, соответствующее полученному времени в формате UTC
    
    toHex(&dateToHex);                                                      // передаем структуру в функцию для расшифровки UTC - времени и формирования строки
    
    for(int i = 0; i < 16; i++) {                                           // счетчик индексов для элементов массивов 
        
        record->format.st.date[i] = dateToHex.symbols[i];                   // копируем информацию о дате в структуру 
    }

    readDate (record, (struct hexSymbols *) &dateToHex);                    // расшифровываем значения времени и даты для вывода нв экран
}
/* Функция записывает рекорд в строку string.format.str */
void writeRecord (struct string * record, uint8_t newRec) {
    
    record->rec = newRec;                                                   // добавляем значение рекорда
    
    struct hexSymbols levelToHex;                                           // структура для преобразования рекорда в HEX - строку
        levelToHex.bytesInNum = 1;
        levelToHex.un.number = (uint64_t) newRec;
    
    toHex(&levelToHex);                                                     // преобразовываем рекорд в HEX - строку
        
        record->format.st.level[0] = levelToHex.symbols[14];                // записываем первый полубайт в строку
        record->format.st.level[1] = levelToHex.symbols[15];                // записываем второй полубайт в строку
}
/* перевод числа в HEX - строку */
void toHex (struct hexSymbols * st) {

    uint8_t mask = 0x0F;                                                    // маска для чтения младшего полубайта
    int positionInStr = 15;                                                 // переменная с номером символа в строке st->symbols 

    char hexNum[16] =   {   '0', '1', '2', '3',
                            '4', '5', '6', '7',
                            '8', '9', 'A', 'B',
                            'C', 'D', 'E', 'F'
                        };                                                  // массив, в котором номеру элемента соответствует его цифра в HEX
    
    for (int i = 0; i < 16; i++) {                                          // заполняем строку нулями

        st->symbols[i] = '0';
    }

    st->symbols[16] = '\0';                                                 // добавляем знак конца строки


    for (int byteNum = 0; byteNum < (st->bytesInNum); byteNum++) {                                  // счетчик байтов

       if (positionInStr >= 0) {                                                                    // защита от записи в отрицательную ячейку массива
           
            st->symbols[positionInStr--] = hexNum[  (st->un.bytes[byteNum] & mask) ];               // записываем младший полубайт
       } 
       
        if (positionInStr >= 0) {                                                                   // защита от записи в отрицательную ячейку массива
           
            st->symbols[positionInStr--] = hexNum[ (( st->un.bytes[byteNum] >> 4) & mask) ];        // записываем  старший полубайт
       
        } else {
           
           break;
       }
    }
}
/*________________ Функции чтения из строки формата string.format.str  ________________*/

/* Функция расшифровывает дату в формате size_t и записывает ее в строку string.format.st[] */
void readDate (struct string * record, struct hexSymbols * date ) {

    char getDate[11] = { 0 };     // формат DD.MM.YYYY
    char getTime[6] = { 0 };     // формат XX:YY 

    time_t timeUTC = (time_t) date->un.number;                              // берем из структуры информацию о времени в формате UTC
    struct tm * timeStruct = localtime(&timeUTC);                           // "расшифровываем" время из формата UTC

     if (timeStruct != NULL) {                                              // если функция localtime вернула указатель
            
            strftime(getDate, (size_t) 11, "%d.%m.%Y", timeStruct);         // записываем данные о дате в строку
            strftime(getTime, (size_t) 6, "%H:%M", timeStruct);             // записываем данные о времени в строку
     }

     for (int i = 0; i < 11; i++) {                                         // записываем в строку date структуры string значение даты

        record->date[i] = getDate[i];
     }

    for (int i = 0; i < 6; i++) {                                           // записываем в строку time структуры string значение времени 

        record->time[i] = getTime[i];
     }
}
/*
Функция расшифровывает значение рекорда из строки string.format.st[] и добавляет значение в целочисленную переменную  
string.record для сравнения и вывода на экран
*/
void readRecord (struct string * record) {

    struct hexSymbols recordFromHex  = { 0 };                               // создаем структуру для расшифровки рекорда
        recordFromHex.bytesInNum = 2;                                       // указываем, что расшифровываемое число размером в 1 байт
        recordFromHex.symbols[14] = record->format.st.level[0];             // добавляем HEX - символы для расшифровки
        recordFromHex.symbols[15] = record->format.st.level[1];             // добавляем HEX - символы для расшифровки

    fromHex( (struct hexSymbols *) &recordFromHex);                         // отправляем HEX - символы для расшифровки

    record->rec = recordFromHex.un.bytes[0];                                // записываем расшифрованное значение рекорда 
}
/* функция расшифроки HEX - строки */
void fromHex (struct hexSymbols * st) {

    int ctr = 15;                                                           // переменная, хранящая номер текущего символа в строке st->symbols
    int byteNum = 0;                                                        // переменная, хранящая номер текущего элемента массива bytes
    uint8_t Nibbles[2] = { 0 };                                             // массив, хранящий младший и старший полубайты однобайтного числа
    int elem = 0;                                                           // счетчик элементов массива Nibbles
    
    if (st->bytesInNum <= 16) {                                             // проверка значения bytesInNum для избежания выхода за пределы массива  
        
        int ctr = st->bytesInNum - 1;                                       // записываем порядковый номер последнего символа 
    
    } else {
        
        return;                                                             // возврат, если bytesInNum больше числа элементов массива
    }

    for (ctr; ctr >= 0; ctr--) {                                            // счетчик элементов строки

        if ( (st->symbols[ctr] >= '0') && (st->symbols[ctr] <= '9') ) {     // если прочитан символ '0' - '9'

            Nibbles[elem++] = st->symbols[ctr] - '0';                       // записывает значение полубайта в массив 
        }

        if ( (st->symbols[ctr] >= 'A') && (st->symbols[ctr] <= 'F') ) {     // если прочитан символ 'A' - 'F'

            Nibbles[elem++] = (st->symbols[ctr] - '7');                     // записывает значение полубайта в массив
        }

        if ( (elem == 2) && (byteNum < 16) ) {                              // если прочитали оба полубайта и не вышли за пределы элементов строки
                
                st->un.bytes[byteNum++] = (Nibbles[1] << 4) + Nibbles[0];   // записываем в массив значение байта
                elem = 0;                                                   // обнуляем количество элементов
        } 
    }
}