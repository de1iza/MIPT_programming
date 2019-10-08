#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>
#include <wchar.h>



typedef int elem_t;

struct stack_t{
    int canary1 = 0;
    int max_size = -1;
    int size = -1;
    const char* name = "";
    elem_t* data = nullptr;
    unsigned long hash = 0;
    int canary2 = 0;
};

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

#ifdef DEBUG
#define STACK_ASSERT(stack){                        \
        bool stack_ok = CheckStack(stack);          \
        if (Errno) {                                \
            DumpStack(stack, Errno, __LINE__);      \
            exit(Errno);                            \
        }                                           \
}
#else
#define STACK_ASSERT(stack) ;
#endif


#define STACK_INIT(stack, max_size){    \
    stack.name = #stack;                \
    StackInit(&stack, max_size);        \
}

#define RED "\x1B[31m"
#define CYAN "\x1B[36m"
#define RESET "\x1B[0m"

const char CANARY_BYTE = 0xAB;
const int POISON  = 13377331;
const int RESIZE_VAL = 20;
StackError Errno = OK;

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
bool CheckStack(stack_t* stack);
void ShowElementStatus(elem_t value);
void StackResize(stack_t* stack, int resize_val);
unsigned long GetHash(void* first_byte, void* last_byte);
unsigned long GetStackHash(stack_t* stack);
void RewriteStackHash(stack_t* stack);
bool MemoryOk(stack_t* stack, void* block);
void SetDataCanaries(elem_t* data, size_t size);
void SetCanary(void* canary_ptr, size_t canary_size);
bool CanaryIsValid(void* canary_ptr, size_t canary_size);
void PrintCanary(stack_t* stack, int canary_num);
void PrintInts(stack_t* stack);
void PrintFloats(stack_t* stack);
void PrintChars(stack_t* stack);
void PrintStackElems(stack_t* stack);

#include "stack.cpp"