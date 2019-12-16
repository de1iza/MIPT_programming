DEF_OP(+, val1 + val2, "+", 0, {
    *new_node = Node("+", TREE_OP);
    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*Differentiate(node->GetRightChild()));

})

DEF_OP(-, val1 - val2, "-", 0, {
    *new_node = Node("-", TREE_OP);
    new_node->AddLeftChild(*Differentiate(node->GetLeftChild()));
    new_node->AddRightChild(*Differentiate(node->GetRightChild()));
})

DEF_OP(*, val1 * val2, "\\cdot", 1, {
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

DEF_OP(/, val1 / val2, "\\frac", 1, {
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

DEF_OP(^, pow(val1, val2), "^", 2, {
   if (node->GetRightChild()->HasVariable()) { // exponential показательная  a^x
        *new_node = Node("*", TREE_OP);

        Node* right = (Node*) calloc(1, sizeof(Node));
        Node* right_right = (Node*) calloc(1, sizeof(Node));

        *right = Node("*", TREE_OP);

        *right_right = Node("ln", TREE_FUNC);
        right_right->AddLeftChild(*node->GetLeftChild());

        right->AddLeftChild(*node->GetRightChild());
        right->AddRightChild(*right_right);

        new_node->AddLeftChild(*node);
        new_node->AddRightChild(*Differentiate(right));

   }
   else { // power степенная x^a
        *new_node = Node("*", TREE_OP);

        Node* left = (Node*) calloc(1, sizeof(Node));
        Node* left_right = (Node*) calloc(1, sizeof(Node));

        *left = Node("*", TREE_OP);

        *left_right = Node("^", TREE_OP);
        left_right->AddLeftChild(*node->GetLeftChild());

        left_right->AddRightChild(Node("-", TREE_OP));
        left_right->GetRightChild()->AddLeftChild(*node->GetRightChild());
        left_right->GetRightChild()->AddRightChild(Node("1", TREE_NUM));

        left->AddLeftChild(*node->GetRightChild());
        left->AddRightChild(*left_right);

        new_node->AddLeftChild(*left);
        new_node->AddRightChild(*Differentiate(node->GetLeftChild()));

   }

})