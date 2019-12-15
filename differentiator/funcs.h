DEF_FUNC(cos, {
    *new_node = Node("*", TREE_OP);

    Node* left = (Node*) calloc(1, sizeof(Node));
    Node* left_right = (Node*) calloc(1, sizeof(Node));

    *left = Node("*", TREE_OP);

    left->AddLeftChild(Node("-1", TREE_NUM));

    *left_right = Node("sin", TREE_FUNC);
    left_right->AddLeftChild(*node->GetLeftChild());
    left->AddRightChild(*left_right);

    new_node->AddLeftChild(*left);
    new_node->AddRightChild(*Differentiate(node->GetLeftChild()));
})

DEF_FUNC(sin, {
    *new_node = Node("*", TREE_OP);

    Node* left = (Node*) calloc(1, sizeof(Node));

    *left = Node("cos", TREE_FUNC);
    left->AddLeftChild(*node->GetLeftChild());

    new_node->AddLeftChild(*left);
    new_node->AddRightChild(*Differentiate(node->GetLeftChild()));
})

DEF_FUNC(tg, {
    *new_node = Node("/", TREE_OP);

    Node* right = (Node*) calloc(1, sizeof(Node));

    *right = Node("*", TREE_OP);

    right->AddLeftChild(Node("cos", TREE_FUNC));
    right->GetLeftChild()->AddLeftChild(*node->GetLeftChild());

    right->AddRightChild(Node("cos", TREE_FUNC));
    right->GetRightChild()->AddLeftChild(*node->GetLeftChild());


    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*right);
})

DEF_FUNC(ln, {
    *new_node = Node("/", TREE_OP);

    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*node->GetLeftChild());
})

