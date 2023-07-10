#include <ncurses.h>
#include <stdlib.h>
/* Estrutura de nó */
typedef struct Node {
    char character;
    struct Node* next;
    struct Node* down;
} Node;

/* Estrutura de fila */
typedef struct Queue {
    Node* front;
    struct Queue* down;
} Queue;

/* Cria uma fila vazia */
Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->down = NULL;
    return queue;
}

/* Adiciona elementos na fila */
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

/* retira elementos da fila */
void dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return;
    }

    Node* temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
}

/* Insere um tipo char na lista */
void insertChar(Queue* queue, int line, int position, char character) {
    Queue* currentQueue = queue;
    int i;
    /* Itera até atingir a queue que se encontra o cursor */
    for (i = 0; i < line && currentQueue != NULL; i++) {
        currentQueue = currentQueue->down;
    }

    if (currentQueue == NULL) {
        return;
    }

    Node* currentNode = currentQueue->front;
    Node* prevNode = NULL;

    /* Itera até atingir o Node que o cursor se encontra */
    int j = 0;
    while (j < position && currentNode != NULL) {
        prevNode = currentNode;
        currentNode = currentNode->next;
        j++;
    }

    /* Insere o char no node selecionado */
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->character = character;
    newNode->next = currentNode;

    /* Atualiza o Node */
    if (prevNode == NULL) {
        currentQueue->front = newNode;
    } else {
        prevNode->next = newNode;
    }
}

/* Deleta um tipo char da lista (BACKSPACE) */
void deleteChar(Queue* queue, int line, int position) {
    Queue* currentQueue = queue;
    int i;
    /* Itera até a Queue selecionada pelo cursor (linha)*/
    for (i = 0; i < line && currentQueue != NULL; i++) {
        currentQueue = currentQueue->down;
    }

    /* Tratamento de erros */
    if (currentQueue == NULL) {
        return;
    }

    Node* currentNode = currentQueue->front;
    Node* prevNode = NULL;

    /* Itera até encontrar o node apontado pelo cursor */
    int j = 0;
    while (j < position && currentNode != NULL) {
        prevNode = currentNode;
        currentNode = currentNode->next;
        j++;
    }

    /* Deleta o node atual e liga os nodes adjacentes */
    if (currentNode != NULL) {
        if (prevNode == NULL) {
            currentQueue->front = currentNode->next;
        } else {
            prevNode->next = currentNode->next;
        }
        free(currentNode);
    }
}

/* Cria uma nova linha (Nova fila ligada a fila de cima) */
void insertLine(Queue* queue, int position) {
    if (position < 0) {
        return;
    }

    /* Procura a Queue selecionada */
    Queue* currentQueue = queue;
    int i;
    for (i = 0; i < position; i++) {
        if (currentQueue->down == NULL) {
            Queue* newQueue = createQueue();
            currentQueue->down = newQueue;
        }
        currentQueue = currentQueue->down;
    }

    /* Adiciona o primeiro elemento da queue */
    if (currentQueue->front == NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->character = '\n';
        newNode->next = NULL;
        newNode->down = NULL;
        currentQueue->front = newNode;
    }
}

/* Printa os elementos de uma fila */
void printQueue(Queue* queue) {
    Queue* currentQueue = queue;
    int i = 0;

    /* percorre os elementos da queue e mostra na tela */
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
    /* Funções do ncurses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    /* primeira linha */
    Queue* queue = createQueue();

    int cursorX = 0, cursorY = 0;

    int ch;
    while ((ch = getch()) != KEY_F(1)) {
        switch (ch) {
            /* As setas alteram o valor da posição do cursor no eixo cartesiano */
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
                    if (cursorX > 0) {
                        deleteChar(queue, cursorY, cursorX - 1);
                        cursorX--;
                    } else {
                        Queue* currentQueue = queue;
                        int i;
                        for (i = 0; i < cursorY - 1 && currentQueue != NULL; i++) {
                            currentQueue = currentQueue->down;
                        }
                        if (currentQueue != NULL) {
                            Node* currentNode = currentQueue->front;
                            while (currentNode->next != NULL) {
                                currentNode = currentNode->next;
                            }
                            if (currentNode->character == '\n') {
                                deleteChar(queue, cursorY - 1, COLS - 2);
                                cursorY--;
                                cursorX = COLS - 2;
                            }
                        }
                    }
                }
                break;
            /* Caso o botão não seja um botão e funçção, seu valor é inserido na queue */
            default:
                if (ch >= 0 && ch <= 127 && ch != 27) {
                    insertChar(queue, cursorY, cursorX, ch);
                    cursorX++;
                }
                break;
        }

        /* Atualiza a queue na tela */
        clear();
        printQueue(queue);

        move(cursorY, cursorX);
        refresh();
    }

    /* 
        Quando o while se encerra (programa desliga) 
        Limpa as memórias alocadas;
    */
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

    /* Finalização do ncurses */
    endwin();

    return 0;
}
