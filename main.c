/*
 * Для компиляции необходимо добавить ключ -lncurses
 * gcc -o snake main.c -lncurses
 Vallen-snake
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <inttypes.h>
#include <wchar.h>

#define Q_SNAKE 2 // количество змей на поле

#define LOGOWIDTH 83
#define LOGOHEIGHT 25

char logo[][83] = {
	"                       ppBBBBBBBBBBBBmp",
	"                     pBBBBBBBBBBBBBBBBBBBBBBBBBNNpp",
	"                   pBBBBBBBBBNMPPPMNBBBBBBBBBBBBBBBBNp",
	"                  BBBBBBBN*           -*NBBBBBBppJNBBBBp",
	"                 BBBBBBB                  BBBBBBBBBBBBBBBN",
	"                jBBBBBB                    -**PPPPMNBBBBBB",
	"                jBBBBBB                                   K",
	"                 BBBBBBK                                  Nmpp*",
	"                 JBBBBBBN-                                    -",
	"                  *BBBBBBBBpp                          -ppppmNBBBBBBBBBBBBBNp",
	"                    *BBBBBBBBBBBBBNmpppppppppppNBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBN",
	"                       *NBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBNMMMMMBBBBBBBBBBK",
	"      ppBBBBBBBBBBNmpppp   -***MNBBBBBBBBBBBBBBBBNNMP***-             8BBBBBBBBK",
   	"pBBBBBBBBBBBBBBBBBBBBBBBBBBBppppp                            pppNBBBBBBBBBBB-",
	" JBBBBBBBN*********MNNBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBP",
	" 1BBBBBBB              Jpp--**PNNBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBNP*",
	"  NBBBBBBBBNppp        BBBBN      pmpp-  -7JI****PMMMMMMMMMMPP****--JppBBBNmp",
	"    *NBBBBBBBBBBBBBNp  jBBBBBN   -BBBBN JBBBBBBN    pBBBN    pBBBBpBBBBBBBBBBBB",
	"        -**MNBBBBBBBBBBpBBBBBBB-  BBBBB-BBBBPBBBBp jBBBBB  pBBBBBBBBBBP    -BBBB",
	"C              *MBBBBBBBBBBBBBBBp BBBBBBBBBP  BBBBpjBBBBBBBBBBBP-8BBBBBBBBBBBBBB",
	" Bp               BBBBBBBBBB-*BBBBBBBBBBBBBBNBBBBBBpBBBBBBBBBBC  BBBBBB*******-",
	"  BBNp            8BBBBBNBBP  -BBBBBBBBBBBBP**BBBBBBBBBBB-*BBBBBpjBBBBN",
	"   *BBBBBmppppppBBBBBBBPjBK    -BBBBBBBBBBB    BBBBBBBBBB   BBBBBBABBBBBNp - ppB",
	"      *NBBBBBBBBBBBBP*  4N      -BBBBBBBBBK    jBBBBNBBBBP   *BBBBP *BBBBBBBBBP",
	"           --  * -                -*- --*       -* - -**-      - -      ***-" };

enum {
    LEFT = 1, UP, RIGHT, DOWN, STOP_GAME = 'q'
};
enum {
    MAX_TAIL_SIZE = 250,
    START_TAIL_SIZE = 3,
    MAX_FOOD_SIZE = 20,
    FOOD_EXPIRE_SECONDS = 10,
    SPEED = 20000,
    SEED_NUMBER = 3
};

enum{
    SNAKE1 = 1,
    SNAKE2 = 2,
    FOOD = 3
};

// Структура хвоста
typedef struct __tail{
    int x;  // положение по оси X
    int y;  // положение по оси Y
} _tail;

// Струтура, описывающая еду
struct food {
    int x;  // положение по оси X
    int y;  // положение по оси Y
    time_t put_time;    // время когда данная точка была установлена
    char point; // внешний вид точки ('$','E'...)
    uint8_t enable; // Флаг, была ли точка съедена
} food[MAX_FOOD_SIZE];

// Структура головы
typedef struct __snake {
    int number; // номер змеи
    int x;  // положение по оси X
    int y;  // положение по оси Y
    int direction;  // направление движения
    unsigned char tsize; // длина хвоста
    _tail tail[MAX_TAIL_SIZE];    // массив хвоста
    struct __snake *next; // указатель на следующую змею    
} _snake;

struct _display {
    unsigned char width;    // ширина экрана
    unsigned char height;   // высота экрана
    unsigned char posYHelp; // позиция по оси Y строки помощи
    unsigned char posYStat; // позиция по оси Y строки статистики
} display;

// ========= Functions prototype ==============================================

unsigned char initHead(_snake *head);
_snake * addSnake(_snake *head);
unsigned char delSnake(_snake *head, unsigned char nSnake);
unsigned char delAllSnake(_snake *head);
unsigned char getNumSnake(_snake *head);
void clearPosSnake(_snake *head);
void printLogo(void);
void setColor(int objectType);
void go(_snake *head);
void changeDirection(int32_t *new_direction, int32_t key);
int distance(const _snake *snake, const struct food food);
void autoChangeDirection(_snake *snake, struct food food[], int foodSize);
int checkDirection(int32_t dir, int32_t key);
void initFood(struct food f[], size_t size);
void goTail(_snake *head);
void printHelp(char *s);
void putFoodSeed(struct food *fp);
void blinkFood(struct food fp[], size_t nfood);
void repairSeed(struct food f[], size_t nfood, _snake *head);
void putFood(struct food f[], size_t number_seeds);
void refreshFood(struct food f[], int nfood);
unsigned char haveEat(_snake *head, struct food f[]);
void printLevel(_snake *head);
_Bool isCrash(_snake *head);
void startMenu(void);

// ========= Main Functions ===================================================

int main() {
    unsigned char qs = 0;
    _snake *snake = NULL;

    initscr();
    getmaxyx(stdscr, display.height, display.width);
    display.posYHelp = 1;
    display.posYStat = 2;
    startMenu();
    char ch[] = "*";
    int x = 0, y = 0, key_pressed = 0;
    snake = addSnake(snake);
    initFood(food, MAX_FOOD_SIZE);
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch

    
    curs_set(FALSE);    //Отключаем курсор
    clear();
    for(unsigned char i = 0; i < display.height; i++) 
        for(unsigned char j = 0; j < display.width; j++)
            if(i == 0 || j == 0 || j == display.width - 1 || i == display.height - 1 || i == display.posYStat + 1)
                mvprintw(i, j, "#");

    printHelp("Use arrows for control. Press 'q' for EXIT");
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    putFood(food, SEED_NUMBER);// Кладем зерна
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    while (key_pressed != STOP_GAME) {
        key_pressed = getch(); // Считываем клавишу
        if (checkDirection(snake->direction, key_pressed)) //Проверка корректности смены направления
        {
            changeDirection(&(snake->direction), key_pressed); // Меняем напарвление движения
        }
        autoChangeDirection(snake, food, SEED_NUMBER);        
        if (isCrash(snake))
            break;
        go(snake); // Обновляем координаты змей
        printLevel(snake);
        haveEat(snake, food); // проверяем поедание еды
        refreshFood(food, SEED_NUMBER);// Обновляем еду
        repairSeed(food, SEED_NUMBER, snake);
        blinkFood(food, SEED_NUMBER);
        if(snake->tsize > 10 && getNumSnake(snake) < 2)
            snake = addSnake(snake);
        if(snake->tsize > 20 && getNumSnake(snake) < 3)
            snake = addSnake(snake);
        timeout(100); // Задержка при отрисовке
    }
    setColor(SNAKE1);
    char str[100] = { 0 };
    sprintf(str, "Your snake is die :( size is %d", snake->tsize);
    printHelp(str);
    timeout(SPEED);
    getch();
    endwin(); // Завершаем режим curses mod
    delAllSnake(snake);
    return 0;
}

// ========= Functions description ============================================

unsigned char initHead(_snake *head)
{
    if(!head)
        return 1;
    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);
    head->x = (rand()) % (max_x - 1);
    head->y = (rand()) % (max_y - 10) + 1;
    head->direction = rand() % DOWN + 1;
    for (unsigned char i = 0; i < MAX_TAIL_SIZE; i++)
    {
        head->tail[i].x = 0;
        head->tail[i].y = 0;
    }
    head->tsize = START_TAIL_SIZE;
    head->next = NULL;
    return 0;  
}

/**
 * @brief Add new snake at list
 * 
 * @param head Pointer to snake
 * @return Pointer to snake
 */
