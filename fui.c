#include <ncurses.h>
#include <stdlib.h>

typedef struct Node {
    char character;
    struct Node* next;
    struct Node* down;
} Node;

typedef struct Queue {
    Node* front;
    struct Queue* down;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->down = NULL;
    return queue;
}

void enqueue(Queue* queue, char character) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->character = character;
    newNode->next = NULL;
    newNode->down = NULL;

    if (queue->front == NULL) {
        queue->front = newNode;
    } else {
        Node* currentNode = queue->front;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
        currentNode->next = newNode;
    }
}

void dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return;
    }

    Node* temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
}

void insertChar(Queue* queue, int line, int position, char character) {
    Queue* currentQueue = queue;
    int i;
    for (i = 0; i < line && currentQueue != NULL; i++) {
        currentQueue = currentQueue->down;
    }

    if (currentQueue == NULL) {
        return;
    }

    Node* currentNode = currentQueue->front;
    Node* prevNode = NULL;

    int j = 0;
    while (j < position && currentNode != NULL) {
        prevNode = currentNode;
        currentNode = currentNode->next;
        j++;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->character = character;
    newNode->next = currentNode;

    if (prevNode == NULL) {
        currentQueue->front = newNode;
    } else {
        prevNode->next = newNode;
    }
}

void insertLine(Queue* queue, int position) {
    if (position < 0) {
        return;
    }

    Queue* currentQueue = queue;
    int i;
    for (i = 0; i < position; i++) {
        if (currentQueue->down == NULL) {
            Queue* newQueue = createQueue();
            currentQueue->down = newQueue;
        }
        currentQueue = currentQueue->down;
    }
}

void printQueue(Queue* queue) {
    Queue* currentQueue = queue;
    int i = 0;

    while (currentQueue != NULL) {
        Node* currentNode = currentQueue->front;
        int j = 0;

        while (currentNode != NULL) {
            mvprintw(i, j, "%c", currentNode->character);
            currentNode = currentNode->next;
            j++;
        }

        currentQueue = currentQueue->down;
        i++;
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    Queue* queue = createQueue();

    int cursorX = 0, cursorY = 0;

    int ch;
    while ((ch = getch()) != KEY_F(1)) {
        switch (ch) {
            case KEY_LEFT:
            case 75:
                if (cursorX > 0) {
                    cursorX--;
                }
                break;
            case KEY_RIGHT:
            case 77:
                if (cursorX < (COLS - 1)) {
                    cursorX++;
                }
                break;
            case KEY_UP:
            case 72:
                if (cursorY > 0) {
                    cursorY--;
                }
                break;
            case KEY_DOWN:
            case 80:
                if (cursorY < (LINES - 1)) {
                    cursorY++;
                }
                break;
            case '\n':
                insertLine(queue, cursorY + 1);
                cursorX = 0;
                cursorY++;
                break;
            case KEY_BACKSPACE:
            case 127:
                if (cursorX > 0 || cursorY > 0) {
                    Node* currentNode = queue->front;
                    Node* prevNode = NULL;
                    int i;
                    for (i = 0; i < cursorY * (COLS - 1) + cursorX - 1; i++) {
                        if (currentNode != NULL) {
                            prevNode = currentNode;
                            currentNode = currentNode->next;
                        }
                    }
                    if (currentNode != NULL) {
                        if (prevNode != NULL) {
                            prevNode->next = currentNode->next;
                            free(currentNode);
                        } else {
                            Node* temp = queue->front;
                            queue->front = queue->front->next;
                            free(temp);
                        }
                        if (cursorX > 0) {
                            cursorX--;
                        } else {
                            cursorX = COLS - 2;
                            cursorY--;
                        }
                    }
                }
                break;
            default:
                if (ch >= 0 && ch <= 127 && ch != 27) {
                    insertChar(queue, cursorY, cursorX, ch);
                    cursorX++;
                }
                break;
        }

        clear();
        printQueue(queue);

        move(cursorY, cursorX);
        refresh();
    }

    Node* currentNode = queue->front;
    while (currentNode != NULL) {
        Node* temp = currentNode;
        currentNode = currentNode->next;
        free(temp);
    }

    Queue* currentQueue = queue;
    while (currentQueue != NULL) {
        Queue* temp = currentQueue;
        currentQueue = currentQueue->down;
        free(temp);
    }

    endwin();

    return 0;
}
