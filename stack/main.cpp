#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>
#include <wchar.h>

#define DEBUG 1

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
#define STACK_ASSERT(stack){                            \
            StackError error = CheckStack(stack);       \
            if (error) {                                \
                ABORT(stack, error, __LINE__);          \
            }                                           \
            if (ERRNO) {                                \
                ABORT(stack, ERRNO, __LINE__);          \
            }                                           \
        }
#else
    #define STACK_ASSERT(stack) ;
#endif

#define ABORT(stack, error, line){          \
    DumpStack(stack, error, line);          \
    exit(error);                            \
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
int Test();
int TestUnderflow();
int TestOverflow();
int TestBrokenArray();
int TestBrokenData();
int TestBrokenCanary();
void PrintCanary(stack_t* stack, int canary_num);
void PrintInts(stack_t* stack);
void PrintFloats(stack_t* stack);
void PrintChars(stack_t* stack);
void PrintStackElems(stack_t* stack);


int main() {
    const int DEFAULT_SIZE = 10;

    //Test();

   TestUnderflow();
    //TestOverflow();
    //TestBrokenArray();
    //TestBrokenData();
    //TestBrokenCanary();

    return 0;
}

/*! Prints first or last canary of data
    @param stack pointer to stack structure
    @param canary_num sequence number of data canary
*/
void PrintCanary(stack_t* stack, int canary_num){
    int valid_canary = 0;
    SetCanary((char*) &valid_canary, sizeof(elem_t));
    if (canary_num == 1)
        printf("0x%x (Expected 0x%x)\n", (stack)->data[-1], valid_canary);
    else
        printf("0x%x (Expected 0x%x)\n", (stack)->data[stack->max_size], valid_canary);
}

/*! Prints integer stack elements
    @param stack pointer to stack structure
*/
void PrintInts(stack_t* stack) {
    PrintCanary(stack, 1);
    for (int i = 0; i < (stack)->max_size; ++i){
        printf("! %s data[%02d]: %s %d ", CYAN, i, RESET, (stack)->data[i]);
        ShowElementStatus(stack->data[i]);
        printf("\n");
    }
    PrintCanary(stack, 2);
}

/*! Prints float stack elements
    @param stack pointer to stack structure
*/
void PrintFloats(stack_t* stack) {
    PrintCanary(stack, 1);
    for (int i = 0; i < (stack)->size; ++i){
        printf("! %s data[%d]: %s %g\n", CYAN, i, RESET, (stack)->data[i]);
        ShowElementStatus(stack->data[i]);
        printf("\n");
    }
    PrintCanary(stack, 2);
}

/*! Prints char stack elements
    @param stack pointer to stack structure
*/
void PrintChars(stack_t* stack) {
    PrintCanary(stack, 1);
    for (int i = 0; i < (stack)->size; ++i){
        printf("! %s data[%d]: %s %c\n", CYAN, i, RESET, (stack)->data[i]);
        ShowElementStatus(stack->data[i]);
        printf("\n");
    }
    PrintCanary(stack, 2);
}

/*! Prints stack elements depending on their type
    @param stack pointer to stack structure
*/
void PrintStackElems(stack_t* stack) {
    if (typeid((stack)->data[0]) == typeid(int)) { PrintInts(stack); }
    else if (typeid((stack)->data[0]) == typeid(float)) { PrintFloats(stack); }
    else if (typeid((stack)->data[0]) == typeid(char)) { PrintChars(stack); }
    else {
        fprintf(stderr, "Unknown data type");

    }
}

/*! Function to initialize stack (constructor)
    @param stack pointer to stack structure
    @param max_size size of a full stack
*/
void StackInit(stack_t* stack, size_t max_size){
    assert(max_size);

    SetCanary(&stack->canary1, sizeof(elem_t));
    SetCanary(&stack->canary2, sizeof(elem_t));

    stack->max_size = max_size;
    stack->size = 0;
    stack->data = (elem_t*) calloc(max_size + 2, sizeof(elem_t)) + 1;
    wmemset((wchar_t*) ((stack)->data - 1), POISON, max_size + 2);

    SetDataCanaries(stack->data, stack->max_size);

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
        StackResize(stack, RESIZE_VAL);
    }
    stack->data[stack->size++] = value;

    RewriteStackHash(stack);

    STACK_ASSERT(stack);

}

/*! Deletes top element from stack
    @param stack pointer to stack structure
    @return value top element
*/
bool StackPop(stack_t* stack, elem_t* value){
    STACK_ASSERT(stack);

    if (!IsEmpty(stack)) {
        *value = stack->data[--stack->size];
        stack->data[stack->size] = POISON;

        RewriteStackHash(stack);
    } else {
        #ifndef DEBUG
            printf("Stack is empty. Unable to pop.\n");
        #endif
        ERRNO = UNDERFLOW;

        STACK_ASSERT(stack);
        return 1;
    }

    STACK_ASSERT(stack);

    return 0;
}

