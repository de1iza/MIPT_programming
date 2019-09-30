#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>

#define CANARY_ALIVE 0xABCD
#define POISON -1

typedef int elem_t;

struct stack_t{
    int canary1;
    int max_size;
    int size;
    const char* name;
    elem_t* data;
    int canary2;

};

enum StackError{
    OK,
    NULL_STACK_PTR,
    NULL_DATA_PTR,
    INVALID_STACK_SIZE,
    CANARY_DEAD,
};

const char* error_strings[] = {"OK",
                               "NULL_STACK_PTR",
                               "NULL_DATA_PTR",
                               "INVALID_STACK_SIZE",
                               "CANARY_DEAD"};

#define STACK_ASSERT(stack){                    \
    StackError error = CheckStack(stack);       \
    if (error) {                                \
        DUMP_ERROR(stack, error);               \
        exit(error);                            \
    }                                           \
}

#define DUMP_ERROR(stack, error){                                                                          \
    printf("! %s ERROR in %s \"%s\" [%p] %s \n", RED, typeid(stack).name(), stack->name, stack, RESET);    \
    printf("! %s %s %s\n", RED, error_strings[error], RESET);                                              \
    printf("! %s max_size: %s %d\n", CYAN, RESET, stack->max_size);                                        \
    printf("! %s size: %s %d\n", CYAN, RESET, stack->size);                                                \
    printf("! %s data %s [%p]\n", CYAN, RESET, stack->data);                                               \
    printf("! %s canary1: %s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary1, CANARY_ALIVE);          \
    printf("! %s canary2: %s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary2, CANARY_ALIVE);          \
    if (error != INVALID_STACK_SIZE || error == INVALID_STACK_SIZE)                                        \
            PRINT_STACK_ELEMS(stack);                                                                      \
}

#define PRINT_STACK_ELEMS(stack){                                                   \
    if (typeid((stack)->data[0]) == typeid(int)) { PRINT_INTS(stack); }             \
    else if (typeid((stack)->data[0]) == typeid(float)) { PRINT_FLOATS(stack); }    \
    else if (typeid((stack)->data[0]) == typeid(char)) { PRINT_CHARS(stack); }      \
    else {                                                                          \
        fprintf(stderr, "Unknown data type");                                       \
        exit(-1);                                                                   \
    }                                                                               \
}

#define PRINT_INTS(stack) {                                                     \
    for (int i = 0; i < (stack)->max_size; ++i){                                \
        printf("! %s data[%d]: %s %d ", CYAN, i, RESET, (stack)->data[i]);      \
        ShowElementStatus(stack->data[i]);                                      \
        printf("\n");                                                           \
    }                                                                           \
}

#define PRINT_FLOATS(stack) {                                                   \
    for (int i = 0; i < (stack)->size; ++i){                                    \
        printf("! %s data[%d]: %s %g\n", CYAN, i, RESET, (stack)->data[i]);     \
        ShowElementStatus(stack->data[i]);                                      \
        printf("\n");                                                           \
    }                                                                           \
}

#define PRINT_CHARS(stack) {                                                    \
    for (int i = 0; i < (stack)->size; ++i)  {                                  \
        printf("! %s data[%d]: %s %c\n", CYAN, i, RESET, (stack)->data[i]);     \
        ShowElementStatus(stack->data[i]);                                      \
        printf("\n");                                                           \
    }                                                                           \
}

#define STACK_INIT(stack, max_size){    \
    stack.name = #stack;                \
    StackInit(&stack, max_size);        \
}

#define RED "\x1B[31m"
#define CYAN "\x1B[36m"
#define RESET "\x1B[0m"

void StackInit(stack_t* stack, size_t max_size);
void StackDelete(stack_t* stack);
void StackPush(stack_t* stack, elem_t value);
elem_t StackPop(stack_t* stack);
bool IsEmpty(stack_t* stack);
StackError CheckStack(stack_t* stack);
void ShowElementStatus(elem_t value);


int main() {
    const int SIZE = 10;
    stack_t stk = {};

    STACK_INIT(stk, SIZE);

    StackPush(&stk, 34);
    StackPush(&stk, 32);
    StackPush(&stk, 1);

    StackPop(&stk);
    StackPop(&stk);
    StackPop(&stk);
    StackPop(&stk);

    StackDelete(&stk);

    return 0;
}

void StackInit(stack_t* stack, size_t max_size){
    assert(max_size);

    stack->canary1 = CANARY_ALIVE;
    stack->max_size = max_size;
    stack->size = 0;
    stack->data = (elem_t*) calloc(max_size, sizeof(elem_t));
    memset((stack)->data, -1, max_size * sizeof(elem_t));
    stack->canary2 = CANARY_ALIVE;

    STACK_ASSERT(stack);

}

void StackDelete(stack_t* stack){
    STACK_ASSERT(stack);

    free(stack->data);
    stack->data = nullptr;
    stack->size = 0;
    stack->max_size = 0;

}

void StackPush(stack_t* stack, elem_t value){
    STACK_ASSERT(stack);

    stack->data[stack->size++] = value;

    STACK_ASSERT(stack);

}

elem_t StackPop(stack_t* stack){
    STACK_ASSERT(stack);

    elem_t value = stack->data[--stack->size];
    stack->data[stack->size] = POISON;

    STACK_ASSERT(stack);

    return value;
}


StackError CheckStack(stack_t* stack){
    if (stack == nullptr)
        return NULL_STACK_PTR;
    if (stack->data == nullptr)
        return NULL_DATA_PTR;
    if (stack->size < 0 || stack->size > stack->max_size)
        return INVALID_STACK_SIZE;
    if (stack->canary1 != CANARY_ALIVE || stack->canary2 != CANARY_ALIVE)
        return CANARY_DEAD;
    return OK;
}

bool IsEmpty(stack_t* stack){
    STACK_ASSERT(stack);

    return (stack->size == 0);
}

void ShowElementStatus(elem_t value){
    if (value == POISON)  printf(RED "(POISON)" RESET);
}