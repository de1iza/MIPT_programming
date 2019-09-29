#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>

typedef int elem_t;

struct stack_t{
    int max_size;
    int size;
    const char* name;
    elem_t* data;

};

enum StackError{
    OK,
    NULL_STACK_PTR,
    NULL_DATA_PTR,
    INVALID_STACK_SIZE,
};

const char* error_strings[] = {"OK",
                               "NULL_STACK_PTR",
                               "NULL_DATA_PTR",
                               "INVALID_STACK_SIZE"};

#define STACK_ASSERT(stack){                    \
    StackError error = CheckStack(stack);       \
    if (error) {                                \
        DUMP_ERROR(stack, error);               \
        exit(error);                            \
    }                                           \
}

#define DUMP_ERROR(stack, error){                                       \
    printf("! ERROR in stack_t \"%s\" [%p]\n", stack->name, stack);     \
    printf("! %s\n", error_strings[error]);                             \
    printf("! max_size: %d\n", stack->max_size);                        \
    printf("! size: %d\n", stack->size);                                \
    printf("! data [%p]\n", stack->data);                               \
    if (error != INVALID_STACK_SIZE)                                    \
            PRINT_STACK_ELEMS(stack);                                   \
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

#define PRINT_INTS(stack) {                                   \
    for (int i = 0; i < (stack)->size; ++i)                   \
        printf("! data[%d] : %d\n", i, (stack)->data[i]);     \
}

#define PRINT_FLOATS(stack) {                                 \
    for (int i = 0; i < (stack)->size; ++i)                   \
        printf("! data[%d] : %g\n", i, (stack)->data[i]);     \
}

#define PRINT_CHARS(stack) {                                  \
    for (int i = 0; i < (stack)->size; ++i)                   \
        printf("! data[%d] : %c\n", i, (stack)->data[i]);     \
}

#define STACK_INIT(stack, max_size){    \
    stack.name = #stack;                \
    StackInit(&stack, max_size);        \
}

void StackInit(stack_t* stack, size_t max_size);
void StackDelete(stack_t* stack);
void StackPush(stack_t* stack, elem_t value);
elem_t StackPop(stack_t* stack);
bool IsEmpty(stack_t* stack);
StackError CheckStack(stack_t* stack);


int main() {
    const int SIZE = 10;

    stack_t stk = {};

    STACK_INIT(stk, SIZE);

    StackPush(&stk, 34);
    StackPush(&stk, 32);
    StackPush(&stk, 1);

    StackPop(&stk);
    StackPop(&stk);

    StackDelete(&stk);

    return 0;
}

void StackInit(stack_t* stack, size_t max_size){
    assert(max_size);

    stack->max_size = max_size;
    stack->size = 0;
    stack->data = (elem_t*) calloc(max_size, sizeof(elem_t));

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
    return OK;
}

bool IsEmpty(stack_t* stack){
    STACK_ASSERT(stack);

    return (stack->size == 0);
}
