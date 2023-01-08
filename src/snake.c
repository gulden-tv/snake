#include "../inc/snake.h"


tail tail1[MAX_TAIL_SIZE], tail2[MAX_TAIL_SIZE];
snake snake1, snake2;

//Инициализация хвоста змеи
void initTail(tail t[], size_t size) 
{
    tail init_t = {0, 0};
    for (size_t i = 0; i < size; i++) {
        t[i] = init_t;
    }
}

//Инициализация головы змеи
void initHead(snake *head) 
{
    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);
    head->x = (rand()) % (max_x - 1);
    head->y = (rand()) % (max_y - 10) + 1;
    head->direction = RIGHT;
}

//Инициализация змеи
void init(snake *head, int number, tail *tail, size_t size) 
{
    clear(); // очищаем весь экран
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head);
    head->number = number;
    head->tail = tail; // прикрепляем к голове хвост
    head->tsize = size + 1;
}

/*
 Движение головы с учетом текущего направления движения
*/
void go(snake *head) {
    setColor(head->number);
    char ch[] = "@";
    int max_x = 0, max_y = 0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    //clear(); // очищаем весь экран
    mvprintw(head->y, head->x, " "); // очищаем один символ
    switch (head->direction) {
        case LEFT:
            if (head->x <= 0) // Циклическое движение, что бы не
                // уходить за пределы экрана
                head->x = max_x;
            mvprintw(head->y, --(head->x), ch);
            break;
        case RIGHT:
            if (head->x >= max_x)
                head->x = 0;
            mvprintw(head->y, ++(head->x), ch);
            break;
        case UP:
            if (head->y <= 0)
                head->y = max_y;
            mvprintw(--(head->y), head->x, ch);
            break;
        case DOWN:
            if (head->y >= max_y)
                head->y = 0;
            mvprintw(++(head->y), head->x, ch);
            break;
        default:
            break;
    }
    refresh();
}

/*
  Движение хвоста с учетом движения головы
*/
void goTail(snake *head) 
{
    char ch[] = "*";
    setColor(head->number);
    mvprintw(head->tail[head->tsize - 1].y, head->tail[head->tsize - 1].x, " ");
    for (size_t i = head->tsize - 1; i > 0; i--) {
        head->tail[i] = head->tail[i - 1];
        if (head->tail[i].y || head->tail[i].x)
            
            mvprintw(head->tail[i].y, head->tail[i].x, ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

/*
  Увеличение хвоста на 1 элемент
*/
void addTail(snake *head) 
{
    if (head == NULL || head->tsize > MAX_TAIL_SIZE) {
        mvprintw(0, 0, "Can't add tail");
        return;
    }
    head->tsize++;
}

//Выводит на экран уровни игрока и змеи бота 
void printLevel(snake *head) 
{
    int max_x = 0, max_y = 0;
    getmaxyx(stdscr, max_y, max_x);
    if (head->number == SNAKE1){
        setColor(head->number);
        mvprintw(0, max_x - 10, "LEVEL: %d", head->tsize);
    }
    if (head->number == SNAKE2){
        setColor(head->number);
        mvprintw(1, max_x - 10, "LEVEL: %d", head->tsize);
    }    
}

//Выводит в конце игры уровень игрока
void printExit(snake *head) 
{
    int max_x = 0, max_y = 0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(max_y / 2, max_x / 2 - 5, "Your LEVEL is %d", head->tsize);
}

//Определяет врезалась ли голова змеи в хвост змеи
_Bool isCrash(snake *head) 
{
    for (size_t i = 1; i < head->tsize; i++)
        if (head->x == head->tail[i].x && head->y == head->tail[i].y)
            return 1;
    return 0;
}
