#include <ncurses.h>
#include <stdlib.h>

typedef struct Node {
    char character;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

void enqueue(Queue* queue, char character) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->character = character;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

void dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return;
    }

    Node* temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
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
                enqueue(queue, '\n');
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
                            if (queue->rear == currentNode) {
                                queue->rear = prevNode;
                            }
                            free(currentNode);
                        } else {
                            Node* temp = queue->front;
                            queue->front = queue->front->next;
                            free(temp);
                            if (queue->front == NULL) {
                                queue->rear = NULL;
                            }
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
                if (ch >= 0 && ch <= 127 && ch != 27) { // Exclude special characters
                    enqueue(queue, ch);
                    cursorX++;
                }
                break;
        }

        clear();
        Node* currentNode = queue->front;
        int i, j;
        for (i = 0; i < LINES - 1 && currentNode != NULL; i++) {
            for (j = 0; j < COLS - 1 && currentNode != NULL; j++) {
                mvprintw(i, j, "%c", currentNode->character);
                currentNode = currentNode->next;
            }
        }

        move(cursorY, cursorX);
        refresh();
    }

    Node* currentNode = queue->front;
    while (currentNode != NULL) {
        Node* temp = currentNode;
        currentNode = currentNode->next;
        free(temp);
    }

    free(queue);

    endwin();

    return 0;
}
