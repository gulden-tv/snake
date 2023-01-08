#include "inc/control.h"
#include "inc/help.h"
#include "inc/snake_and_food.h"


int main() 
{
    char ch[] = "*";
    int x = 0, y = 0, key_pressed = 0;
    init(&snake1, 1, tail1, START_TAIL_SIZE); //Инициализация, хвост = 3
    init(&snake2, 2, tail2, START_TAIL_SIZE); //Инициализация, хвост = 3
    initFood(food1, MAX_FOOD_SIZE);
    initscr();            // Старт curses mod
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    printHelp("  Use arrows for control. Press 'q' for EXIT");
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    putFood(food1, SEED_NUMBER);// Кладем зерна
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    while (key_pressed != STOP_GAME) {
        key_pressed = getch(); // Считываем клавишу
        if (checkDirection(snake1.direction, key_pressed)) //Проверка корректности смены направления
        {
            changeDirection(&snake1.direction, key_pressed); // Меняем напарвление движения
        }
        autoChangeDirection(&snake2, food1, SEED_NUMBER);
        if (isCrash(&snake1))
            break;
        go(&snake1); // Рисуем новую голову
        goTail(&snake1); //Рисуем хвост
        go(&snake2); // Рисуем новую голову
        goTail(&snake2); //Рисуем хвост
        if (haveEat(&snake1, food1)) {
            addTail(&snake1);
            printLevel(&snake1);
        }
        if (haveEat(&snake2, food1)) {
            addTail(&snake2);
            printLevel(&snake2);
        }
        refreshFood(food1, SEED_NUMBER);// Обновляем еду
        repairSeed(food1, SEED_NUMBER, &snake1);
        blinkFood(food1, SEED_NUMBER);
        timeout(100); // Задержка при отрисовке
    }
    setColor(SNAKE1);
    printExit(&snake1);
    timeout(SPEED);
    getch();
    endwin(); // Завершаем режим curses mod

    return 0;
}
