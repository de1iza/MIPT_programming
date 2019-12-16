#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <assert.h>
#include <math.h>

const int MAX_INPUT_SIZE = 500;
const int MAX_LABEL_SIZE = 100;
const double EPS = 0.0001;

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
    bool has_variable;

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
        assert(label);

        this->label = (char*) calloc(strlen(label) + 1, sizeof(char));
        strcpy(this->label, label);
        left = nullptr;
        right = nullptr;
        this->data_type = data_type;
        has_variable = (data_type == TREE_VAR);
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

    bool HasVariable() {
        return has_variable;
    }

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
        left->has_variable = node.has_variable;

        if(node.has_variable)
            this->has_variable = true;

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
        right->has_variable = node.has_variable;

        if(node.has_variable)
            this->has_variable = true;

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

    void* GetLabelValue() { // TODO replace strtod with this method
        char* end = nullptr;

        if (data_type == TREE_NUM) {
            double* val = (double*) calloc(1, sizeof(double));
            *val = std::strtod(label, &end);
            return val;
        }
        /*if (data_type == TREE_OP) {
            char* op = (char*) calloc(1, sizeof(char));
            *op = label[0];
            return op;

        }*/
        return label;
    }

    void Dump(FILE *file) {
        assert(file);

        fprintf(file, "{");
        fprintf(file, "\"%s\"%d", label, has_variable);

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

    bool Calculate(double* result) {
        assert(result);

        if (has_variable)
            return false;

        if (IsLeaf()) {
            char* end = nullptr;
            if (data_type == TREE_NUM) {
                *result = std::strtod(label, &end);
            }
            else {
                fprintf(stderr, "Wrong data_type in leaf: %d", data_type);
                abort();
            }
            return true;
        }

        double val1 = 0., val2 = 0.;

        if (left) {
            left->Calculate(&val1);
        }
        if (right) {
            right->Calculate(&val2);
        }


        if (data_type == TREE_OP) {
            #define DEF_OP(op, tex_repr, priority, code) else if (!strcmp(label, #op)) *result = val1 op val2;

            if (false) ;
            #include "operators.h"
            else {
                fprintf(stderr, "Unknown operator %s\n", label);
                return false;
            }

            #undef DEF_OP
        }
        else if (data_type == TREE_FUNC) {
            #define DEF_FUNC(func, lib_func, code) else if (!strcmp(label, #func)) *result = lib_func(val1);

            if (false) ;
            #include "funcs.h"
            else {
                fprintf(stderr, "Unknown function %s\n", label);
                return false;
            }

            #undef DEF_FUNC
        }
        else {
            fprintf(stderr, "Calculation error.\n");
            return false;
        }

        return true;

    }


    // TODO think about my encapsulation, isn't it stupid????

    friend Node* Simplify(Node* node);
    //friend void CopyData(Node* node, void* label, char data_type);
    //friend void LoadNode(FILE* file, Node* node);
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

bool Compare(double a, double b);
bool Compare(Node* a, Node* b);
bool Compare(DiffTree* a, DiffTree* b);
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
Node* DifferentiateFunction(Node* node);
DiffTree* Simplify(DiffTree* tree);
Node* Simplify(Node* node);
void MakeTex(DiffTree* tree, char* file_name = "diff.tex");
void DumpToTex(Node* node, char parent_priority, FILE* fp);
char GetOpPriority(Node* node);


int main() {

    //DiffTree* tree = GetG("(-13-42*x)*1+7/(-8-9*sin(6+ln(9)))");
    //tree->Show();
    //tree->Dump(fopen("dump.txt", "w"));

    //DiffTree* tree = GetG("x/3"); // TODO parse whitespaces
    //tree->Show();

    //DiffTree* new_tree = Differentiate(tree);
    //new_tree->Show();   // TODO simplify x + x = 2*x and x - x = 0

    double res = 0.;

    DiffTree* tree = GetG("x/(2+x)"); //TODO simplify before differentiation
    tree->Show();
    //tree->GetRoot()->Calculate(&res);



    //printf("\n!! %lf\n", res);

    DiffTree* new_tree = Simplify(Differentiate(tree));
    new_tree->Show();

    MakeTex(tree);


    system("pdflatex diff.tex ; evince diff.pdf");

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

    DiffTree* tree = (DiffTree*) calloc(1, sizeof(DiffTree));
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
        while ('0' <= *s && *s <= '9' || *s == '.' || *s == '-' && first_char) {
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

    if (sscanf(s, " %[sincotgln]%n", string, &read_cnt)) {
        s += read_cnt;

        printf(string);
        if (IsFunction(string)) {

            node = (Node*) calloc(1, sizeof(Node));
            *node = Node(string, TREE_FUNC);

            Node* new_node = GetP();
            node->AddLeftChild(*new_node);

        }
    }

    return node;

}

#define DEF_FUNC(name, lib_func, code)     \
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
        case TREE_FUNC:
            return DifferentiateFunction(node);
            break;
    }

    return new_node;
}

