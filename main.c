/*
 * ��� ���������� ���������� �������� ���� -lncurses
 * gcc -o snake main.c -lncurses
  
 //1. ������� ���� - ���������� ����������, ��������� ��� �������� ������� ����, �.�. ������� ������ ����� ���������� �������������
    go, initHead, initFood, putFoodSeed, refreshFood, printLevel, printExit, main
    //1. int max_x = 0, max_y = 0;
    //1. getmaxyx(stdscr, max_y, max_x); // macro - ������ ���������
    //1. ����� ������� ������� �����, ���� ��������� ������
    //1. ��������� ������ ������ ��������������
    //1. ��������� ��������� ������� ������ ��������������
 //2. ������ �������
    //2. ������ �����
 //3. ������ �� ����� ��������� ������ ���� �����
    //3. ������ ������� �������
 //4. ������� �������� "�� ���������"
 //5. ��������� ��������� �������
    checkDirection, isCrash 

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <inttypes.h>
#include <wchar.h>

// 1. ������� ���� - ���������� ����������, ��������� ��� �������� ������� ����, �.�. ������� ������ ����� ���������� �������������
int max_x = 0, max_y = 0;
//int max_x_old = 0, max_y_old = 0;

int count = 0;

enum {
    LEFT = 1, UP, RIGHT, DOWN, STOP_GAME = 'q'
};
enum {
    MAX_TAIL_SIZE = 1000,
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

/*
 ����� ��� ������ ��������� �� ��������� x,y
 */
struct tail {
    int x;
    int y;
} tail[2][MAX_TAIL_SIZE]; //2. ������ �������
/*
 ��� ������ �����
 x, y - ���������� ��� ����������� �����
 put_time - ����� ����� ������ ����� ���� �����������
 point - ������� ��� ����� ('$','E'...)
 enable - ���� �� ����� �������
 */
struct food {
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE]; //2. ������ �����

/*
 ������ ������ �������� � ����
 x,y - ���������� ������� �������
 direction - ����������� ��������
 tsize - ������ ������
 *tail -  ������ �� �����
 */
struct snake {
    int number;
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail *tail;
} snake[2];

