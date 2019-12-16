DEF_OP(+, "+", 0, {
    *new_node = Node("+", TREE_OP);
    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*Differentiate(node->GetRightChild()));

})

DEF_OP(-, "-", 0, {
    *new_node = Node("-", TREE_OP);
    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*Differentiate(node->GetRightChild()));
})

DEF_OP(*, "\\cdot", 1, {
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
})

DEF_OP(/, "\\frac", 1, {
    *new_node = Node("/", TREE_OP);

    Node* left = (Node*) calloc(1, sizeof(Node));
    Node* right = (Node*) calloc(1, sizeof(Node));
    Node* left_left = (Node*) calloc(1, sizeof(Node));
    Node* left_right = (Node*) calloc(1, sizeof(Node));

    *left = Node("-", TREE_OP);
    *left_left = Node("*", TREE_OP);
    *left_right = Node("*", TREE_OP);

    left_left->AddLeftChild(*Differentiate(node->GetLeftChild()));
    left_left->AddRightChild(*node->GetRightChild());
    left->AddLeftChild(*left_left);

    left_right->AddLeftChild(*node->GetLeftChild());
    left_right->AddRightChild(*Differentiate(node->GetRightChild()));
    left->AddRightChild(*left_right);

    new_node->AddLeftChild(*left);

    *right = Node("*", TREE_OP);

    right->AddLeftChild(*node->GetRightChild());
    right->AddRightChild(*node->GetRightChild());

    new_node->AddRightChild(*right);
})