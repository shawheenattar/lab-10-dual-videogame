//
// Created by Shawheen Attar on 4/21/18.
//

#include <stdlib.h>
#include "BST.h"
//#include <iostream>

using namespace std;

BST::BST() {
    this->root = 0;
}

void BST::insert(string el, int num) {
    if (this->find(el)){
        //cout << "variable " << el << " incorrectly re-initialized" << endl;
        setVal(el, num);
        return;
    }

    Node *n = new Node(el, num);

    if (this->root == 0) {
        this->root = n;
        return;
    }

    Node *parent = 0;
    Node *current = this->root;

    while (current != 0) {
        parent = current;
        if (current->name > el) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // Include new node into the tree.
    if (parent->name > el) {
        parent->left = n;
    } else {
        parent->right = n;
    }
    n->parent = parent;
}

bool BST::find(string el) {
    Node *current = root;
    while (current != 0) {
        if (current->name == el) {
            return true;
        }

        if (current->name > el) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return false;
}

int BST::outVal(string el) {
    Node *current = root;
    while (current != 0) {
        if (current->name == el) {
            return current->val;
        }

        if (current->name > el) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0;
}

int BST::setVal(string el, int num) {
    Node *current = root;
    while (current != 0) {
        if (current->name == el) {
            current->val = num;
        }

        if (current->name > el) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0;
}

