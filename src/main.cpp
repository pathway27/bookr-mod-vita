#include <psp2/kernel/processmgr.h>
#include <sstream>
#include <vector>

#include <cstdio>

using namespace std;

int main(int argc, char *argv[]) {
    stringstream output;
    vector<string> hello = { "Hello, world!" };

    for (auto &s : hello) {
        output << s;
    }
    output << endl;
    printf("%s\n", output.str().c_str());


    sceKernelExitProcess(0);
    return 0;
}
