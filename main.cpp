#include <math.h>
#include "vmd.h"
#include <list>
#include <map>
#include <sstream>
#include <cstdlib>
#include <stdexcept>


#define SAMPLE_FILENAME "Test.fbx"
VMD vmd;

using namespace std;

int main(int argc, char **argv) {
    //Sample Input: ./main.c hoge.vmd oppagedon.csv
    const char* vmdPath = argv[1];
    const char* csvPath = argv[2];
    std::string n_str = argv[3];
    int n_int = stoi(n_str);
    const char* newVMDPath = "result.vmd";
    BoneList boneList = InputBoneData(csvPath);
    printf("frameBoneList's size%d\n", boneList.frameBoneList.size());

    vmd.Read(vmdPath);

    VMD renamedVMD = vmd.BoneRename(vmd, boneList, n_int);
    saveVMD(newVMDPath, renamedVMD);
    printf("End");

    return 0;
}