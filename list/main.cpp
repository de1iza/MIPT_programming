#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <algorithm>

#define CHECK_FULL_LIST                     \
    if (size == max_size) {                 \
        fprintf(stderr, "List is full\n");  \
        return default_ret;                 \
    }

#define CHECK_INDEX                                 \
    if (!IndexIsValid(index)) {                     \
        fprintf(stderr, "Index out of range\n");    \
        return default_ret;                         \
    }

#define CHECK_ELEM_IN_LIST                                      \
    if (prev[index] == POISON &&  index != head) {              \
        fprintf(stderr, "This element is not in the list\n");   \
        return default_ret;                                     \
    }

const int POISON = 13377331;


bool TestInsertFirst();
bool TestInsertLast();
bool TestInsertAfter();
bool TestInsertBefore();
bool TestGetElemByPos();
bool TestDeleteElem();
bool TestDump();

template <typename T>
class List_t {
private:
    T* data;
    int* next;
    int* prev;
    int head;
    int tail;
    int next_free;
    size_t size;
    size_t max_size;
    bool sorted;
    bool IndexIsValid(const int index);
    bool ListIsValid();
    bool Resize(const int resize_val = 20);
public:
    List_t(size_t max_size);
    ~List_t();
    bool isEmpty();
    int InsertBefore(const int index, const T value);
    int InsertAfter(const int index, const T value);
    int InsertFirst(const T value);
    int InsertLast(const T value);
    T GetHead();
    T GetTail();
    T GetAfter(const int index);
    T GetBefore(const int index);
    bool DeleteElem(const int index);
    bool ClearList();
    void Dump(const char* filename = "dump.dot");

    size_t GetSize();

    void ShowList();
    T* GetHeadP();
    T* GetTailP();
    T* GetAfterP(const int index);
    T* GetBeforeP(const int index);
    T GetElemByPosition(const int position);
    void Sort();
};


int main() {
    TestInsertFirst();
    TestInsertLast();
    TestInsertAfter();
    TestInsertBefore();
    TestGetElemByPos();
    TestDeleteElem();
    TestDump();

    return 0;
}

template<typename T>
List_t<T>::List_t(size_t max_size): size(0), head(0), tail(0), next_free(0), max_size(max_size), sorted(false) {
    data = (T*) calloc(max_size, sizeof(T));
    wmemset((wchar_t*) data, (T) POISON, max_size);

    next = (int*) calloc(max_size, sizeof(int));
    for (int i = 0; i < max_size - 1; i++) {
        next[i] = i + 1;
    }
    next[max_size - 1] = POISON;

    prev = (int*) calloc(max_size, sizeof(int));
    wmemset((wchar_t*) prev, (T) POISON, max_size);
}

template<typename T>
List_t<T>::~List_t() {
    free(data);
    free(next);
    free(prev);
}

template<typename T>
int List_t<T>::InsertAfter(const int index, const T value) {
    if (!ListIsValid()) return -1;
    int default_ret = -1;
    if (size == max_size) {
        Resize();
    }
    CHECK_FULL_LIST;
    CHECK_INDEX;

    if (index == tail) {
        return InsertLast(value);
    }

    int pos = next_free;
    next_free = next[pos];

    data[pos] = value;
    prev[pos] = index;
    next[pos] = next[index];

    prev[next[index]] = pos;
    next[index] = pos;

    sorted = false;
    size++;

    return pos;
}

template<typename T>
int List_t<T>::InsertBefore(const int index, const T value) {
    if (!ListIsValid()) return -1;
    bool default_ret = -1;
    if (size == max_size) {
        Resize();
    }
    CHECK_FULL_LIST;
    CHECK_INDEX;
    CHECK_ELEM_IN_LIST;
    if (index == head) {
        return InsertFirst(value);
    }
    return InsertAfter(prev[index], value);
}

template<typename T>
int List_t<T>::InsertFirst(const T value) {
    if (!ListIsValid()) return -1;
    bool default_ret = -1;
    if (size == max_size) {
        Resize();
    }
    CHECK_FULL_LIST;
    int pos = next_free;
    data[pos] = value;

    next_free = next[pos];

    if (isEmpty()) {
        tail = pos;
        next[pos] = POISON;
        sorted = true;
    }
    else {
        next[pos] = head;
        prev[head] = pos;
        sorted = false;
    }


    head = pos;

    size++;
    return pos;
}

