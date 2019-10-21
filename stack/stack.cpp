


/*! Function to initialize stack (constructor)
    @param stack pointer to stack structure
    @param max_size size of a full stack
*/
void StackInit(stack_t* stack, size_t max_size){
    assert(max_size > 0);
    assert(stack);

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
    stack->size = -1;
    stack->max_size = -1;
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
        Errno = UNDERFLOW;

        STACK_ASSERT(stack);
        return true;
    }

    STACK_ASSERT(stack);

    return false;
}

#define GET_ERROR_STRING(error){    \
    printf(#error);                 \
}

/*! Prints full info about error and all stack structure fields
    @param stack pointer to stack structure
    @param error error code
*/
void DumpStack(stack_t* stack, StackError error, int line){
    printf("! %s ASSERTION FAILED %s in file %s, line %d\n", RED, RESET, __FILE__, line);
    //GET_ERROR_STRING(error);
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
    @return true if stack is valid
*/
bool CheckStack(stack_t* stack){
    if (stack == nullptr)
        Errno = NULL_STACK_PTR;
    else if (stack->data == nullptr)
        Errno = NULL_DATA_PTR;
    else if (stack->size == -1)
        Errno = UNDERFLOW;
    else if  (stack->size > stack->max_size || stack->size < 0)
        Errno = INVALID_STACK_SIZE;
    else if (!CanaryIsValid(&stack->canary1, sizeof(int)) || !CanaryIsValid(&stack->canary2, sizeof(int)) )
        Errno = STRUCTURE_CANARY_DEAD;
    else if (!CanaryIsValid(&(stack->data[-1]), sizeof(elem_t))  || !CanaryIsValid(&(stack->data[stack->max_size]), sizeof(elem_t)))
        Errno = DATA_CANARY_DEAD;
    else if (stack->hash != GetStackHash(stack))
        Errno = WRONG_HASH;
    else
        return true;
    return false;
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

    bool error = MemoryOk(stack, new_mem_block);

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
bool MemoryOk(stack_t* stack, void* block){
    if (block == nullptr) {
        Errno = MEMORY_ALLOCATION_ERROR;
        return false;
    }
    return true;
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
