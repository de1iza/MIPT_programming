#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <assert.h>

const int MAX_INPUT_SIZE = 500;

enum tree_data_types {
    TREE_NUM,
    TREE_VAR,
    TREE_OP,
    TREE_FUNC,
};


class Node { // make template label data type
private:
    char* label;
    Node* left;
    Node* right;
    char data_type;

    void MakeLeftChild() {
        left = (Node*) calloc(1, sizeof(Node));
        *left = Node("", 0);
    }

    void MakeRightChild() {
        right = (Node*) calloc(1, sizeof(Node));
        *right = Node("", 0);
    }

public:
    Node(char* label, char data_type) {
        this->label = (char*) calloc(strlen(label) + 1, sizeof(char));
        strcpy(this->label, label);
        left = nullptr;
        right = nullptr;
        this->data_type = data_type;
    }

    /*~Node() {
        free(label);
        if (left) {
            left->~Node();
        }
        free(left);

        if (right) {
            right->~Node();
        }
        free(right);
    }*/

    char GetDataType() {
        return data_type;
    }

    bool IsLeaf(){
        return left == nullptr && right == nullptr;
    }

    void AddLeftChild(Node node) {
        MakeLeftChild();

        left->label = (char*) calloc(strlen(node.label) + 1, sizeof(char));
        strcpy(left->label, node.label);
        left->data_type = node.data_type;
        if (node.left)
            left->AddLeftChild(*node.left);
        if (node.right)
            left->AddRightChild(*node.right);

    }

    void AddRightChild(Node node) {
        MakeRightChild();

        right->label = (char*) calloc(strlen(node.label) + 1, sizeof(char));
        strcpy(right->label, node.label);
        right->data_type = node.data_type;
        if (node.left)
            right->AddLeftChild(*node.left);
        if (node.right)
            right->AddRightChild(*node.right);

    }

    Node* GetLeftChild() {
        return left;
    }

    Node* GetRightChild() {
        return right;
    }

    char* GetLabel() {
        return label;
    }

    void Dump(FILE *file) {
        assert(file);

        fprintf(file, "{");
        fprintf(file, "\"%s\"", label);

        if (left) left->Dump(file);

        if (right) right->Dump(file);

        fprintf(file, "}");
    }

    void Show(FILE* file) {
        assert(file);
        assert(this);


        fprintf(file, "N%p [label=\"%s\"]\n", this, label);


        if (left) {
            fprintf(file, "N%p -> N%p\n", this, left);
            left->Show(file);
            fprintf(file, "\n");
        }

        if (right) {
            fprintf(file, "N%p -> N%p\n", this, right);
            right->Show(file);
            fprintf(file, "\n");
        }
    }

    bool Find(char* label, int path, int* final_path) {
        if (IsLeaf()) {
            if (strcmp(this->label, label) == 0) {
                *final_path = path;
                return true;
            }
        }

        if (left) {
            if (left->Find(label, (path << 1) | 1, final_path)) return true;
        }
        if (right) {
            if (right->Find(label, path << 1, final_path)) return true;
        }

        return false;
    }



    friend void LoadNode(FILE* file, Node* node);
};

class DiffTree {
private:
    Node* root;
public:
    DiffTree(char* init_str, char data_type) {
        root = (Node*) calloc(1, sizeof(Node));
        *root = Node(init_str, data_type);
    }

    DiffTree(Node* root) {
        this->root = root;
    }

    /*DiffTree(FILE* load_file) {
        assert(load_file);

        char ch = ' ';
        fscanf(load_file, "%c", &ch);

        if (ch != '{') {
            fprintf(stderr, "Parsing error. Expected {, received %c", ch);
        }

        root = (Node*) calloc(1, sizeof(Node));

        LoadNode(load_file, root);

    }*/

    /*~AkinatorTree() {
        root->~Node();
        free(root);
    }*/

    Node* GetRoot() {
        return root;
    }

    void Dump(FILE* fp) {
        root->Dump(fp);
    }

    void Show(const char* filename = "tree.dot") {
        FILE * file = fopen(filename, "w");

        fprintf(file, "digraph tree{\n");

        root->Show(file);

        fprintf(file, "}");

        fclose(file);

        system("dot -Tpng tree.dot -o dump.png");
        system("display dump.png");
    }

