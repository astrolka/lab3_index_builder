#include <stdlib.h>
#include <string>

template <class Term>
class AVLTermNode {

public:
    Term &term;
    size_t count;
    AVLTermNode *leftChild;
    AVLTermNode *rightChild;

    size_t height;

    size_t leftHeight() {
        if (leftChild == nullptr) {
            return -1;
        }
        return leftChild->height;
    }
    size_t rightHeight() {
        if (rightChild == nullptr) {
            return -1;
        }
        return rightChild->height;
    }
    int balanceFactor() {
        return leftHeight() - rightHeight();
    }

    AVLTermNode(Term &term) {
        this->term = term;
        count = 1;
    }

};