void setColor(int objectType){
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

void go(struct snake *);

//3. ������ �� ����� ��������� ������ ���� �����
int checkPoint(struct snake *head, int y, int x) {
	int empty = 1;
	
	for(int i = 0; i < 2; i++) {
		if(head->number != snake[i].number) {
			if(snake[i].x == x &&  snake[i].y == y) {
				empty = 0;
				break;
			}
			for(int j = 0; j < snake[i].tsize; j++) {
				if(snake[i].tail[j].x == x &&  snake[i].tail[j].y == y) {
					empty = 0;
					break;
				}

			}
			if (empty == 0) break;
		}
	}

	//mvprintw(1, 0, "empty: %d count: %d", empty, count);	

	if(empty == 0 && count < 4) { //3. ������ ������� �������
		switch (head->direction) {
			case LEFT:
				head->direction = UP;
				go(head);
				break;
			case UP:
				head->direction = RIGHT;
				go(head);
				break;
			case RIGHT:
				head->direction = DOWN;
				go(head);
				break;
			case DOWN:
				head->direction = LEFT;
				go(head);
				break;
			default:
				break;
		}
		count++;
	}
	else {
		count = 0;
	}
	return empty;
}

/*
 �������� ������ � ������ �������� ����������� ��������
 */
void go(struct snake *head) {
    setColor(head->number);
    char ch[] = "@";
    //1. int max_x = 0, max_y = 0;
    //1. getmaxyx(stdscr, max_y, max_x); // macro - ������ ���������
    mvprintw(head->y, head->x, " "); // ������� ���� ������
    switch (head->direction) {
        case LEFT:
            if (head->x <= 0) {// ����������� ��������, ��� �� ��
                // ������� �� ������� ������
                head->x = max_x;
			}
            if (checkPoint(head, head->y, (head->x)-1)) {
				mvprintw(head->y, --(head->x), ch);
			}
            break;
        case RIGHT:
            if (head->x >= max_x) {
                head->x = 0;
			}
            if (checkPoint(head, head->y, (head->x)+1)) {
				mvprintw(head->y, ++(head->x), ch);
			}
            break;
        case UP:
            if (head->y <= 0) {
                head->y = max_y;
			}
			if (checkPoint(head, (head->y)-1, head->x)) {
				mvprintw(--(head->y), head->x, ch);	
			}
            break;
        case DOWN:
            if (head->y >= max_y) {
                head->y = 0;
			}
			if (checkPoint(head, (head->y)+1, head->x)) {
				mvprintw(++(head->y), head->x, ch);
			}
            break;
        default:
            break;
    }
    refresh();
}

void changeDirection(int32_t *new_direction, int32_t key) {
	//mvprintw(1, 0, "key: %d", key);
    switch (key) {
        case KEY_DOWN: // ������� ����
            *new_direction = DOWN;
            break;
        case KEY_UP: // ������� �����
            *new_direction = UP;
            break;
        case KEY_LEFT: // ������� �����
            *new_direction = LEFT;
            break;
        case KEY_RIGHT: // ������� ������
            *new_direction = RIGHT;
            break;
        default:
            break;
    }
}


int distance(const struct snake snake, const struct food food) {   // ��������� ���������� ����� �� ���
    return (abs(snake.x - food.x) + abs(snake.y - food.y));
}

void autoChangeDirection(struct snake *snake, struct food food[], int foodSize) {
    int pointer = 0;
    for (int i = 1; i < foodSize; i++) {   // ���� ��������� ���
        pointer = (distance(*snake, food[i]) < distance(*snake, food[pointer])) ? i : pointer;
    }
    //4. ������� �������� "�� ���������"
    if(abs(snake->y-food[pointer].y) < abs(snake->x-food[pointer].x) || abs(snake->x-food[pointer].x) == 0) {
		if ((snake->direction == RIGHT || snake->direction == LEFT) &&
			(snake->y != food[pointer].y)) {  // �������������� ��������
			snake->direction = (food[pointer].y > snake->y) ? DOWN : UP;
		}
	} 
	if(abs(snake->x-food[pointer].x) < abs(snake->y-food[pointer].y) || abs(snake->y-food[pointer].y) == 0) {
		if ((snake->direction == DOWN || snake->direction == UP) &&
				   (snake->x != food[pointer].x)) {  // ������������ ��������
			snake->direction = (food[pointer].x > snake->x) ? RIGHT : LEFT;
		}
	}
}


int checkDirection(int32_t dir, int32_t key) {
	//5. ��������� ��������� �������
	return 1;
	
    if ((KEY_DOWN == key && dir == UP) || (KEY_UP == key && dir == DOWN) || (KEY_LEFT == key && dir == RIGHT) ||
        (KEY_RIGHT == key && dir == LEFT)) {
        return 0;
    } else {
        return 1;
    }
}

void initTail(struct tail t[], size_t size) {
    struct tail init_t = {0, 0};
    for (size_t i = 0; i < size; i++) {
        t[i] = init_t;
    }
}

void initHead(struct snake *head) {
    //1. int max_y = 0, max_x = 0;
    //2. getmaxyx(stdscr, max_y, max_x);
    head->x = (rand()) % (max_x - 1);
    head->y = (rand()) % (max_y - 10) + 1;
    head->direction = RIGHT;
}

void initFood(struct food f[], size_t size) {
    struct food init = {0, 0, 0, 0, 0};
    //1. int max_y = 0, max_x = 0;
    //2. getmaxyx(stdscr, max_y, max_x);
    for (size_t i = 0; i < size; i++) {
        f[i] = init;
    }
}

void init(struct snake *head, int number, struct tail *tail, size_t size) {
    clear(); // ������� ���� �����
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head);
    head->number = number;
    head->tail = tail; // ����������� � ������ �����
    head->tsize = size + 1;
}

/*
 �������� ������ � ������ �������� ������
 */
