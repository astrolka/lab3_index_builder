#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <regex>
#include <chrono>
#include <fstream>
#include <set>
#include "AVLTermTree.cpp"
#include "BinaryMmap.cpp"
#include "utils.cpp"

using namespace std;
using namespace std::chrono;

static size_t termCount = 0;
void saveIndex(AVLTermNode *node, BinaryMmap &mainIndex, BinaryMmap &coordBlocks, size_t at = 0);
string &lowerCaseStr(string &str);

int main() {

    string tokensPath = "tokens.bin";

    cout << "Enter path to file with tokens" << endl;
//    cin >> tokensPath;

    BinaryMmap tokens(tokensPath, 0);
    tokens.updateCurrentPosition();

    remove("reverseIndex.bin");
    remove("coordBlocks.bin");
    BinaryMmap mainIndex("reverseIndex.bin", 10000);
    BinaryMmap coordBlocks("coordBlocks.bin");

    AVLTermTree indexTree;

    regex graveRX("\\xCC[\\x80-\\xBB]");

    size_t articleCounter = 0;
    size_t articleIndexTime = 0;

    size_t totalTermCount = 0;
    double termLength = 0;

    unsigned kbCount = 0;
    unsigned byteBuffer = 0;
    size_t kbIndexTime = 0;

    high_resolution_clock::time_point totalTimer = high_resolution_clock::now();
    high_resolution_clock::time_point kbTimer = high_resolution_clock::now();
    high_resolution_clock::time_point articleTimer = high_resolution_clock::now();

    int prevDocId = 0;

    while (tokens.currentPosition() < tokens.writtenBytes()) {
        int docId = tokens.readInt(DOC_ID_C);
        int termPerArticleCount = tokens.readInt(TERM_COUNT_C);

        while (termPerArticleCount) {
            int tokenLen = tokens.readInt(TERM_SIZE_C);
            string_view token = tokens.readStr(tokenLen);

            string term = regex_replace(string(token), graveRX, "");
            lowerCaseStr(term);

            indexTree.insert(term, docId);

            termLength = (termLength * (double)totalTermCount + (double)term.length()) / (double)(totalTermCount + 1);
            totalTermCount++;
            byteBuffer += term.length();

            if (byteBuffer >= 1024) {
                high_resolution_clock::time_point now = high_resolution_clock::now();
                kbIndexTime = (kbIndexTime * kbCount + duration_cast<microseconds>(now - kbTimer).count()) / (kbCount + 1);
                kbTimer = now;
                kbCount++;
                byteBuffer = 0;
            }

            termPerArticleCount--;
        }

        high_resolution_clock::time_point now = high_resolution_clock::now();
        articleIndexTime = (articleIndexTime * articleCounter + duration_cast<microseconds>(now - articleTimer).count()) / (articleCounter + 1);
        articleTimer = now;
        articleCounter++;
    }

    high_resolution_clock::time_point now = high_resolution_clock::now();

    cout << "total time in milliseconds:\t" << duration_cast<milliseconds>(now - totalTimer).count() << endl;
    cout << "time per article in microseconds:\t" << articleIndexTime << endl;
    cout << "time per kb in microseconds:\t" << kbIndexTime << endl;

    saveIndex(indexTree.root, mainIndex, coordBlocks);

    cout << "term count:\t" << termCount << "\tterm size:\t" << termLength << endl;

    tokens.terminate();
    mainIndex.terminate();
    coordBlocks.terminate();

    return 0;
}

void saveIndex(AVLTermNode *node, BinaryMmap &mainIndex, BinaryMmap &coordBlocks, size_t at) {
    termCount++;
    size_t writePosition = at ?: mainIndex.currentPosition();

    mainIndex.writeInt(node->term.length(), TERM_SIZE_C, writePosition);
    mainIndex.writeStr(node->term);
    mainIndex.writeInt(node->docIdSet.size(), DOC_COUNT_C);
    mainIndex.writeInt(node->count, TOTAL_COUNT_C);
    mainIndex.writeInt(coordBlocks.currentPosition(), COORD_BLOCKS_O_C);

    writePosition = mainIndex.currentPosition();

    coordBlocks.writeCollection<set<unsigned int>, set<unsigned int>::iterator>(node->docIdSet);

    if (node->leftChild != nullptr)
        saveIndex(node->leftChild, mainIndex, coordBlocks, writePosition + RIGHT_O_C);

    if (node->rightChild != nullptr) {
        size_t rightChildOffset = mainIndex.currentPosition() + RIGHT_O_C;
        mainIndex.writeInt(rightChildOffset, RIGHT_O_C, writePosition);
        saveIndex(node->rightChild, mainIndex, coordBlocks, rightChildOffset);
    }

}

string &lowerCaseStr(string &str) {
    unsigned char *main = (unsigned char *)&(str[0]);
    unsigned char *head = 0;

    while (*main) {
        if ((*main >= 0x41) && (*main <= 0x5a)) { // US ASCII
            (*main) += 0x20;
        } else if (*main > 0xc0) {
            head = main;
            main++;
            switch (*head) {
                case 0xd0: // Cyrillic
                    if ((*main >= 0x80) && (*main <= 0x8f)) {
                        *head = 0xd1;
                        (*main) += 0x10;
                    } else if ((*main >= 0x90) && (*main <= 0x9f))
                        (*main) += 0x20; // US ASCII shift
                    else if ((*main >= 0xa0) && (*main <= 0xaf)) {
                        *head = 0xd1;
                        (*main) -= 0x20;
                    }
                    break;
                case 0xd1: // Cyrillic supplement
                    if ((*main >= 0xa0) && (*main <= 0xbf) && (!(*main % 2))) // Even
                        (*main)++;
                    break;
            }
            head = 0;
        }
        main++;
    }
    return str;
}