#define DEF_OP(op, tex_repr, priority, code) \
    else if (!strcmp(node->GetLabel(), #op)) code

Node* DifferentiateOperator(Node* node) {
    assert(node);

    Node* new_node = (Node*) calloc(1, sizeof(Node)); // TODO add free()

    printf(node->GetLabel());


    if (false) ;

    #include "operators.h"

    else {
        fprintf(stderr, "Unknown operator %s", node->GetLabel());
        abort();
    }

    return new_node;
}

#undef DEF_OP

#define DEF_FUNC(name, lib_func, code)    \
    else if (!strcmp(node->GetLabel(), #name)) code

Node* DifferentiateFunction(Node* node) { // TODO check zero division in calculation
    Node* new_node = (Node*) calloc(1, sizeof(Node));

    if (false) ;

    #include "funcs.h"

    else {
        fprintf(stderr, "Unknown function %s", node->GetLabel());
        abort();
    }

    return new_node;
}

#undef DEF_FUNC

bool Compare(DiffTree* a, DiffTree* b) {
    assert(a);
    assert(b);

    return Compare(a->GetRoot(), b->GetRoot());
}

bool Compare(Node* a, Node* b) {
    assert(a);
    assert(b);

    if (strcmp(a->GetLabel(), b->GetLabel()))
        return false;
    if (a->GetDataType() != b->GetDataType())
        return false;
    if (a->HasVariable() != b->HasVariable())
        return false;
    if (a->GetLeftChild() == nullptr || b->GetLeftChild() == nullptr) {
        if (a->GetLeftChild() != b->GetLeftChild())
            return false;
    }
    if (a->GetRightChild() == nullptr || b->GetRightChild() == nullptr) {
        if (a->GetRightChild() != b->GetRightChild())
            return false;
    }
    if (a->GetLeftChild()) {
        if (a->GetRightChild()) {
            return Compare(a->GetLeftChild(), b->GetLeftChild()) && Compare(a->GetRightChild(), b->GetRightChild());
        }
        return Compare(a->GetLeftChild(), b->GetLeftChild());
    }
    else if (a->GetRightChild())
        return Compare(a->GetRightChild(), b->GetRightChild());

    return true;
}

DiffTree* Simplify(DiffTree* tree) { //TODO optimize loop
    assert(tree);

    DiffTree* new_tree1 = (DiffTree*) calloc(1, sizeof(DiffTree));
    assert(new_tree1);
    *new_tree1 = DiffTree(Simplify(tree->GetRoot()));

    DiffTree* new_tree2 = (DiffTree*) calloc(1, sizeof(DiffTree));
    *new_tree2 = DiffTree(Simplify(new_tree1->GetRoot()));

    while (Compare(new_tree1, new_tree2) == 0) {

        new_tree2 = new_tree1;

        new_tree1 = (DiffTree*) calloc(1, sizeof(DiffTree));
        *new_tree1 = DiffTree(Simplify(new_tree2->GetRoot()));

    }


    return new_tree1;
}


Node* Simplify(Node* node) {
    assert(node);

    printf("\n");
    node->Dump(stdout);
    printf("\n");

    printf("\nLABEL %s VAR %d\n", node->GetLabel(), node->HasVariable());

    if (node->HasVariable()) {
        printf("%s has VAR", node->GetLabel());

        if (node->GetDataType() == TREE_VAR) {
            return node;
        }
        Node* left = Simplify(node->GetLeftChild());
        Node* right = nullptr;

        if (node->GetDataType() != TREE_FUNC)
            right = Simplify(node->GetRightChild());


        char* end = nullptr;

        if (!strcmp(node->GetLabel(), "*")) {
            if (left->GetDataType() == TREE_NUM) {
                if (Compare(std::strtod(left->GetLabel(), &end), 1.0)) {
                    return right;
                }
                if (Compare(std::strtod(left->GetLabel(), &end), 0.0)) {
                    Node* new_node = (Node*) calloc(1, sizeof(Node));
                    *new_node = Node("0", TREE_NUM);
                    return new_node;
                }
            }
            if (right->GetDataType() == TREE_NUM) {
                if (Compare(std::strtod(right->GetLabel(), &end), 1.0)) {
                    return left;
                }
                if (Compare(std::strtod(right->GetLabel(), &end), 0.0)) {
                    Node* new_node = (Node*) calloc(1, sizeof(Node));
                    *new_node = Node("0", TREE_NUM);
                    return new_node;
                }
            }
        }
        else if (!strcmp(node->GetLabel(), "+")) {
            if (left->GetDataType() == TREE_NUM) {
                if (Compare(std::strtod(left->GetLabel(), &end), 0.0)) {
                    return right;
                }
            }
            if (right->GetDataType() == TREE_NUM) {
                if (Compare(std::strtod(right->GetLabel(), &end), 0.0)) {
                    return left;
                }
            }
        }
        else if (!strcmp(node->GetLabel(), "/")) {
            if (right->GetDataType() == TREE_NUM) {
                if (Compare(std::strtod(right->GetLabel(), &end), 1.0)) {
                    return left;
                }
            }
        }
        else if (!strcmp(node->GetLabel(), "-")) {
            if (right->GetDataType() == TREE_NUM) {
                if (Compare(std::strtod(right->GetLabel(), &end), 0.0)) {
                    return left;
                }
            }
        }

        node->AddLeftChild(*left);
        if (node->GetDataType() != TREE_FUNC) {
            node->AddRightChild(*right);
            node->has_variable = (left->HasVariable() || right->HasVariable());
        }
        else {
            node->has_variable = left->HasVariable();
        }


    }
    else {
        Node* new_node = (Node*) calloc(1, sizeof(Node));

        char label[MAX_LABEL_SIZE] = "";
        double value = 0.;
        node->Calculate(&value);
        sprintf(label, "%lf", value);
        *new_node = Node(label, TREE_NUM);

        return new_node;
    }

    return node;

}

bool Compare(double a, double b) {
    return fabs(a - b) <= EPS;
}

void MakeTex(DiffTree* tree, char* file_name) {
    FILE* fp = fopen(file_name, "w");
    fprintf(fp, "\\documentclass{article}\n"
                "\\usepackage[14pt]{extsizes}\n"
                "\\usepackage[utf8]{inputenc}\n"
                "\n\\begin{document}\n");
    fprintf(fp, "$ ");

    fprintf(fp, "( ");
    DumpToTex(tree->GetRoot(), GetOpPriority(tree->GetRoot()), fp);
    fprintf(fp, ")' = ");


    DiffTree* derivative = Simplify(Differentiate(tree));

    DumpToTex(derivative->GetRoot(), GetOpPriority(derivative->GetRoot()), fp);

    fprintf(fp, "$\n\\end{document}");

    fclose(fp);

}

#define DEF_OP(op, tex_repr, priority, code)            \
    else if (!strcmp(node->GetLabel(), #op)) {          \
        return priority;                                \
    }                                                   \

char GetOpPriority(Node* node) {
    if (false) ;
    #include "operators.h"
}

#undef DEF_OP


void DumpToTex(Node* node, char parent_priority, FILE* fp) { // TODO more universal: prefix or postfix tex operator notion

    printf("\nPARENT PRIORITY %d\n", parent_priority);

    bool parentheses = false;

    if (node->GetDataType() == TREE_OP && parent_priority > GetOpPriority(node)) {
        parentheses = true;
    }

    if (parentheses) {
        fprintf(fp, "(");
    }


    if (node->GetDataType() == TREE_OP) { //TODO REFACTOR THIS IF
        if (node->GetLabel()[0] == '/') {
            fprintf(fp, "\\frac{ ");
            if (node->GetLeftChild()) DumpToTex(node->GetLeftChild(), GetOpPriority(node), fp); // LEFT
            fprintf(fp, "}{");
            if (node->GetRightChild()) DumpToTex(node->GetRightChild(), GetOpPriority(node), fp); // RIGHT
            fprintf(fp, "} ");

            if (parentheses) {
                fprintf(fp, ")");
            }

            return;
        }
    }


    if (node->GetLeftChild()) DumpToTex(node->GetLeftChild(), GetOpPriority(node), fp); // LEFT

    #define DEF_OP(op, tex_repr, priority, code)            \
        else if (!strcmp(node->GetLabel(), #op)) {          \
             fprintf(fp, tex_repr" ");                      \
        }

    if (node->GetDataType() == TREE_NUM) {
        fprintf(fp, "%.2lf ", *((double*) node->GetLabelValue()));
    }
    else if (node->GetDataType() == TREE_OP){
        if (false) ;
        #include "operators.h"

    }
    else if (node->GetDataType() == TREE_VAR) {
        fprintf(fp, "%s ", node->GetLabel());
    }

    #undef DEF_OP

    if (node->GetRightChild()) DumpToTex(node->GetRightChild(), GetOpPriority(node), fp); // RIGHT

    if (parentheses) {
        fprintf(fp, ")");
    }
}

