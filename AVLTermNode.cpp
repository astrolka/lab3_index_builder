#include <stdlib.h>
#include <string>
#include <set>

using namespace std;

class AVLTermNode {

public:
    string term;
    unsigned int count;
    std::set<unsigned int> docIdSet;

    AVLTermNode *leftChild = nullptr;
    AVLTermNode *rightChild = nullptr;

    size_t height = 0;

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

    AVLTermNode(string t, unsigned int docId) {
        term = t;
        docIdSet.insert(docId);
        count = 1;
    }

    ~AVLTermNode() {
        delete this;
    }

};