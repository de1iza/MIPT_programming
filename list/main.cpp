#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

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
    bool IndexIsValid(const int index);
public:
    List_t(size_t max_size);
    ~List_t();
    bool isEmpty();
    bool InsertBefore(const int index, const T value);
    bool InsertAfter(const int index, const T value);
    bool InsertFirst(const T value);
    bool InsertLast(const T value);
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
List_t<T>::List_t(size_t max_size): size(0), head(0), tail(0), next_free(0), max_size(max_size) {
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
bool List_t<T>::InsertAfter(const int index, const T value) {
    if (size == max_size) {
        fprintf(stderr, "List is full\n");  // TODO resize
        return false;
    }
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return false;
    }

    if (prev[index] == POISON && index != head) {
        fprintf(stderr, "This element is not in the list\n");
        return false;
    }

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


    size++;
    return true;
}

template<typename T>
bool List_t<T>::InsertBefore(const int index, const T value) {
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return false;
    }
    if (prev[index] == POISON && next[index] == POISON) {
        fprintf(stderr, "This element is not in the list\n");
        return false;
    }
    if (index == head) {
        return InsertFirst(value);
    }
    InsertAfter(prev[index], value);
    return true;
}

template<typename T>
bool List_t<T>::InsertFirst(const T value) {
    if (size == max_size) {
        fprintf(stderr, "List is full\n");
        return false;
    }
    int pos = next_free;
    data[pos] = value;

    next_free = next[pos];

    if (isEmpty()) {
        tail = pos;
        next[pos] = POISON;
    }
    else {
        next[pos] = head;
        prev[head] = pos;
    }


    head = pos;

    size++;
    return true;
}

template<typename T>
bool List_t<T>::InsertLast(const T value) {
    if (size == max_size) {
        fprintf(stderr, "List is full\n");  // TODO resize
        return false;
    }
    int pos = next_free;
    next_free = next[pos];

    data[pos] = value;

    next[pos] = POISON;
    next[tail] = pos;

    if (isEmpty()) {
        head = pos;
        next[pos] = POISON;
    }
    else {
        prev[pos] = tail;
    }
    tail = pos;

    size++;
    return true;
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
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return false;
    }
    if (prev[index] == POISON &&  index != head) {
        fprintf(stderr, "This element is not in the list\n");
        return false;
    }

    if (index == head) head = next[head];
    else if (index == tail) tail = prev[tail];

    next[prev[index]] = next[index];
    prev[next[index]] = prev[index];

    data[index] = POISON;
    next[index] = next_free;
    prev[index] = POISON;
    next_free = index;

    size--;
    return true;
}

template<typename T>
void List_t<T>::Dump(const char* filename) {
    remove("dump.png");

    FILE* file = fopen(filename, "w");

    fprintf(file, "digraph list{\n"
                  "rankdir=LR;\n"
                  "forcelabels=true;\n"
                  "node [shape=record];\n");

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

    system("dot -Tpng dump.dot -o dump.png");

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
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return nullptr;
    }
    if (prev[index] == POISON &&  index != head) {
        fprintf(stderr, "This element is not in the list\n");
        return nullptr;
    }
    if (index == tail) {
        return nullptr;
    }
    else
        return &next[index];
}

template<typename T>
T *List_t<T>::GetBeforeP(const int index) {
    if (!IndexIsValid(index)) {
        fprintf(stderr, "Index out of range\n");
        return nullptr;
    }
    if (prev[index] == POISON &&  index != head) {
        fprintf(stderr, "This element is not in the list\n");
        return nullptr;
    }
    if (index == head) {
        return nullptr;
    }
    else
        return &prev[index];
}

template<typename T>
T List_t<T>::GetElemByPosition(const int position) {
    if (position >= size || position < 0) {
        fprintf(stderr, "List doesn't contain element on this position\n");
        return POISON;
    }

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
    list.InsertAfter(0, 34);
    list.Dump();
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
    list.InsertFirst(3);
    list.InsertLast(22);
    list.InsertBefore(1, 179);
    list.InsertLast(111);
    list.InsertLast(44);
    list.Dump();
    return true;
}