void goTail(struct snake *head) {
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
 ���������� ������ �� 1 �������
 */
void addTail(struct snake *head) {
    if (head == NULL || head->tsize > MAX_TAIL_SIZE) {
        mvprintw(0, 0, "Can't add tail");
        return;
    }
    head->tsize++;
}

void printHelp(char *s) {
    mvprintw(0, 0, s);
}

/*
 ��������/���������� ������� ����� �� ����
 */
void putFoodSeed(struct food *fp) {
    //1. int max_x = 0, max_y = 0;
    char spoint[2] = {0};
    //2. getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1; //�� �������� ������� ������
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    setColor(FOOD);
    mvprintw(fp->y, fp->x, spoint);
}

// ������ ������, ����� ��� ��� ��� ��������
void blinkFood(struct food fp[], size_t nfood) {
    time_t current_time = time(NULL);
    char spoint[2] = {0}; // ��� �������� ����� '$','\0'
    for (size_t i = 0; i < nfood; i++) {
        if (fp[i].enable && (current_time - fp[i].put_time) > 6) {
            spoint[0] = (current_time % 2) ? 'S' : 's';
            setColor(FOOD);
            mvprintw(fp[i].y, fp[i].x, spoint);
        }
    }
}

void repairSeed(struct food f[], size_t nfood, struct snake *head) {
    for (size_t i = 0; i < head->tsize; i++)
        for (size_t j = 0; j < nfood; j++) {
            /* ���� ����� ��������� � ������ */
            if (f[j].x == head->tail[i].x && f[j].y == head->tail[i].y && f[i].enable) {
                mvprintw(0, 0, "Repair tail seed %d", j);
                putFoodSeed(&f[j]);
            }
        }
    for (size_t i = 0; i < nfood; i++)
        for (size_t j = 0; j < nfood; j++) {
            /* ���� ��� ����� �� ����� ����� */
            if (i != j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y == f[i].y && f[i].enable) {
                mvprintw(0, 0, "Repair same seed %d", j);
                putFoodSeed(&f[j]);
            }
        }
}

/*
 ���������� ��� �� ����
 */
void putFood(struct food f[], size_t number_seeds) {
    for (size_t i = 0; i < number_seeds; i++) {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(struct food f[], int nfood) {
    //1. int max_x = 0, max_y = 0;
    //char spoint[2] = {0};
    //2. getmaxyx(stdscr, max_y, max_x);
    for (size_t i = 0; i < nfood; i++) {
        if (f[i].put_time) {
            if (!f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS) {
                putFoodSeed(&f[i]);
            }
        }
    }
}

_Bool haveEat(struct snake *head, struct food f[]) {
    for (size_t i = 0; i < MAX_FOOD_SIZE; i++)
        if (f[i].enable && head->x == f[i].x && head->y == f[i].y) {
            f[i].enable = 0;
            return 1;
        }
    return 0;
}

void printLevel(struct snake *head) {
    //1. int max_x = 0, max_y = 0;
    //2. getmaxyx(stdscr, max_y, max_x);
    if (head->number == SNAKE1){
        setColor(head->number);
        mvprintw(0, max_x - 10, "LEVEL: %d", head->tsize);
    }
    if (head->number == SNAKE2){
        setColor(head->number);
        mvprintw(1, max_x - 10, "LEVEL: %d", head->tsize);
    }    
}

void printExit(struct snake *head) {
    //1. int max_x = 0, max_y = 0;
    //2. getmaxyx(stdscr, max_y, max_x);
    mvprintw(max_y / 2, max_x / 2 - 5, "Your LEVEL is %d", head->tsize);
}

_Bool isCrash(struct snake *head) {
	//5. ��������� �������� �����
	return 0;
	
    for (size_t i = 1; i < head->tsize; i++)
        if (head->x == head->tail[i].x && head->y == head->tail[i].y)
            return 1;
    return 0;
}

/*
#ifndef is_term_resized

_Bool is_term_resized(int x, int y) {
	return is_termresized();
}


#endif
*/

//1. ����� ������� ������� �����, ���� ��������� ������
void checkSize(void) {
	if(is_termresized()) { // for Linux is_term_resized(int, int)
		resize_term(0, 0);
		getmaxyx(stdscr, max_y, max_x);
		clear();
		curs_set(FALSE);    //��������� ������
		setColor(0);
		printHelp(" Use arrows for control. Press 'q' for EXIT");
		printLevel(&snake[0]);
		printLevel(&snake[1]);
	}
}


int main() {
    //char ch[] = "*";
    //int x = 0, y = 0, key_pressed = 0;
    int key_pressed = 0;
    initscr();            // ����� curses mod
	getmaxyx(stdscr, max_y, max_x); //1. ��������� ������ ������ ��������������
    init(&snake[0], 1, tail[0], START_TAIL_SIZE); //�������������, ����� = 3
    init(&snake[1], 2, tail[1], START_TAIL_SIZE); //�������������, ����� = 3
    initFood(food, MAX_FOOD_SIZE);
	checkSize(); //1. ��������� ��������� ������� ������ ��������������
    keypad(stdscr, TRUE); // �������� F1, F2, ������� � �.�.
    raw();                // ��������� line buffering
    noecho();            // ��������� echo() ����� ��� ������ getch
    curs_set(FALSE);    //��������� ������
    printHelp(" Use arrows for control. Press 'q' for EXIT");
	printLevel(&snake[0]);
	printLevel(&snake[1]);
    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK); // text color
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // blue is dark
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    putFood(food, SEED_NUMBER);// ������ �����
    timeout(0);    //��������� ������� ����� ������� ������� � �����
    while (key_pressed != STOP_GAME) {
		checkSize(); //1. ��������� ��������� ������� ������ ��������������
		key_pressed = getch(); // ��������� �������
        if (checkDirection(snake[0].direction, key_pressed)) //�������� ������������ ����� �����������
        {
            changeDirection(&snake[0].direction, key_pressed); // ������ ����������� ��������
        }
        autoChangeDirection(&snake[1], food, SEED_NUMBER);
        if (isCrash(&snake[0]))
            break;
        go(&snake[0]); // ������ ����� ������
        goTail(&snake[0]); //������ �����
        go(&snake[1]); // ������ ����� ������
        goTail(&snake[1]); //������ �����
        if (haveEat(&snake[0], food)) {
            addTail(&snake[0]);
            printLevel(&snake[0]);
        }
        if (haveEat(&snake[1], food)) {
            addTail(&snake[1]);
            printLevel(&snake[1]);
        }
        refreshFood(food, SEED_NUMBER);// ��������� ���
        repairSeed(food, SEED_NUMBER, &snake[0]);
        blinkFood(food, SEED_NUMBER);
        timeout(100); // �������� ��� ���������
    }
    setColor(SNAKE1);
    printExit(&snake[0]);
    timeout(SPEED);
    getch();
    endwin(); // ��������� ����� curses mod

    return 0;
}