template<typename T>
int List_t<T>::InsertLast(const T value) {
    if (!ListIsValid()) return -1;
    bool default_ret = -1;
    if (size == max_size) {
        Resize();
    }
    CHECK_FULL_LIST;
    int pos = next_free;
    next_free = next[pos];

    data[pos] = value;

    next[pos] = POISON;
    next[tail] = pos;

    if (isEmpty()) {
        head = pos;
        next[pos] = POISON;
        sorted = true;
    }
    else {
        prev[pos] = tail;
    }
    tail = pos;

    size++;
    return pos;
}

template<typename T>
T List_t<T>::GetHead() {
    return data[head];
}

template<typename T>
T List_t<T>::GetTail() {
    return data[tail];
}

template<typename T>
T List_t<T>::GetAfter(const int index) {
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return POISON;
    }
    return data[next[index]];
}

template<typename T>
T List_t<T>::GetBefore(const int index) {
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return POISON;
    }
    return data[prev[index]];
}

template<typename T>
bool List_t<T>::IndexIsValid(const int index) {
    return (index >= 0 && index <= max_size - 1);
}

template<typename T>
bool List_t<T>::DeleteElem(const int index) {
    if (!ListIsValid()) return false;
    bool default_ret = false;
    CHECK_INDEX;
    CHECK_ELEM_IN_LIST;

    if (index == head) {
        head = next[head];
        prev[next[index]] = POISON;
        sorted = false;
    }
    else if (index == tail) {
        tail = prev[tail];
        next[prev[index]] = POISON;
    }
    else {
        next[prev[index]] = next[index];
        prev[next[index]] = prev[index];
        sorted = false;
    }

    data[index] = POISON;
    next[index] = next_free;
    prev[index] = POISON;

    next_free = index;
    size--;

    return true;
}

template<typename T>
void List_t<T>::Dump(const char* filename) {
    if (!ListIsValid()) return;

    FILE* file = fopen(filename, "w");

    fprintf(file, "digraph list{\n"
                  "rankdir=LR;\n"
                  "forcelabels=true;\n"
                  "node [shape=record, fontname=\"Lobster\"];\n");

    fprintf(file, "head [shape=circle];\n");
    fprintf(file, "tail [shape=circle];\n");

    if (!isEmpty()) {
        int cur_elem = head;
        int cur_size = 0;
        while (cur_size < size) {
            fprintf(file, "node%d [xlabel=\"%d\"  label=\"{PREV\\n%d | DATA\\n%d | NEXT\\n%d}\"];\n", cur_size, cur_elem, prev[cur_elem], data[cur_elem], next[cur_elem]);
            cur_elem = next[cur_elem];
            cur_size++;
        };

        fprintf(file, "head -> node0\n");
        fprintf(file, "node%d -> tail [dir=\"back\"]\n", size - 1);

        fprintf(file, "edge [dir=\"both\"]\n");

        for (int i = 0; i < size - 1; i++) {
            fprintf(file, "node%d -> ", i);
        }
        fprintf(file, "node%d\n", size - 1);
    }
    else {
        fprintf(file, "head -> tail\n");
    }
    fprintf(file, "}");

    fclose(file);

    system("dot -Tpng dump.dot -Gdpi=300 -o dump.png");
}

template<typename T>
size_t List_t<T>::GetSize() {
    return size;
}

template<typename T>
void List_t<T>::ShowList() {
    printf("data: ");
    for (int i = 0; i < max_size; i++) {
        printf("%d ", data[i]);
    } printf("\n");
    printf("next: ");
    for (int i = 0; i < max_size; i++) {
        printf("%d ", next[i]);
    } printf("\n");
    printf("prev: ");
    for (int i = 0; i < max_size; i++) {
        printf("%d ", prev[i]);
    } printf("\n");
    printf("SIZE %d\n", size);
    printf("NEXT FREE %d\n", next_free);
    printf("HEAD %d TAIL %d\n\n", head, tail);
}

