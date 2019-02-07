//
// Created by Shawheen Attar on 4/21/18.
//

#ifndef PROJECT8_BST_H
#define PROJECT8_BST_H

#endif //PROJECT8_BST_H

#include <vector>
#include <string>
#include <cstdint>

using namespace std;

class BST {
    class Node {
    public:
        int val;
        string name;
        Node *left;
        Node *right;
        Node *parent; // optional

        Node(string el, int num) {
            this->name = el;
            this->val = num;
            this->left = 0;
            this->right = 0;
            this->parent = 0;
        }

        ~Node(){
            delete left;
            delete right;
        }
    };

    Node *root;

    //Node* findMin();
    //Node* successor(Node *n);

public:

    BST();

    ~BST(){
        delete root;
    }

    // BST(BST &other)
    // BST& operator=(BST &other)

    void insert(string el, int num);
    bool find(string el);
    int outVal(string el);
    int setVal(string el, int num);
    // void remove(int el);
};
