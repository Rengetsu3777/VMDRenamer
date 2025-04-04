#include <math.h>
#include "vmd.h"


VMD vmd;

using namespace std;

int main(int argc, char **argv) {
  
    const char* vmdPath = argv[1];
    const char* csvPath = argv[2];
    std::string transformScaleString = argv[3];
    const int transformScale = stoi(transformScaleString);
    BoneList boneList = inputBoneData(csvPath);
    int boneNumber = boneList.frameBoneList.size();
    printf("Info: Bone Number in csv file: %d\n", boneNumber);
    printf("Info: csvファイル内の定義ボーン数: %d\n", boneNumber);
    

    vmd.ReadVMD(vmdPath);

    VMD renamedVMD = vmd.RenameAndRescaleBone(vmd, boneList, transformScale);
    saveVMD(PATH_OUTPUT, renamedVMD);
    printf("Info: Successfully End the process\n");
    printf("Info: 正常に処理を完了しました。\n");

    return 0;
}