template<typename T>
bool List_t<T>::ClearList() {
    size = 0;
    head = 0;
    tail = 0;
    next_free = 0;
    sorted = false;

    wmemset((wchar_t*) data, (T) POISON, max_size);

    for (int i = 0; i < max_size - 1; i++) {
        next[i] = i + 1;
    }
    next[max_size - 1] = POISON;

    wmemset((wchar_t*) prev, (T) POISON, max_size);
    return true;
}

template<typename T>
bool List_t<T>::isEmpty() {
    return size == 0;
}

template<typename T>
T *List_t<T>::GetHeadP() {
    if (isEmpty())
        return nullptr;
    else {
        return &data[head];
    }
}

template<typename T>
T *List_t<T>::GetTailP() {
    if (isEmpty())
        return nullptr;
    else {
        return &data[tail];
    }
}

template<typename T>
T *List_t<T>::GetAfterP(const int index) {
    T* default_ret = nullptr;
    CHECK_INDEX;
    CHECK_ELEM_IN_LIST;
    if (index == tail) {
        return nullptr;
    }
    else
        return &next[index];
}

template<typename T>
T *List_t<T>::GetBeforeP(const int index) {
    T* default_ret = nullptr;
    CHECK_INDEX;
    CHECK_ELEM_IN_LIST;
    if (index == head) {
        return nullptr;
    }
    else
        return &prev[index];
}

template<typename T>
T List_t<T>::GetElemByPosition(const int position) {
    if (!ListIsValid()) return false;
    if (position >= size || position < 0) {
        fprintf(stderr, "List doesn't contain element on this position\n");
        return POISON;
    }
    if (sorted) {
        return data[position];
    }
    else {
        if (position > size / 2) {
            int i = size - position - 1;
            int cur_ind = tail;
            while (i--) {
                cur_ind = prev[cur_ind];
            }
            return data[cur_ind];
        }

        int i = position;
        int cur_ind = head;
        while (i--) {
            cur_ind = next[cur_ind];
        }
        return data[cur_ind];
    }
}

template<typename T>
void List_t<T>::Sort() {
    if (!ListIsValid()) return;
    T cur_ind = head;

    int i = 0;

    while (i < size) {
        prev[i++] = data[cur_ind];
        cur_ind = next[cur_ind];
    }

    memcpy(data, prev, size * sizeof(T));
    std::fill_n(prev, max_size, (T) POISON);

    for (int i = 0; i < size - 1; i++) {
        next[i] = i + 1;
        prev[i + 1] = i;
    }

    for (int i = size; i < max_size - 1; i++) {
        next[i] = i;
    }

    next[size - 1] = POISON;
    prev[0] = POISON;

    next_free = size;
    head = 0;
    tail = size - 1;

}

template<typename T>
bool List_t<T>::ListIsValid() {
    if (max_size < 0) {
        fprintf(stderr, "Invalid max_size: %d\n", max_size);
        return false;
    }
    if (size > max_size) {
        fprintf(stderr, "Invalid size: %d\n", size);
        return false;
    }
    if (size) {
        if (data[head] == POISON) {
            fprintf(stderr, "Invalid head index: %d\n", head);
            return false;
        }
        if (data[tail] == POISON) {
            fprintf(stderr, "Invalid tail index: %d\n", tail);
            return false;
        }
    }
    if (next_free != POISON) {
        if (data[next_free] != POISON) {
            fprintf(stderr, "Invalid next_free index (is not free): %d\n", next_free);
            return false;
        }
    }

    int index = head;
    int cur_size = 0;
    bool default_ret = false;
    while (cur_size < size) {
        if (index != head) {
            if (index != tail) {
                if (next[index] == POISON || prev[index] == POISON) {
                    fprintf(stderr, "Error in linking elements. Element with index %d is not in list\n", index);
                    return false;
                }
                if (prev[next[index]] != index || next[prev[index]] != index) {
                    fprintf(stderr, "Error in linking elements. Prev or next index of element with index %d is broken\n", index);
                    return false;
                }
            }
            else {
                if (next[prev[index]] != index){
                    fprintf(stderr, "Error in linking elements. Tail element with index %d is broken\n", index);
                    return false;
                }
            }
        }
        else {
            if (size > 1 && prev[next[index]] != index){
                fprintf(stderr, "Error in linking elements. Head element with index %d is broken\n", index);
                return false;
            }
        }
        cur_size++;
    }

    return true;
}