_snake * addSnake(_snake *head)
{
    _snake *newSnake = malloc(sizeof(_snake));
    initHead(newSnake);
    if(!head)
    {
        newSnake->number = 0;
        return newSnake;
    }
    _snake *next = head;
    while(next->next)
        next = next->next;
    newSnake->number = next->number + 1;
    next->next = newSnake;
    return head;
}

/**
 * @brief Clear display under snake
 * 
 * @param head Pointer to snake
 */
void clearPosSnake(_snake *head)
{
    for(unsigned char i = 0; i < head->tsize; i++)
        mvprintw(head->tail[i].y, head->tail[i].x, " ");
    mvprintw(head->y, head->x, " ");
}

/**
 * @brief Delete selected snake
 * 
 * @param head Pointer to snake
 * @param nSnake Num of snake to delete
 * @return unsigned char Errorcode (0 - Ok)
 */
unsigned char delSnake(_snake *head, unsigned char nSnake)
{
    if(!head || !head->next)
        return 1;
    if(head->next->number == nSnake && head->next->next == NULL)
    {
        clearPosSnake(head->next);
        free(head->next);
        head->next = NULL;
        return 0;
    }
    _snake *next = head->next;
    while (next->next->number != nSnake)
    {
        if(!next->next)
            return 1;
        next = next->next;
    }
    if(!next->next->next)
    {
        clearPosSnake(next->next);
        free(next->next);
        next->next = NULL;
        return 0;
    }
    _snake *delS = next->next;
    next->next = next->next->next;
    clearPosSnake(delS);
    free(delS);
    unsigned char counter = 0;
    while (head->next)
    {
        head->number = counter;
        head = head->next;
    }    
    return 0;    
}