/*! Prints full info about error and all stack structure fields
    @param stack pointer to stack structure
    @param error error code
*/
void DumpStack(stack_t* stack, StackError error, int line){
    printf("! %s ASSERTION FAILED %s in file %s, line %d\n", RED, RESET, __FILE__, line);
    printf("! %s %s %s\n", RED, error_strings[error], RESET);
    printf("!  %s \"%s\" [%p] \n", typeid(stack).name(), stack->name, stack);
    printf("! %s max_size: %-4s %d\n", CYAN, RESET, stack->max_size);
    printf("! %s size: %-8s %d\n", CYAN, RESET, stack->size);
    printf("! %s data %-9s [%p]\n", CYAN, RESET, stack->data);
    int valid_canary = 0;
    SetCanary(&valid_canary, sizeof(int));
    printf("! %s canary1: %-5s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary1, valid_canary);
    printf("! %s canary2: %-5s 0x%x (Expected 0x%x)\n", CYAN, RESET, stack->canary2, valid_canary);
    printf("! %s hash: %-8s %lu (Expected %lu)\n", CYAN, RESET, stack->hash, GetStackHash(stack));
    PrintStackElems(stack);
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
    if (stack->size == -1)
        return UNDERFLOW;
    if  (stack->size > stack->max_size || stack->size < 0)
        return INVALID_STACK_SIZE;
    if (!CanaryIsValid(&stack->canary1, sizeof(int)) || !CanaryIsValid(&stack->canary2, sizeof(int)) )
        return STRUCTURE_CANARY_DEAD;
    if (!CanaryIsValid(&(stack->data[-1]), sizeof(elem_t))  || !CanaryIsValid(&(stack->data[stack->max_size]), sizeof(elem_t)))
        return DATA_CANARY_DEAD;
    if (stack->hash != GetStackHash(stack))
        return WRONG_HASH;
    return OK;
}

/*! Checks if stack is empty
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
void StackResize(stack_t* stack, int resize_val){
    STACK_ASSERT(stack);

    int prev_size = stack->size;
    int new_size = prev_size + resize_val + 2;

    void* new_mem_block = (void*) realloc((stack)->data - 1, new_size * sizeof(elem_t));

    StackError error = MemoryOk(stack, new_mem_block);
    if (error) ERRNO = error;

    STACK_ASSERT(stack);

    stack->data = (elem_t*) new_mem_block + 1;

    wmemset((wchar_t*)(stack->data + prev_size), POISON, abs(new_size - prev_size) + 1);

    stack->max_size += resize_val;

    SetDataCanaries(stack->data, stack->max_size);

    RewriteStackHash(stack);

    STACK_ASSERT(stack);
}

/*! Calculates custom hash in range [first_byte, last_byte)
    @param first_byte byte to start calculating hash from
    @param last_byte byte to finish calculating hash with
    @return hash calculated hash
*/
unsigned long GetHash(void* first_byte, void* last_byte){
    unsigned int hash = 0;
    for (char* byte = (char*) first_byte; byte < (char*) last_byte; byte++){
        unsigned int int_byte = (unsigned int) *byte;
        hash += int_byte * (int_byte << 3) + 4622041 ^ int_byte;
    }

    return hash;
}

/*! Returnes hash of struct and data
    @param stack pointer to stack structure
*/
unsigned long GetStackHash(stack_t* stack){
    return GetHash(&stack->canary1, &stack->hash) +
    GetHash(&stack->canary2, &stack->canary2 + 1) +
    GetHash(&stack->data[-1], &stack->data[stack->max_size] + 1);
}

/*! Writes new hash to stack structure
    @param stack pointer to stack structure
*/
void RewriteStackHash(stack_t* stack){
    //STACK_ASSERT(stack);
    stack->hash = GetStackHash(stack);
}

/*! Checks if memory was allocated successfully
    @param stack pointer to stack structure
    @param block pointer to allocated memory
*/
StackError MemoryOk(stack_t* stack, void* block){
    if (block == nullptr) {
        return MEMORY_ALLOCATION_ERROR;
    }
    return OK;
}

/*! Sets front and back canaries to data array
    @param data array of elem_t
    @param size size of array
*/
void SetDataCanaries(elem_t* data, size_t size){
    SetCanary(data - 1, sizeof(elem_t));
    SetCanary(data + size, sizeof(elem_t));
}


/*! Fills each byte of canary with default canary byte to fit in elem_t array
    @param data pointer to canary
    @param size size of canary in bytes
*/
void SetCanary(void* canary_ptr, size_t canary_size){
    for (char* byte = (char*) canary_ptr, cnt = 0; cnt < canary_size; cnt++, byte++){
        *byte = CANARY_BYTE;
    }
}

/*! Checks if each byte of canary is valid
    @param data pointer to canary
    @param size size of canary in bytes
*/
bool CanaryIsValid(void* canary_ptr, size_t canary_size){
    for (char* byte = (char*) canary_ptr, cnt = 0; cnt < canary_size; cnt++, byte++){
        if (*byte != CANARY_BYTE)
            return false;
    }
    return true;
}

int TestUnderflow(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    elem_t val = 0;

    StackPop(&stk, &val);

    StackPush(&stk, 8);
    StackPop(&stk, &val);
    StackPop(&stk, &val);

    StackDelete(&stk);
    return 0;
}

int TestOverflow(){
    stack_t stk = {};
    STACK_INIT(stk, 10);
    for (int i = 0; i < 20; i++)
        StackPush(&stk, 5);

    StackDelete(&stk);

    return 0;
}

int TestBrokenArray(){
    stack_t stk = {};
    STACK_INIT(stk, 10);
    for (int i = 0; i < 5; i++)
        StackPush(&stk, 33);
    stk.data[2] = 45;

    StackDelete(&stk);
}

int TestBrokenData(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i = 0; i < 5; i++)
        StackPush(&stk, 33);
    stk.size = 4;

    StackDelete(&stk);
}

int TestBrokenCanary(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i = 0; i < 5; i++)
        StackPush(&stk, 33);
    stk.canary1 = 111;

    StackDelete(&stk);
}



int Test(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i =0; i<100; i++) {
        StackPush(&stk, 55);
    }

    stk.max_size = 100000;

    for (int i =0; i<50; i++) {
        elem_t value = 0;
        StackPop(&stk, &value);
    }

    StackDelete(&stk);

    return 0;
}