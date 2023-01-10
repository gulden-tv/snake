#include "inc/control.h"
#include "inc/help.h"
#include "inc/snake_and_food.h"
#include "inc/update_time.h"


int main() 
{
    int key_pressed = 0;
    init(&snake_user, SNAKE_USER_NUMBER, snake_user_tail, START_TAIL_SIZE); //Инициализация, хвост = 3
    init(&snake_robot, SNAKE_ROBOT_NUMBER, snake_robot_tail, START_TAIL_SIZE); //Инициализация, хвост = 3
    initFood(corn, MAX_FOOD_SIZE);
    initscr();            // Старт curses mod
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    printHelp("  Use arrows for control. Press 'q' for EXIT");
    start_color();
    init_pair(SNAKE_USER_NUMBER, COLOR_RED, COLOR_BLACK);
    init_pair(SNAKE_ROBOT_NUMBER, COLOR_BLUE, COLOR_BLACK);
    init_pair(FOOD_NUMBER, COLOR_GREEN, COLOR_BLACK);
    putFood(corn, SEED_NUMBER);// Кладем зерна
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    speedInit(&upd_time, UPDATE_TIME_INIT_VAL);
    while (key_pressed != STOP_GAME) {
        key_pressed = getch(); // Считываем клавишу
        if (checkDirection(snake_user.direction, key_pressed)) //Проверка корректности смены направления
        {
            changeDirection(&snake_user.direction, key_pressed); // Меняем напарвление движения
        }
        autoChangeDirection(&snake_robot, corn, SEED_NUMBER);
        if (isCrash(&snake_user))
            break;
        go(&snake_user); // Рисуем новую голову
        goTail(&snake_user); //Рисуем хвост
        go(&snake_robot); // Рисуем новую голову
        goTail(&snake_robot); //Рисуем хвост
        if (haveEat(&snake_user, corn)) {
            addTail(&snake_user);
            printLevel(&snake_user);
            speedUpdate(&upd_time, UPDATE_TIME_INKREMENT, UPDATE_TIME_MIN_VAL, UPDATE_TIME_MAX_VAL, UPDATE_TIME_CHANGE_CMD);
        }
        if (haveEat(&snake_robot, corn)) {
            addTail(&snake_robot);
            printLevel(&snake_robot);
            speedUpdate(&upd_time, UPDATE_TIME_DECREMENT, UPDATE_TIME_MIN_VAL, UPDATE_TIME_MAX_VAL, UPDATE_TIME_CHANGE_CMD);
        }
        refreshFood(corn, SEED_NUMBER);// Обновляем еду
        repairSeed(corn, SEED_NUMBER, &snake_user);
        blinkFood(corn, SEED_NUMBER);
        timeout(upd_time); // Задержка при отрисовке
    }
    setColor(SNAKE_USER_NUMBER);
    printExit(&snake_user);
    timeout(EXIT_TIME_OUT);
    getch();
    endwin(); // Завершаем режим curses mod

    return 0;
}
