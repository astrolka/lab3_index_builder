#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

int main() {

    string articlesPath;
    struct stat s;
    size_t articlesSize;
    size_t pageSize = getpagesize();

    cout << string("a").compare(string("b")) << endl;

    cout << "Enter path to file with articles" << endl;
    cin >> articlesPath;

    int articlesFD = open(articlesPath.c_str(), O_RDONLY);
    fstat(articlesFD, &s);

    articlesSize = s.st_size;
    articlesSize += pageSize - (articlesSize % pageSize);

    char *articles = (char *) mmap(0, articlesSize, PROT_READ, MAP_PRIVATE, articlesFD, 0);


    return 0;
}