    bool Find(char* label, int* path) {
        root->Find(label, 1, path);
    }
};

DiffTree* GetG(const char* s);
Node* GetN();
Node* GetE();
Node* GetT();
Node* GetP();
Node* GetF();
bool IsFunction(char* string);
DiffTree* Differentiate(DiffTree* tree);
Node* Differentiate(Node* node);
Node* DifferentiateOperator(Node* node);

int main() {

    //DiffTree* tree = GetG("(-13-42*x)*1+7/(-8-9*sin(6+ ln(9)))");
    //tree->Show();
    //tree->Dump(fopen("dump.txt", "w"));

    DiffTree* tree = GetG("x*7/6"); // TODO parse whitespaces
    //tree->Show();

    DiffTree* new_tree = Differentiate(tree);
    new_tree->Show();

    return 0;
}

/*void LoadNode(FILE* file, Node* node) {
    const int LABELSIZE = 100;

    char cur_char = ' ';
    char label[LABELSIZE] = {};

    fscanf(file, "%*[\"]%[^\"]%*[\"]", label);

    node->label = (char*) calloc(strlen(label) + 1, sizeof(char));
    strcpy(node->label, label);


    fscanf(file, " %c ", &cur_char);

    if (cur_char == '{') {
        node->MakeLeftChild();
        LoadNode(file, node->left);
    } else if (cur_char == '}') {
        return;
    } else {
        fprintf(stderr, "Parsing error. Expected { or }, received %c", cur_char);
        abort();
    }


    fscanf(file, " %c ", &cur_char);

    if (cur_char == '{') {
        node->MakeRightChild();
        LoadNode(file, node->right);
    } else if (cur_char == '}') {
        return;
    } else {
        fprintf(stderr, "Parsing error. Expected { or }, received %c", cur_char);
        abort();
    }

    fscanf(file, " %c ", &cur_char);

    if (cur_char != '}')
        fprintf(stderr, "Parsing error. Expected }, received %c", cur_char);
} */

const char* s = " ";

DiffTree* GetG(const char* str) {
    s = str;

    printf("G %s\n", s);

    DiffTree* tree = (DiffTree*) calloc(1, sizeof(DiffTree()));
     *tree =  DiffTree(GetE());

    // TODO assert(*s == '\0');

    return tree;
}

Node* GetN() {
    printf("N %s\n", s);

    char val[MAX_INPUT_SIZE] = "";

    Node *new_node = (Node *) calloc(1, sizeof(Node));
    bool first_char = true;

    if (*s == 'x') {
        strncat(val, s, 1);
        s++;
        *new_node = Node(val, TREE_VAR);
    } else {
        while ('0' <= *s && *s <= '9' || *s == '-' && first_char) {
            strncat(val, s, 1);
            s++;
            first_char = false;
        }
        *new_node = Node(val, TREE_NUM);
    }
    printf("%s\n", val);


    return new_node;
}

Node* GetE() {
    printf("E %s\n", s);

    Node* node = GetT();
    assert(node);

    Node* new_node = nullptr;

    while (*s == '+' || *s == '-') {
        char op[2] = "";
        strncat(op, s, 1);
        s++;

        Node* val = GetT();
        assert(val);

        new_node = (Node*) calloc(1, sizeof(Node));
        *new_node = Node(op, TREE_OP);

        new_node->AddLeftChild(*node);
        new_node->AddRightChild(*val);

        node = new_node;

    }

    node->Dump(stdout);

    return node;
}

Node* GetT() {
    printf("T %s\n", s);

    Node* node = GetP();
    assert(node);

    Node* new_node = nullptr;

    while (*s == '*' || *s == '/') {
        char op[2] = "";
        strncat(op, s, 1);
        s++;

        Node* val = GetP();
        assert(val);

        new_node = (Node*) calloc(1, sizeof(Node));
        *new_node = Node(op, TREE_OP);

        new_node->AddLeftChild(*node);
        new_node->AddRightChild(*val);

        node = new_node;

    }

    node->Dump(stdout);

    return node;

}

