#include <math.h>
#include "vmd.h"
#include <list>
#include <map>
#include <sstream>
#include <cstdlib>
#include <stdexcept>

//2024/8/24更新

#define SAMPLE_FILENAME "Test.fbx"
VMD vmd;

using namespace std;

int main(int argc, char **argv) {
    //Sample Input: ./main.c hoge.vmd oppagedon.csv
    const char* vmdPath = argv[1];
    const char* csvPath = argv[2];
    std::string n_str = argv[3];
    const int n_int = stoi(n_str);
    BoneList boneList = inputBoneData(csvPath);
    
    printf("Bone Number in csv file: %d\n", boneList.frameBoneList.size());

    vmd.Read(vmdPath);

    VMD renamedVMD = vmd.BoneRename(vmd, boneList, n_int);
    saveVMD(PATH_OUTPUT, renamedVMD);
    printf("End");

    return 0;
}