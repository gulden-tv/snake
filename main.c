/*
 * Для компиляции необходимо добавить ключ -lncurses
 * gcc -o snake main.c -lncurses
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <inttypes.h>
#include <wchar.h>
#include <unistd.h>


#define PRINT_USAGE printf("Usage: %s [-s]\n -s flag get you possibility to enjoy two-player game. Try to find it out!\n", argv[0]);




enum {LEFT=0, UP, RIGHT, DOWN};
enum { STOP_GAME=KEY_F(10), MAX_PLAYER_ID=1, MAX_SNAKES=10};

enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=5, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10, SPEED=20000, SEED_NUMBER=3};
 char* heads="@Q0";

/*
 Хвост этто массив состоящий из координат x,y
 */
typedef struct tail_t {
    int x;
    int y;
} tail_t;

/*
 Еда массив точек
 x, y - координата где установлена точка
 put_time - время когда данная точка была установлена
 point - внешний вид точки ('$','E'...)
 enable - была ли точка съедена
 */
struct food {
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];


// Здесь храним коды управления змейкой
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
}control_buttons;


struct control_buttons default_controls[3] = {{KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT}, {'s', 'w', 'a', 'd'}, {0,1,2,3}};

/*
 Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост
 */
typedef struct snake_t {
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail_t *tail;
    char head_symbol;
    struct control_buttons controls;
} snake_t;

/*
 Движение головы с учетом текущего направления движения
 */
void go(struct snake_t *head) {
    char ch = head->head_symbol;
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    //clear(); // очищаем весь экран
    mvprintw(head->y, head->x, " "); // очищаем один символ
    switch (head->direction) {
        case LEFT:
            if(head->x <= 0) // Циклическое движение, что бы не
                // уходить за пределы экрана
                head->x = max_x;
            mvprintw(head->y, --(head->x), "%c", ch);
            break;
        case RIGHT:
            if(head->x >= max_x)
                head->x = 0;
            mvprintw(head->y, ++(head->x), "%c", ch);
            break;
        case UP:
            if(head->y <= 0)
                head->y = max_y;
            mvprintw(--(head->y), head->x, "%c", ch);
            break;
        case DOWN:
            if(head->y >= max_y)
                head->y = 0;
            mvprintw(++(head->y), head->x, "%c", ch);
            break;
        default:
            break;
    }
    refresh();
}
void changeDirection(snake_t* snake, const int32_t key) {
    if (key == snake->controls.down)
        snake->direction = DOWN;
    else if (key == snake->controls.up)
        snake->direction = UP;
    else if (key == snake->controls.right)
        snake->direction = RIGHT;
    else if (key == snake->controls.left)
        snake->direction = LEFT;

}

