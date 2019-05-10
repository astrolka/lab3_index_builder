#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <regex>

using namespace std;

string &lowerCaseStr(string &str);

int main() {

    string articlesPath = "/Users/fukingmac/MEGA/town_planing_articles/town_planing_articles.txt";
    struct stat s;
    size_t articlesSize;
    size_t pageSize = getpagesize();

    cout << "Enter path to file with articles" << endl;
//    cin >> articlesPath;

    int articlesFD = open(articlesPath.c_str(), O_RDONLY);
    fstat(articlesFD, &s);

    articlesSize = s.st_size;
    articlesSize += pageSize - (articlesSize % pageSize);

    char *text = (char *) mmap(0, articlesSize, PROT_READ, MAP_PRIVATE, articlesFD, 0);

    string textObj(text);

    regex wordRX("(?:\\w|\\xD0[\\x80-\\xBF]|\\xD1[\\x80-\\x9F]|\\xCC[\\x80-\\xBB])+");
    regex boundaryRX("wiki_search_engine_38hf91\\|title=(.+)\\|pageId=(\\d+)\\|docId=(\\d+)\\|size=(\\d+)");
    regex graveRX("\\xCC[\\x80-\\xBB]");

    for (sregex_iterator it(textObj.begin(), textObj.end(), boundaryRX); it != sregex_iterator(); ++it) {

        smatch m = *it;

        string articleTitle = m.str(1);
        int pageId = atoi(m.str(2).c_str());
        int docId = atoi(m.str(3).c_str());
        int articleSize = atoi(m.str(4).c_str());

        string article = textObj.substr(m.position() + m.str().length() + 1, articleSize);
        for (sregex_iterator it(article.begin(), article.end(), wordRX); it != sregex_iterator(); ++it) {
            smatch m = *it;
            string word = regex_replace(m.str(), graveRX, "");
            lowerCaseStr(word);
        }

    }

    return 0;
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