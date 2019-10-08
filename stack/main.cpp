/*
поставь условную компиляцию на канарейки и хеши в структуре и на их проверки( совет #ifdef DEBUG
                                                                                           #define $ON_DEBUG(code) code
                                                                                    #else
                                                                                           #define %ON_DEBUG(code)

убрать принты из функций \ +
*/

//  error_strings == copypaste. можно юзать define  \ ??
//  один файл плоха
//  мало проверок в конструкторе
//  нет ресайза обратно


#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <typeinfo>
#include <wchar.h>

#define DEBUG 1

#include "stack.h"


int Test();
int TestUnderflow();
int TestOverflow();
int TestBrokenArray();
int TestBrokenData();
int TestBrokenCanary();
int TestMemOverflow();


int main() {
    const int DEFAULT_SIZE = 10;
    //struct stack_t stk = {};
    //STACK_INIT(stk, -100);


    //StackDelete(&stk);
    //StackDelete(&stk);

    TestUnderflow();
    //TestOverflow();
    //TestBrokenArray();
    //TestBrokenData();
    //TestBrokenCanary();

    //Test();

    //TestMemOverflow();

    return 0;
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

    return 0;
}

int TestBrokenData(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i = 0; i < 5; i++)
        StackPush(&stk, 33);
    stk.size = 4;

    StackDelete(&stk);

    return 0;
}

int TestBrokenCanary(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i = 0; i < 5; i++)
        StackPush(&stk, 33);
    stk.canary1 = 111;

    StackDelete(&stk);

    return 0;
}

int TestMemOverflow(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i = 0; i < 100000; i++)
        StackPush(&stk, 33);


    StackDelete(&stk);

    return 0;
}

int Test(){
    stack_t stk = {};
    STACK_INIT(stk, 10);

    for (int i = 0; i < 100; i++) {
        StackPush(&stk, 55);
    }

    stk.max_size = 100000;

    for (int i = 0; i < 50; i++) {
        elem_t value = 0;
        StackPop(&stk, &value);
    }

    StackDelete(&stk);

    return 0;
}

