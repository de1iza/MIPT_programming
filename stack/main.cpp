#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>
#include <wchar.h>

#define CANARY_ALIVE 0xAB
#define POISON 13377331
#define DEBUG 1

const size_t RESIZE_VAL = 20;

#ifdef DEBUG
    #define STACK_ASSERT(stack){                    \
        StackError error = CheckStack(stack);       \
        if (error) {                                \
            DumpStack(stack, error);                \
            exit(error);                            \
        }                                           \
    }
#else
    #define STACK_ASSERT(stack) ;
#endif

#define PRINT_CANARY(stack, canary_num){                    \
    if (canary_num == 1)                                    \
        printf("0x%x\n", (stack)->data[-1]);                \
    else                                                    \
        printf("0x%x\n", (stack)->data[stack->max_size]);   \
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
    PRINT_CANARY(stack, 1);                                                     \
    for (int i = 0; i < (stack)->max_size; ++i){                                \
        printf("! %s data[%02d]: %s %d ", CYAN, i, RESET, (stack)->data[i]);      \
        ShowElementStatus(stack->data[i]);                                      \
        printf("\n");                                                           \
    }                                                                           \
    PRINT_CANARY(stack, 2);                                                     \
}

#define PRINT_FLOATS(stack) {                                                   \
    PRINT_CANARY(stack, 1);                                                     \
    for (int i = 0; i < (stack)->size; ++i){                                    \
        printf("! %s data[%d]: %s %g\n", CYAN, i, RESET, (stack)->data[i]);     \
        ShowElementStatus(stack->data[i]);                                      \
        printf("\n");                                                           \
    }                                                                           \
    PRINT_CANARY(stack, 2);                                                     \
}

#define PRINT_CHARS(stack) {                                                    \
    PRINT_CANARY(stack, 1);                                                     \
    for (int i = 0; i < (stack)->size; ++i)  {                                  \
        printf("! %s data[%d]: %s %c\n", CYAN, i, RESET, (stack)->data[i]);     \
        ShowElementStatus(stack->data[i]);                                      \
        printf("\n");                                                           \
    }                                                                           \
    PRINT_CANARY(stack, 2);                                                     \
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
    unsigned long hash;
    int canary2;

};

enum StackError{
    OK,
    NULL_STACK_PTR,
    NULL_DATA_PTR,
    INVALID_STACK_SIZE,
    STRUCTURE_CANARY_DEAD,
    DATA_CANARY_DEAD,
    WRONG_HASH,
};

const char* error_strings[] = {"OK",
                               "NULL_STACK_PTR",
                               "NULL_DATA_PTR",
                               "INVALID_STACK_SIZE",
                               "STRUCTURE_CANARY_DEAD",
                               "DATA_CANARY_DEAD",
                               "WRONG_HASH"};

void StackInit(stack_t* stack, size_t max_size);
void StackDelete(stack_t* stack);
void StackPush(stack_t* stack, elem_t value);
elem_t StackPop(stack_t* stack);
void DumpStack(stack_t* stack, StackError error);
bool IsEmpty(stack_t* stack);
StackError CheckStack(stack_t* stack);
void ShowElementStatus(elem_t value);
void StackResize(stack_t* stack);
unsigned long GetStackHash(stack_t* stack);
void RewriteStackHash(stack_t* stack);


int main() {
    const int SIZE = 10;
    stack_t stk = {};

    STACK_INIT(stk, SIZE);

    StackPush(&stk, 34);
    StackPush(&stk, 32);
    StackPush(&stk, 1);
    StackPush(&stk, 1);
    StackPush(&stk, 1);
    StackPush(&stk, 1);
    StackPush(&stk, 1);

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
    stack->data = (elem_t*) calloc(max_size + 2, sizeof(elem_t)) + 1;
    //memset((stack)->data, -1, max_size * sizeof(elem_t));
    wmemset((wchar_t*) ((stack)->data - 1), POISON, max_size + 2);


    stack->data[-1] = (elem_t) CANARY_ALIVE;
    stack->data[max_size] = (elem_t) CANARY_ALIVE;

    stack->canary2 = CANARY_ALIVE;

    stack->hash = GetStackHash(stack);

    STACK_ASSERT(stack);


}

/*! Function to delete stack (destructor)
    @param stack pointer to stack structure
*/
void StackDelete(stack_t* stack){
    STACK_ASSERT(stack);

    free(stack->data - 1);
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

    if (stack->size == stack->max_size){
        StackResize(stack);
    }
    stack->data[stack->size++] = value;

    RewriteStackHash(stack);

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

    RewriteStackHash(stack);

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
    printf("! %s max_size: %-4s %d\n", CYAN, RESET, stack->max_size);
    printf("! %s size: %-8s %d\n", CYAN, RESET, stack->size);
    printf("! %s data %-9s [%p]\n", CYAN, RESET, stack->data);
    printf("! %s canary1: %-5s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary1, CANARY_ALIVE);
    printf("! %s canary2: %-5s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary2, CANARY_ALIVE);
    printf("! %s hash: %-8s %lu (Expected %lu)\n", CYAN, RESET, stack->hash, GetStackHash(stack));
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
        return STRUCTURE_CANARY_DEAD;
    if (stack->data[-1] != CANARY_ALIVE || stack->data[stack->max_size] != CANARY_ALIVE)
        return DATA_CANARY_DEAD;
    if (stack->hash != GetStackHash(stack))
        return WRONG_HASH;
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

/*! Resizes stack data array
    @param stack pointer to stack structure
*/
void StackResize(stack_t* stack){
    STACK_ASSERT(stack);

    int prev_size = stack->max_size;
    int new_size = prev_size + RESIZE_VAL + 2;

    void* new_mem_block = realloc((void*) (stack->data - 1), new_size * sizeof(elem_t));

    stack->data = (elem_t*) new_mem_block + 1;

    wmemset((wchar_t*)((stack)->data + prev_size), POISON, new_size - prev_size + 1);

    stack->max_size += RESIZE_VAL;

    stack->data[stack->max_size] = (elem_t) CANARY_ALIVE;

    STACK_ASSERT(stack);
}

/*! Calculates hash of stack structure
    @param stack pointer to stack structure
    @return hash calculated hash
*/
unsigned long GetStackHash(stack_t* stack){
    unsigned int hash = 0;
    for (int i = 0; i < stack->max_size; ++i){
        hash += stack->data[i] * (stack->data[i] << 3) + 4622041 ^ stack->data[i];
    }
    hash += stack->size * (stack->max_size >> 2) + 142091 * (long)stack->data;
    return hash;
}

/*! Writes new hash to stack structure
    @param stack pointer to stack structure
*/
void RewriteStackHash(stack_t* stack){
    stack->hash = GetStackHash(stack);
}