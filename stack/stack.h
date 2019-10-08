#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>
#include <wchar.h>

//#define DEBUG 1

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

const char CANARY_BYTE = 0xAB;
const int POISON  = 13377331;
const int RESIZE_VAL = 20;

#ifdef DEBUG
    #define STACK_ASSERT(stack){                        \
            StackError error = CheckStack(stack);       \
            if (error) {                                \
                DumpStack(stack, error, __LINE__);      \
                exit(error);                            \
            }                                           \
            if (ERRNO) {                                \
                DumpStack(stack, ERRNO, __LINE__);      \
                exit(ERRNO);                            \
            }                                           \
        }
#else
    #define STACK_ASSERT(stack) ;
#endif

#define PRINT_CANARY(stack, canary_num){                                                  \
    int valid_canary = 0;                                                                 \
    SetCanary((char*) &valid_canary, sizeof(elem_t));                                     \
    if (canary_num == 1)                                                                  \
        printf("0x%x (Expected 0x%x)\n", (stack)->data[-1], valid_canary);                \
    else                                                                                  \
        printf("0x%x (Expected 0x%x)\n", (stack)->data[stack->max_size], valid_canary);   \
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
        printf("! %s data[%02d]: %s %d ", CYAN, i, RESET, (stack)->data[i]);    \
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
    for (int i = 0; i < (stack)->size; ++i){                                    \
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


enum StackError{
    OK,
    NULL_STACK_PTR,
    NULL_DATA_PTR,
    INVALID_STACK_SIZE,
    STRUCTURE_CANARY_DEAD,
    DATA_CANARY_DEAD,
    WRONG_HASH,
    UNDERFLOW,
    MEMORY_ALLOCATION_ERROR,
};

StackError ERRNO = OK;

const char* error_strings[] = {"OK",
                               "NULL_STACK_PTR",
                               "NULL_DATA_PTR",
                               "INVALID_STACK_SIZE",
                               "STRUCTURE_CANARY_DEAD",
                               "DATA_CANARY_DEAD",
                               "WRONG_HASH",
                               "UNDERFLOW",
                               "MEMORY_ALLOCATION_ERROR"};


void StackInit(stack_t* stack, size_t max_size);
void StackDelete(stack_t* stack);
void StackPush(stack_t* stack, elem_t value);
bool StackPop(stack_t* stack, elem_t* value);
void DumpStack(stack_t* stack, StackError error, int line);
bool IsEmpty(stack_t* stack);
StackError CheckStack(stack_t* stack);
void ShowElementStatus(elem_t value);
void StackResize(stack_t* stack, int resize_val);
unsigned long GetHash(void* first_byte, void* last_byte);
unsigned long GetStackHash(stack_t* stack);
void RewriteStackHash(stack_t* stack);
StackError MemoryOk(stack_t* stack, void* block);
void SetDataCanaries(elem_t* data, size_t size);
void SetCanary(void* canary_ptr, size_t canary_size);
bool CanaryIsValid(void* canary_ptr, size_t canary_size);