Node* GetP() {
    Node* node = nullptr;

    printf("P %s\n", s);

    if (*s == '(') {
        s++;
        node = GetE();
        assert(node);

        if (*s == ')') {
            s++;
        } else {
            fprintf(stderr, "Expected ')', got %c", *s);
            abort();
        }

    }
    else if (*s >= '0' && *s <= '9' || *s == '-' || *s == 'x'){
        node = GetN();
        assert(node);
    }
    else if ((node = GetF()) == nullptr) {
        fprintf(stderr, "Syntax error. Expected integer, function or parentheses, got %c", *s);
        abort();
    }

    return node;
}



Node* GetF() {
    printf("F %s\n", s);

    Node* node = nullptr;
    char string[MAX_INPUT_SIZE] = "";
    int read_cnt = 0;

    if (sscanf(s, " %[sincotanln]%n", string, &read_cnt)) {
        s += read_cnt;

        printf(string);
        if (IsFunction(string)) {
            printf("FUNC!");

            node = (Node*) calloc(1, sizeof(Node));
            *node = Node(string, TREE_FUNC);

            Node* new_node = GetP();
            node->AddRightChild(*new_node);

        }
    }



    return node;

}

#define DEF_FUNC(name, some_str)     \
    if (!strcmp(string, #name)) return true;

bool IsFunction(char* string) {
    #include "funcs.h"
    return false;
}

#undef DEF_FUNC


DiffTree* Differentiate(DiffTree* tree) {
    assert(tree);

    DiffTree* new_tree = (DiffTree*) calloc(1, sizeof(DiffTree));
    assert(new_tree);

    *new_tree = DiffTree(Differentiate(tree->GetRoot()));

    return new_tree;
}

Node* Differentiate(Node* node) {
    assert(node);

    Node* new_node = (Node*) calloc(1, sizeof(Node));

    switch (node->GetDataType()) {
        case TREE_VAR:
            *new_node = Node("1", TREE_NUM);
            break;
        case TREE_NUM:
            *new_node = Node("0", TREE_NUM);
            break;
        case TREE_OP:
            return DifferentiateOperator(node);
            break;
    }

    return new_node;
}

Node* DifferentiateOperator(Node* node) {
    assert(node);

    Node* new_node = (Node*) calloc(1, sizeof(Node)); // TODO add free()

    printf(node->GetLabel());

    if (!strcmp(node->GetLabel(), "+")) {     // TODO make macros for that???
        *new_node = Node("+", TREE_OP);
        new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
        new_node->AddRightChild(*Differentiate(node->GetRightChild()));
    }
    else if (!strcmp(node->GetLabel(), "-")) {
        *new_node = Node("-", TREE_OP);
        new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
        new_node->AddRightChild(*Differentiate(node->GetRightChild()));
    }
    else if (!strcmp(node->GetLabel(), "*")) {
        *new_node = Node("+", TREE_OP);

        Node* left = (Node*) calloc(1, sizeof(Node));
        Node* right = (Node*) calloc(1, sizeof(Node));

        *left = Node("*", TREE_OP);
        left->AddLeftChild(*node->GetLeftChild());
        left->AddRightChild(*Differentiate(node->GetRightChild()));

        *right = Node("*", TREE_OP);
        right->AddLeftChild(*Differentiate(node->GetLeftChild()));
        right->AddRightChild(*node->GetRightChild());

        new_node->AddLeftChild(*left);
        new_node->AddRightChild(*right);
    }
    else if (!strcmp(node->GetLabel(), "/")) {
        *new_node = Node("/", TREE_OP);

        Node* left = (Node*) calloc(1, sizeof(Node));
        Node* left_left = (Node*) calloc(1, sizeof(Node));
        Node* left_right = (Node*) calloc(1, sizeof(Node));
        Node* right = (Node*) calloc(1, sizeof(Node));


        *left = Node("-", TREE_OP);

        *left_left = Node("*", TREE_OP);
        left_left->AddLeftChild(*Differentiate(node->GetLeftChild()));
        left_left->AddRightChild(*node->GetRightChild());

        *left_right = Node("*", TREE_OP);
        left_right->AddLeftChild(*node->GetLeftChild());
        left_right->AddRightChild(*Differentiate(node->GetRightChild()));

        left->AddLeftChild(*left_left);
        left->AddRightChild(*left_right);

        *right = Node("*", TREE_OP);
        right->AddLeftChild(*node->GetRightChild());
        right->AddRightChild(*node->GetRightChild());

        new_node->AddLeftChild(*left);
        new_node->AddRightChild(*right);
    }

    return new_node;
}