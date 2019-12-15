DEF_OP(+, {
    *new_node = Node("+", TREE_OP);
    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*Differentiate(node->GetRightChild()));
})

DEF_OP(-, {
    *new_node = Node("-", TREE_OP);
    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*Differentiate(node->GetRightChild()));
})

DEF_OP(*, {
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

DEF_OP(/, {
    *new_node = Node("/", TREE_OP);

    Node* left = (Node*) calloc(1, sizeof(Node));

    *left = Node("-", TREE_OP);

    left->AddLeftChild(Node("*", TREE_OP));
    left->GetLeftChild()->AddLeftChild(*Differentiate(node->GetLeftChild()));
    left->GetLeftChild()->AddRightChild(*node->GetRightChild());

    left->AddRightChild(Node("*", TREE_OP));
    left->GetRightChild()->AddLeftChild(*node->GetLeftChild());
    left->GetRightChild()->AddRightChild(*Differentiate(node->GetRightChild()));

    new_node->AddLeftChild(*left);

    new_node->AddRightChild(Node("*", TREE_OP));
    new_node->GetRightChild()->AddLeftChild(*node->GetRightChild());
    new_node->GetRightChild()->AddRightChild(*node->GetRightChild());
})