template<typename T>
bool List_t<T>::Resize(const int resize_val) {
    T* new_data = (T*) realloc(data, (max_size + resize_val) * sizeof(T));
    int* new_next = (int*) realloc(next, (max_size + resize_val) * sizeof(int));
    int* new_prev = (int*) realloc(prev, (max_size + resize_val) * sizeof(int));

    if (new_data == nullptr || new_prev == nullptr || new_next == nullptr) {
        fprintf(stderr, "Memory allocation error\n");
        return false;
    }

    data = new_data;
    next = new_next;
    prev = new_prev;

    wmemset((wchar_t*) data + max_size, (T) POISON, resize_val);
    wmemset((wchar_t*) prev + max_size, (T) POISON, resize_val);


    for (int i = max_size; i < max_size + resize_val - 1; i++) {
        next[i] = i + 1;
    }
    next[max_size + resize_val - 1] = POISON;
    next_free = max_size;

    max_size += resize_val;

    return true;
}

bool TestInsertFirst() {
    List_t<int> list = List_t<int>(10);
    list.InsertFirst(33);
    int size = -1;
    if ((size = list.GetSize()) == 1) {
        int res = 0;
        if ((res = list.GetHead())== 33) {
            printf("TestInsertFirst passed\n");
            return true;
        }
        printf("TestInsertFirst failed. Wrong head element: Expected 33, got %d\n", res);
        return false;
    }
    printf("TestInsertFirst failed. Wrong size: Expected 1, got %d\n", size);
    return false;
}

bool TestInsertLast() {
    List_t<int> list = List_t<int>(10);
    list.InsertFirst(33);
    list.InsertLast(44);

    int res = 0;
    if ((res = list.GetTail()) == 44) {
        printf("TestInsertLast passed\n");
        return true;
    }
    printf("TestInsertLast failed. Wrong tail element: Expected 44, got %d\n", res);
    return false;
}

bool TestInsertAfter() {
    List_t<int> list = List_t<int>(10);
    list.InsertFirst(22);
    list.InsertLast(13);
    for (int i = 0; i < 15; i++)
        list.InsertAfter(0, 34);
    int res = 0;
    if ((res = list.GetAfter(0)) == 34) {
        printf("TestInsertAfter passed\n");
        return true;
    }
    printf("TestInsertAfter failed. Wrong element: Expected 34, got %d\n", res);
    return false;
}

bool TestInsertBefore() {
    List_t<int> list = List_t<int>(10);
    list.InsertFirst(22);
    list.InsertLast(13);
    list.InsertLast(49);
    list.InsertBefore(2, 87);
    int res = 0;
    if ((res = list.GetBefore(2)) == 87) {
        printf("TestInsertBefore passed\n");
        return true;
    }
    printf("TestInsertBefore failed. Wrong element: Expected 87, got %d\n", res);
    return false;
}

bool TestGetElemByPos() {
    List_t<int> list = List_t<int>(10);
    list.InsertFirst(22);
    list.InsertLast(13);
    list.InsertAfter(0, 70);
    int res = 0;
    if ((res = list.GetElemByPosition(1)) == 70) {
        printf("TestGetElemByPos passed\n");
        return true;
    }
    printf("TestGetElemByPos failed. Wrong element: Expected 70, got %d\n", res);
    return false;
}

bool TestDeleteElem() {
    List_t<int> list = List_t<int>(10);
    list.InsertFirst(22);
    list.InsertLast(13);
    list.InsertAfter(0, 70);

    list.DeleteElem(2);

    list.Dump();

    if (list.GetTail() == 13) {
        printf("TestDeleteElem passed\n");
        return true;
    }
    printf("TestDeleteElem failed.\n");
    return false;
}

bool TestDump() {
    List_t<int> list = List_t<int>(10);
    list.InsertLast(5);
    int test_index = list.InsertLast(22);
    list.InsertLast(130);
    list.InsertLast(13);
    for (int i = 0; i < 10; i++)
        list.InsertLast(i+100);


    list.DeleteElem(test_index);
    list.Dump();
    return true;
}
