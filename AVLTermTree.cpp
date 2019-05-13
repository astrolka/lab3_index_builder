#include <stdlib.h>
#include "AVLTermNode.cpp"

using namespace std;

template <class Term>
class AVLTermTree {

public:
    AVLTermNode<Term> *root;

    void insert(Term &term, unsigned int docId) {
        if (root == nullptr) {
            *root = AVLTermNode<Term>(term, docId);
        }
        root = &insert(root, term, docId);
    }

private:

    AVLTermNode<Term> &insert(AVLTermNode<Term> *fromNode, Term &term, unsigned int docId) {

        if (fromNode == nullptr) {
            return AVLTermNode<Term>(term, docId);
        }

        AVLTermNode<Term> &node = *fromNode;

        if (term.compare(node.term) == 0) {
            node.count++;
            node.docIdSet.insert(docId);
        } else if (term.compare(node.term) < 0) {
            node.leftChild = insert(node.leftChild, term);
        } else {
            node.rightChild = insert(node.rightChild, term);
        }

        AVLTermNode<Term> &balancedNode = balanced(node);
        balancedNode.height = max(balancedNode.leftHeight(), balancedNode.rightHeight()) + 1;

        return balancedNode;
    }

    AVLTermNode<Term> &balanced(AVLTermNode<Term> &node) {
        if (node.balanceFactor() == 2 && node.leftChild != nullptr && node.leftChild->balanceFactor() == -1) {
            return leftRightRotate();
        } else if (node.balanceFactor() == 2) {
            return rightRotate(node);
        } else if (node.balanceFactor() == -2 && node.rightChild != nullptr && node.rightChild->balanceFactor() == 1) {
            return rightLeftRotate(node);
        } else if (node.balanceFactor() == -2) {
            return leftRotate(node);
        }
        return node;
    }

    AVLTermNode<Term> &leftRotate(AVLTermNode<Term> &node) {
        AVLTermNode<Term> *pivot = node.rightChild;

        node.rightChild = pivot->leftChild;
        pivot->leftChild = node;

        node.height = max(node.leftHeight(), node.rightHeight()) + 1;
        pivot->height = max(pivot->leftHeight(), pivot->rightHeight()) + 1;

        return *pivot;
    }

    AVLTermNode<Term> &rightRotate(AVLTermNode<Term> &node) {
        AVLTermNode<Term> *pivot = node.leftChild;

        node.leftChild = pivot->rightChild;
        pivot->rightChild = node;

        node.height = max(node.leftHeight(), node.rightHeight()) + 1;
        pivot->height = max(pivot->leftHeight(), pivot->rightHeight()) + 1;

        return *pivot;
    }

    AVLTermNode<Term> &rightLeftRotate(AVLTermNode<Term> &node) {
        if (node.rightChild == nullptr) {
            return node;
        }

        node.rightChild = &rightRotate(*(node.rightChild));

        return leftRotate(node);
    }

    AVLTermNode<Term> &leftRightRotate(AVLTermNode<Term> &node) {
        if (node.leftChild == nullptr) {
            return node;
        }

        node.leftChild = &leftRotate(*(node.leftChild));

        return rightRotate(node);
    }
    
};
