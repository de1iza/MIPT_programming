#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "textlib.h"

//const int LABELSIZE = 100;

const int MAX_INPUT_SIZE = 100;
const char* TREE_FILE = "tree.txt";

//TODO destructors

class Node { // make template label data type
private:
    char* label;
    Node* left;
    Node* right;
    void MakeLeftChild() {
        left = (Node*) calloc(1, sizeof(Node));
        *left = Node("");
    }
    void MakeRightChild() {
        right = (Node*) calloc(1, sizeof(Node));
        *right = Node("");
    }
public:
    Node(char* label) {
        this->label = (char*) calloc(strlen(label) + 1, sizeof(char));
        strcpy(this->label, label);
        left = nullptr;
        right = nullptr;
    }

    bool IsLeaf(){
        return left == nullptr && right == nullptr;
    }

    void AddLeftChild(Node node) {
        MakeLeftChild();

        left->label = (char*) calloc(strlen(node.label) + 1, sizeof(char));
        strcpy(left->label, node.label);
        if (node.left)
            left->AddLeftChild(*node.left);
        if (node.right)
            left->AddRightChild(*node.right);

    }

    void AddRightChild(Node node) {
        MakeRightChild();

        right->label = (char*) calloc(strlen(node.label) + 1, sizeof(char));
        strcpy(right->label, node.label);
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
            fprintf(file, "N%p -> N%p [label=\"YES\" color=\"green4\" fontcolor=\"green4\"]\n", this, left);
            left->Show(file);
            fprintf(file, "\n");
        }

        if (right) {
            fprintf(file, "N%p -> N%p [label=\"NO\" color=\"red4\" fontcolor=\"red4\"]\n", this, right);
            right->Show(file);
            fprintf(file, "\n");
        }
    }

    friend void LoadNode(FILE* file, Node* node);
    friend int main(); // TODO REMOVE THIS
};


class AkinatorTree {
private:

public:
    Node* root;   // TODO MAKE ROOT PRIVATE AGAIN

    AkinatorTree(char* init_question) {
        root = (Node*) calloc(1, sizeof(Node));
        *root = Node(init_question);
    }

    AkinatorTree(FILE* load_file) {
        assert(load_file);

        char ch = ' ';
        fscanf(load_file, "%c", &ch);

        if (ch != '{') {
            fprintf(stderr, "Parsing error. Expected {, received %c", ch);
        }

        root = (Node*) calloc(1, sizeof(Node));

        LoadNode(load_file, root);

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
    }


};

void Game(AkinatorTree* tree);
void GuessingMode(AkinatorTree* tree);
void AskQuestion(AkinatorTree* tree, Node* node);
void AddNewQuestion(Node* node, char* object, char* question);
void GuessingFinal(AkinatorTree* tree, Node* node, bool guessed);
void ComparisonMode(AkinatorTree* tree);
void DefinitionMode(AkinatorTree* tree);

int main() {

    /*AkinatorTree tree = AkinatorTree("initial");
    tree.root->AddLeftChild(Node("lefft"));
    tree.root->AddRightChild(Node("right"));
    tree.Show();*/


    AkinatorTree T = AkinatorTree(open_file(TREE_FILE, "r"));
    T.Show();

    Game(&T);

    T.Show();
    return 0;
}

void LoadNode(FILE* file, Node* node) {
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
}

void Game(AkinatorTree* tree) {
    assert(tree);

    printf("Hello! I'm Akinator. What mode do you want to play? \n");
    printf("1 -  guessing mode\n");
    printf("2 -  comparison mode\n");
    printf("3 -  definition mode\n");
    int ans = 0;
    while (true) {
        printf("Enter number from 1 to 3: ");
        scanf("%d", &ans);
        if (ans >= 1 && ans <= 3) break;
    }
    printf("\n");

    switch (ans) {
        case 1:
            GuessingMode(tree);
            break;
        case 2:
            ComparisonMode(tree);
            break;
        case 3:
            DefinitionMode(tree);
            break;
    }
}

void AddNewQuestion(Node* node, char* object, char* question) {
    Node* new_node = (Node*) calloc(1, sizeof(Node));
    *new_node = Node(question);
    new_node->AddLeftChild(Node(object));
    new_node->AddRightChild(Node(node->GetLabel()));

    *node = *new_node;

}

void GuessingFinal(AkinatorTree* tree, Node* node, bool guessed) {
    char inp_object[MAX_INPUT_SIZE] = {};
    char inp_property[MAX_INPUT_SIZE] = {};

    if (guessed) {
        printf("I knew! It was too easy :)");
    } else {
        printf("I failed :( What was it? \n");
        scanf("%s", inp_object);
        printf("What distinguishes %s from %s?\n", inp_object, node->GetLabel());
        scanf("%s", inp_property);
        AddNewQuestion(node, inp_object, inp_property);

        tree->Dump(open_file(TREE_FILE, "w"));
    }
}

void AskQuestion(AkinatorTree* tree, Node* node) {
    char ans[MAX_INPUT_SIZE] = {};

    while (true) {
        printf("Is it %s?\n", node->GetLabel());
        printf("Enter yes/y or no/n: ");
        scanf("%s", ans);
        printf("\n");
        if (strcmp(ans, "yes") == 0 || strcmp(ans, "y") == 0) {
            if (node->IsLeaf()) {
                GuessingFinal(tree, node, true);
                return;
            }
            AskQuestion(tree, node->GetLeftChild());
            break;
        } else if (strcmp(ans, "no") == 0 || strcmp(ans, "n") == 0) {
            if (node->IsLeaf()) {
                GuessingFinal(tree, node, false);
                return;
            }
            AskQuestion(tree, node->GetRightChild());
            break;
        } else {
            printf("I don't understand you:( Try again.\n\n");
        }

    }
}

void GuessingMode(AkinatorTree* tree) {
    printf("Think of an object or whatever you want and I'll try to guess it.\n");
    AskQuestion(tree, tree->root);

}

void ComparisonMode(AkinatorTree* tree) {

}


void DefinitionMode(AkinatorTree* tree) {

}