/**
 * @brief Delete all snake
 * 
 * @param head Pointer to snake
 * @return unsigned char Errorcode (0 - Ok)
 */
unsigned char delAllSnake(_snake *head)
{
    while (head->next)
    {
        _snake *delSnake = head;
        head = head->next;
        free(delSnake);
    }
    free(head);
    return 0;
}

/**
 * @brief Get the Num Snake object
 * 
 * @param head Pointer to snake
 * @return unsigned char Number snake
 */
unsigned char getNumSnake(_snake *head)
{
    while (head->next)
    {
        head = head->next;
    }
    return head->number + 1;
}

void printLogo(void) {
    for(unsigned char line = 0; line < LOGOHEIGHT; line++)
        printw("%s\n", logo[line]);
}

void setColor(int objectType)
{
    attroff(COLOR_PAIR(1));
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
    switch (objectType){
        case 1:{ // SNAKE1
            attron(COLOR_PAIR(1));
            break;
        }
        case 2:{ // SNAKE2
            attron(COLOR_PAIR(2));
            break;
        }
        case 3:{ // FOOD
            attron(COLOR_PAIR(3));
            break;
        }
    }
}

/*
 Движение головы с учетом текущего направления движения
 */
void go(_snake *head) 
{
    while (head)
    {
        setColor(head->number);
        char ch[] = "@";
        int max_x = 0, max_y = 0;
        getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
        mvprintw(head->y, head->x, " "); // очищаем один символ
        switch (head->direction) {
        case LEFT:
            if (head->x <= 0) // Циклическое движение, что бы не уходить за пределы экрана
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
        goTail(head);
        head = head->next;
    }
}

void changeDirection(int32_t *new_direction, int32_t key)
{
    switch (key) {
        case KEY_DOWN: // стрелка вниз
            *new_direction = DOWN;
            break;
        case KEY_UP: // стрелка вверх
            *new_direction = UP;
            break;
        case KEY_LEFT: // стрелка влево
            *new_direction = LEFT;
            break;
        case KEY_RIGHT: // стрелка вправо
            *new_direction = RIGHT;
            break;
        default:
            break;
    }
}

int distance(const _snake *snake, const struct food food)
{   // вычисляет количество ходов до еды
    return (abs(snake->x - food.x) + abs(snake->y - food.y));
}

void autoChangeDirection(_snake *snake, struct food food[], int foodSize)
{
    _snake *autoSnake = snake->next;
    while(autoSnake)
    {
        int pointer = 0;
        for (int i = 1; i < foodSize; i++)
        {   // ищем ближайшую еду
            pointer = (distance(autoSnake, food[i]) < distance(autoSnake, food[pointer])) ? i : pointer;
        }
        if ((autoSnake->direction == RIGHT || autoSnake->direction == LEFT) &&
            (autoSnake->y != food[pointer].y))
        {  // горизонтальное движение
            autoSnake->direction = (food[pointer].y > autoSnake->y) ? DOWN : UP;
        } else if ((autoSnake->direction == DOWN || autoSnake->direction == UP) &&
               (autoSnake->x != food[pointer].x))
        {  // вертикальное движение
            autoSnake->direction = (food[pointer].x > autoSnake->x) ? RIGHT : LEFT;
        }   
        autoSnake = autoSnake->next;
    }    
}

int checkDirection(int32_t dir, int32_t key)
{
    if (KEY_DOWN == key && dir == UP || KEY_UP == key && dir == DOWN || KEY_LEFT == key && dir == RIGHT ||
        KEY_RIGHT == key && dir == LEFT) {
        return 0;
    } else {
        return 1;
    }
}

void initFood(struct food f[], size_t size)
{
    struct food init = {0, 0, 0, 0, 0};
    int max_y = 0, max_x = 0;
    getmaxyx(stdscr, max_y, max_x);
    for (size_t i = 0; i < size; i++) {
        f[i] = init;
    }
}

/*
 Движение хвоста с учетом движения головы
 */
void goTail(_snake *head)
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

void printHelp(char *s)
{
    for(unsigned char px = 2; px < display.width - 1; px++)
        mvprintw(display.posYHelp, px, " ");
    mvprintw(display.posYHelp, 2, s);
}

/*
 Обновить/разместить текущее зерно на поле
 */
void putFoodSeed(struct food *fp)
{
    int max_x = 0, max_y = 0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    setColor(FOOD);
    mvprintw(fp->y, fp->x, spoint);
}

// Мигаем зерном, перед тем как оно исчезнет
void blinkFood(struct food fp[], size_t nfood)
{
    time_t current_time = time(NULL);
    char spoint[2] = {0}; // как выглядит зерно '$','\0'
    for (size_t i = 0; i < nfood; i++) {
        if (fp[i].enable && (current_time - fp[i].put_time) > 6) {
            spoint[0] = (current_time % 2) ? 'S' : 's';
            setColor(FOOD);
            mvprintw(fp[i].y, fp[i].x, spoint);
        }
    }
}

void repairSeed(struct food f[], size_t nfood, _snake *head)
{
    for (size_t i = 0; i < head->tsize; i++)
        for (size_t j = 0; j < nfood; j++) {
            /* Если хвост совпадает с зерном */
            if (f[j].x == head->tail[i].x && f[j].y == head->tail[i].y && f[i].enable) {
                mvprintw(0, 0, "Repair tail seed %d", j);
                putFoodSeed(&f[j]);
            }
        }
    for (size_t i = 0; i < nfood; i++)
        for (size_t j = 0; j < nfood; j++) {
            /* Если два зерна на одной точке */
            if (i != j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y == f[i].y && f[i].enable) {
                mvprintw(0, 0, "Repair same seed %d", j);
                putFoodSeed(&f[j]);
            }
        }
}

/*
 Разместить еду на поле
 */
void putFood(struct food f[], size_t number_seeds)
{
    for (size_t i = 0; i < number_seeds; i++) {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(struct food f[], int nfood)
{
    int max_x = 0, max_y = 0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    for (size_t i = 0; i < nfood; i++) {
        if (f[i].put_time) {
            if (!f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS) {
                putFoodSeed(&f[i]);
            }
        }
    }
}

unsigned char haveEat(_snake *head, struct food f[])
{
    while(head)
    {
        for(unsigned char i = 0; i < MAX_FOOD_SIZE; i++)
            if (f[i].enable && head->x == f[i].x && head->y == f[i].y)
            {
                f[i].enable = 0;
                head->tsize++;
                printLevel(head);
            }
        head = head->next;
    }
    return 0;
}

void printLevel(_snake *head)
{
    setColor(head->number);
    mvprintw(display.posYStat, 2, "LEVEL:   ");
    mvprintw(display.posYStat, 2, "LEVEL: %d", head->tsize);
}

_Bool isCrash(_snake *head)
{
    unsigned char crush = 0;
    _snake *ihead = head;
    _snake *next = head;
    while (ihead)
    {   
        // проверяем, что нет столкновений со стенками
        if(ihead->x == 0 || ihead->x == display.width - 1 || ihead->y == display.height - 1 || ihead->y == display.posYStat + 1)
        {
            if(ihead->number == 0)
                return 1;
            unsigned char delnum = ihead->number;
            ihead = head;
            delSnake(head, delnum);
            continue;            
        }
        // проверяем, что нет столкновений голов
        if(ihead->number != next->number)
            if(ihead->x == next->x && ihead->y == next->y)
            {
                if(ihead->number == 0)
                    return 1;
                unsigned char delnum = ihead->number;
                ihead = head;
                delSnake(head, delnum);
                continue;
            }
        for (unsigned char i = 1; i < next->tsize; i++)
        {
            if(ihead->x == next->tail[i].x && ihead->y == next->tail[i].y)
            {
                if(ihead->number == 0)
                {
                    return 1;
                }                    
                unsigned char delnum = ihead->number;
                ihead = head;
                delSnake(head, delnum);
                continue;
            }
        }
        next = next->next;
        if(!next)
        {
            next = head;
            ihead = ihead->next;
        }
    }
    return 0;
}

void startMenu(void)
{
    const unsigned char menuHeight = 10;
    const unsigned char winHeight = LOGOHEIGHT + menuHeight;
    const unsigned char winWidth = LOGOWIDTH;
    //initscr();
    //resize_term(winHeight, winWidth);
    noecho();
	curs_set(FALSE);
    cbreak();

    if(has_colors() == FALSE)
	{
        endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

    for(unsigned char i = 0; i < display.height; i++) 
        for(unsigned char j = 0; j < display.width; j++)
            if(i == 0 || j == 0 || j == display.width - 1 || i == display.height - 1)
                mvprintw(i, j, "#");
    printLogo();


	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);

    attron(COLOR_PAIR(1));
	mvprintw(2, 2, "1. Start");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(4, 2, "2. Exit");

    char ch = 0;
    while(1) {
		ch = getch();
        if(ch == '1') {
            clear();
            attron(COLOR_PAIR(2));
            mvprintw(10, 50, "S N A K E ");

            attron(COLOR_PAIR(1));
            mvprintw(20, 50, "Press any key ...");
            break;
        }
		else if(ch == '2') {
            endwin();
            exit(0);
        }
    }
    refresh();
    getch();
    //endwin();
}