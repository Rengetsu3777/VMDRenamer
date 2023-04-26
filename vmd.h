#ifndef VMDTOFBX_VMD_H
#define VMDTOFBX_VMD_H
typedef unsigned long DWORD;
typedef unsigned char BYTE;

#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <list>
#include <cstring>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <cctype>
using namespace std;
#define BONE_NAME_SIZE 15

struct VMDHeader{
    char Version[30];
    char ModelName[20];
    //unsigned long FrameData[4];
};

struct VMDMotionFrame{
    char BoneName[BONE_NAME_SIZE];
    DWORD FrameNo;
    float Location[3];
    float Rotation[4];
    BYTE Interpolation[64];
};

struct Restriction {
    bool Movement;//in csv file, No symbol means restricted, "1" means free
    bool rotation;
    bool scale;
};

struct BoneList {
    vector<string> frameBoneList;
    vector<vector<bool>> restrictionList;
    vector<string> newBoneList;
};

//
//
// 表情データ数
struct VMDMorphHeader {
    unsigned long Count; // 表情データ数
} ;

struct VMDMorphFrame {
    char SkinName[16]; // 表情名
    unsigned long FrameNo; // フレーム番号
    float Weight; // 表情の設定値(表情スライダーの値)
};
//
// カメラデータ数
struct VMD_CAMERA_COUNT {
    unsigned long Count; // カメラデータ数
} ;


class VMD{
public:
    int BoneCount;
    int MorphCount;
    int CameraCount;
//    int LightCount;
//    int ShadowCount;
    VMDHeader Header;
    std::vector<VMDMotionFrame> MotionFrames;
    std::vector<VMDMorphFrame> MorphFrames;

public :
    void Read(const char* filePath);
    std::vector<const char*> GetMorphList();
    VMD BoneRename(VMD vmd, BoneList boneList, int n);
    void setBoneOffset(float boneOffset) {
        this->BONE_OFFSET = boneOffset;
    }
private:
    float BONE_OFFSET;
};

void PrintMotion(const VMDMotionFrame& motion);
BoneList InputBoneData(const char *filePath);
int found(string target, vector<string> nameList );
void saveVMD(const char* filePath, VMD vmd);
void removeNullChar(char* str);
string trim(string str);
std::wstring jisToUTF(const std::string& str);
std::string sjisToUtf8(const std::string& str);
std::string utf8ToJis(const std::string& utf8_str);
//std::string utf8ToJis(std::string utf8Str);
void overwriteSJIS(char sjisStr[BONE_NAME_SIZE], std::string utf8Str);
void displayFileContents(const char* filePath);
void alignData(char* data, size_t dataSize, size_t alignment);
void printStringWithControlCharacters(const std::string& str);
void writeShiftJIS(std::ostream& os, const std::string& str);
#endif //VMDTOFBX_VMD_H
