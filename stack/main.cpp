#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>

#define CANARY_ALIVE 0xABCD
#define POISON -1

#define STACK_ASSERT(stack){                    \
    StackError error = CheckStack(stack);       \
    if (error) {                                \
        DumpStack(stack, error);                \
        exit(error);                            \
    }                                           \
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

void StackInit(stack_t* stack, size_t max_size);
void StackDelete(stack_t* stack);
void StackPush(stack_t* stack, elem_t value);
elem_t StackPop(stack_t* stack);
void DumpStack(stack_t* stack, StackError error);
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

/*! Function to initialize stack (constructor)
    @param stack pointer to stack structure
    @param max_size size of a full stack
*/
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

/*! Function to delete stack (destructor)
    @param stack pointer to stack structure
*/
void StackDelete(stack_t* stack){
    STACK_ASSERT(stack);

    free(stack->data);
    stack->data = nullptr;
    stack->size = 0;
    stack->max_size = 0;

}

/*! Adds new element to stack
    @param stack pointer to stack structure
    @param value element to add
*/
void StackPush(stack_t* stack, elem_t value){
    STACK_ASSERT(stack);

    stack->data[stack->size++] = value;

    STACK_ASSERT(stack);

}

/*! Deletes top element from stack
    @param stack pointer to stack structure
    @return value top element
*/
elem_t StackPop(stack_t* stack){
    STACK_ASSERT(stack);

    elem_t value = stack->data[--stack->size];
    stack->data[stack->size] = POISON;

    STACK_ASSERT(stack);

    return value;
}

/*! Prints full info about error and all stack structure fields
    @param stack pointer to stack structure
    @param error error code
*/
void DumpStack(stack_t* stack, StackError error){
    printf("! %s ASSERTION FAILED %s in file %s, line %d\n", RED, RESET, __FILE__, __LINE__ );
    printf("! %s %s %s\n", RED, error_strings[error], RESET);
    printf("!  %s \"%s\" [%p] \n", typeid(stack).name(), stack->name, stack);
    printf("! %s max_size: %s %d\n", CYAN, RESET, stack->max_size);
    printf("! %s size: %s %d\n", CYAN, RESET, stack->size);
    printf("! %s data %s [%p]\n", CYAN, RESET, stack->data);
    printf("! %s canary1: %s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary1, CANARY_ALIVE);
    printf("! %s canary2: %s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary2, CANARY_ALIVE);
    PRINT_STACK_ELEMS(stack);
}

/*! Validates stack fields and returns error code (or 0 if everything is ok)
    @param stack pointer to stack structure
    @return error code
*/
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

/*! Checks is stack is empty
    @param stack pointer to stack structure
*/
bool IsEmpty(stack_t* stack){
    STACK_ASSERT(stack);

    return (stack->size == 0);
}

/*! Prints "poison" label if stack element is poisoned
    @param value stack element
*/
void ShowElementStatus(elem_t value){
    if (value == POISON)  printf(RED "(POISON)" RESET);
}