int checkDirection(snake_t* snake, int32_t key)
{
    if(snake->controls.down == key&&snake->direction==UP || snake->controls.up == key&&snake->direction==DOWN || snake->controls.left == key&&snake->direction==RIGHT || snake->controls.right == key&&snake->direction==LEFT)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void initTail(struct tail_t t[], size_t size) {
    struct tail_t init_t={0,0};
    for(size_t i=0; i<size; i++) {
        t[i]=init_t;
    }
}
void initHead(struct snake_t *head, int x, int y) {
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}
void initFood(struct food f[], size_t size) {
    struct food init = {0,0,0,0,0};
    int max_y=0, max_x=0;
    getmaxyx(stdscr, max_y, max_x);
    for(size_t i=0; i<size; i++) {
        f[i] = init;
    }
}
void init(struct snake_t *head, struct tail_t *tail, size_t size, int x, int y) {
    //clear(); // очищаем весь экран
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail; // прикрепляем к голове хвост
    head->tsize = size+1;
}

/*
 Движение хвоста с учетом движения головы
 */
void goTail(struct snake_t *head) {
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    for(size_t i = head->tsize-1; i>0; i--) {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

/*
 Увеличение хвоста на 1 элемент
 */
void addTail(struct snake_t *head) {
    if(head == NULL || head->tsize>MAX_TAIL_SIZE) {
        mvprintw(0, 0, "Can't add tail");
        return;
    }
    head->tsize++;
}
void printHelp(char *s) {
    mvprintw(0, 0, "%s", s);
}

/*
 Обновить/разместить текущее зерно на поле
 */
void putFoodSeed(struct food *fp) {
    int max_x=0, max_y=0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    mvprintw(fp->y, fp->x, "%s", spoint);
}

void repairSeed(struct food f[], size_t nfood, struct snake_t *head) {
    for( size_t i=0; i<head->tsize; i++ )
        for( size_t j=0; j<nfood; j++ ){
            /* Если хвост совпадает с зерном */
            if( f[j].x == head->tail[i].x && f[j].y == head->tail[i].y && f[i].enable ) {
                mvprintw(1, 0, "Repair tail seed %zu",j);
                putFoodSeed(&f[j]);
            }
        }
    for( size_t i=0; i<nfood; i++ )
        for( size_t j=0; j<nfood; j++ ){
            /* Если два зерна на одной точке */
            if( i!=j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y == f[i].y && f[i].enable ) {
                mvprintw(1, 0, "Repair same seed %zu",j);
                putFoodSeed(&f[j]);
            }
        }
}

/*
 Разместить еду на поле
 */
void putFood(struct food f[], size_t number_seeds) {
    for(size_t i=0; i<number_seeds; i++) {
        putFoodSeed(&f[i]);
    }
}
void refreshFood(struct food f[], int nfood) {
    int max_x=0, max_y=0;
    char spoint[2] = {0};
    getmaxyx(stdscr, max_y, max_x);
    for(size_t i=0; i<nfood; i++) {
        if( f[i].put_time ) {
            if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS ) {
                putFoodSeed(&f[i]);
            }
        }
    }
}

_Bool haveEat(struct snake_t *head, struct food f[]) {
    for(size_t i=0; i<MAX_FOOD_SIZE; i++)
        if( f[i].enable && head->x == f[i].x && head->y == f[i].y  ) {
            f[i].enable = 0;
            return 1;
        }
    return 0;
}

void printLevel(int level) {
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(0, max_x-10, "LEVEL: %d", level);
}

void printExit(int level) {
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(max_y/2, max_x/2-5, "Your LEVEL is %d", level);
}

int calculateLevel(snake_t ** snakes, int last_snake_id)
{
    int res=0;
    for(int i = 0; i<= last_snake_id && i <= MAX_PLAYER_ID ; i++)
    {
        res += snakes[i]->tsize;
    }
    res-=START_TAIL_SIZE;
    return res+1;
}

_Bool isLoop(struct snake_t *head) {
    for(size_t i=1; i<head->tsize; i++)
        if(head->x == head->tail[i].x && head->y == head->tail[i].y)
        {

            return i;
        }
    return 0;
}

_Bool isCrush(struct snake_t ** snakes, int id, int max_id) {
    for(int k = 0; k<=max_id; k++)
    {
        if(id == k)
        {
            continue;
        }
        for(size_t i=1; i<snakes[k]->tsize; i++)
        if(snakes[id]->x == snakes[k]->tail[i].x && snakes[id]->y == snakes[k]->tail[i].y)
        {
            return 1;
        }
    }
    return 0;
}


int makeSnake(snake_t ** s,  int current_max_id, int tail_size, int x, int y)
{
    if(current_max_id + 1 >= MAX_SNAKES)
        return current_max_id;

    snake_t*new_snake =  (snake_t*) malloc(sizeof(struct snake_t));

    if(NULL != new_snake)
    {
        tail_t * tail = (tail_t*)malloc(MAX_TAIL_SIZE* sizeof(struct tail_t));
        if(tail == NULL)
        {
            free(new_snake);
            return current_max_id;
        }
        current_max_id++;
        s[current_max_id] = new_snake;

        if(current_max_id <= MAX_PLAYER_ID)
        {
            new_snake->controls = default_controls[current_max_id];
            new_snake->head_symbol=heads[current_max_id];
        }
        else
        {
            new_snake->controls = default_controls[MAX_PLAYER_ID+1];
            new_snake->head_symbol=heads[MAX_PLAYER_ID+1];
        }
        init(new_snake, tail, tail_size, x,y);
    }
    return current_max_id;
}

void clearHeadlessTail(snake_t * snake)
{
    for (int i = snake->tsize; i< MAX_TAIL_SIZE; i++)
    {
        if(snake->tail[i].x == 0 && snake->tail[i].x == 0 ) ///хвост закончился
        {
         //   break;
        }
        mvprintw(snake->tail[i].y,  snake->tail[i].x, " ");
    }
}


int main(int argc, char ** argv)
{
    int opt;
    int split_mode = 0;
    while((opt = getopt(argc, argv, "hs")) !=-1)
    {
        if( 'h' == opt )
        {
            PRINT_USAGE;
            exit(0);
        }
        else if ('s' == opt)
        {
            split_mode = 1;
        }
    }

    int last_snake_id = -1;
    snake_t * snakes[MAX_SNAKES];

    last_snake_id=makeSnake(snakes, last_snake_id, START_TAIL_SIZE, 0, 2);
    if(last_snake_id < 0)
    {
        mvprintw(0, 10, "Could not start snake...");
        return 0;
    }

//     snakes[0] = (snake_t*) malloc(sizeof(struct snake_t));

//     char ch[]="*";
    int x=0, y=0, key_pressed=0;
//     init(snake, tail, START_TAIL_SIZE); //Инициализация, хвост = 3
    initFood(food, MAX_FOOD_SIZE);
    initscr();            // Старт curses mod
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    printHelp("  Use arrows for control. Press 'F10' for EXIT");
    putFood(food, SEED_NUMBER);// Кладем зерна
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    while( key_pressed != STOP_GAME ) {
        for(int snake_id = MAX_PLAYER_ID+1; snake_id<=last_snake_id; snake_id++) //меняем направление автозмей
        {
            if(rand() % 100 < 5)//С вероятностью 5%
            {
                int vkey = rand() % 4;
                if(checkDirection(snakes[snake_id], vkey)) //Проверка корректности смены направления
                {
                    changeDirection(snakes[snake_id], vkey); // Меняем напарвление движения
                }
            }
            int frac = isLoop(snakes[snake_id]);
            if(frac)
            {
                int t = snakes[snake_id]->tsize;
                int x = snakes[snake_id]->x;
                int y = snakes[snake_id]->y;
                int dir = snakes[snake_id]->direction;

                last_snake_id = makeSnake(snakes, last_snake_id, t-frac, x, y);
                snakes[snake_id]->tsize = frac;
                clearHeadlessTail(snakes[snake_id]);
                init(snakes[snake_id],snakes[snake_id]->tail, frac-1, x, y);
                snakes[snake_id]->direction = dir;
                snakes[last_snake_id]->direction = (dir+1)%4; // чтобы новая змея ползла  в другую сторону
            }
            if(isCrush(snakes, snake_id, last_snake_id))
            {
                free(snakes[last_snake_id]);
            }

            go(snakes[snake_id]); // Рисуем новую голову
            goTail(snakes[snake_id]); //Рисуем хвост
            if(haveEat(snakes[snake_id], food)) {
                addTail(snakes[snake_id]);
                printLevel( calculateLevel(snakes, last_snake_id));
            }
            repairSeed(food, SEED_NUMBER, snakes[snake_id]);
        }
        key_pressed = getch(); // Считываем клавишу
        for(int snake_id = 0; snake_id <= MAX_PLAYER_ID && snake_id<=last_snake_id; snake_id++)
        {
            if(checkDirection(snakes[snake_id], key_pressed)) //Проверка корректности смены направления
            {
                changeDirection(snakes[snake_id], key_pressed); // Меняем напарвление движения
            }

            int frac = isLoop(snakes[snake_id]);

            if(frac)
            {
                if(!split_mode)
                {
                    key_pressed = STOP_GAME;
                    break;
                }
                else
                {
                    int t = snakes[snake_id]->tsize;
                    int x = snakes[snake_id]->x;
                    int y = snakes[snake_id]->y;
                    int dir = snakes[snake_id]->direction;

                    last_snake_id = makeSnake(snakes, last_snake_id, t-frac, x, y);
                    snakes[snake_id]->tsize = frac;
                    clearHeadlessTail(snakes[snake_id]);
                    init(snakes[snake_id],snakes[snake_id]->tail, frac, x, y);
                    snakes[snake_id]->direction = dir;
                    snakes[last_snake_id]->direction = (dir+1)%4; // чтобы новая змея ползла  в другую сторону
                }
            }
            if(isCrush(snakes, snake_id, last_snake_id))
            {
                key_pressed = STOP_GAME;
                break;
            }

            go(snakes[snake_id]); // Рисуем новую голову
            goTail(snakes[snake_id]); //Рисуем хвост
            if(haveEat(snakes[snake_id], food)) {
                addTail(snakes[snake_id]);
                printLevel( calculateLevel(snakes, last_snake_id) );
            }
            repairSeed(food, SEED_NUMBER, snakes[snake_id]);
        }

        refreshFood(food, SEED_NUMBER);// Обновляем еду
        timeout(100); // Задержка при отрисовке
    }
    printExit(calculateLevel(snakes, last_snake_id));
    timeout(SPEED);
    for(int snake_id = 0; snake_id <= last_snake_id; snake_id++)
    {
        free(snakes[snake_id]);
    }
    getch();
    endwin(); // Завершаем режим curses mod
    
    return 0;
}
