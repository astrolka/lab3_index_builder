#include <stdlib.h>
#include <string>
#include <set>

template <class Term>
class AVLTermNode {

public:
    Term &term;
    unsigned int count;
    std::set<unsigned int> docIdSet;

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

    AVLTermNode(Term &term, unsigned int docId) {
        this->term = term;
        docIdSet.insert(docId);
        count = 1;
    }

};