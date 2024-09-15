#ifndef VMD_RENAMER_H
#define VMD_RENAMER_H

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
#define BONE_NAME_SIZE 15 //MMDではボーン名は15バイトまで
#define PATH_OUTPUT "result.vmd"

struct VMDHeader{
    char Version[30];
    char ModelName[20];
};

//モーションの1ボーン・1フレームあたりのデータ
struct VMDMotionFrame{
    char BoneName[BONE_NAME_SIZE];
    DWORD FrameNo;
    float Location[3];
    float Rotation[4];
    BYTE Interpolation[64];
};

//トランスフォーム制限の定義
struct Restriction {
    bool Movement;//in csv file, No symbol means restricted, "1" means free
    bool rotation;
    bool scale;
};

//ボーン名対応表のcsvファイルのデータを種類別に分けて格納。
struct BoneList {
    vector<string> frameBoneList;//モーションデータに定義された変換前のボーン名のリスト
    vector<vector<bool>> restrictionList;
    vector<string> newBoneList;//変換後のボーン名のリスト
};


// 表情データ数
struct VMDMorphHeader {
    unsigned long Count; // 表情データ数
} ;

struct VMDMorphFrame {
    char SkinName[16]; // 表情名
    unsigned long FrameNo; // フレーム番号
    float Weight; // 表情の設定値(表情スライダーの値)
};

// カメラデータ数
struct VMD_CAMERA_COUNT {
    unsigned long Count; // カメラデータ数
} ;


class VMD{
public:
    int BoneCount;
    int MorphCount;
    int CameraCount;
    VMDHeader Header;
    std::vector<VMDMotionFrame> MotionFrames;
    std::vector<VMDMorphFrame> MorphFrames;

    void Read(const char* filePath);
    std::vector<const char*> GetMorphList();
    VMD BoneRename(VMD vmd, BoneList boneList, const int n);
};

void printMotion(const VMDMotionFrame& motion);
BoneList inputBoneData(const char *filePath);
int found(string target, vector<string> nameList );
void saveVMD(const char* filePath, VMD vmd);
void removeNullChar(char* str);
string trim(string str);
std::wstring jisToUTF(const std::string& str);
std::string sjisToUtf8(const std::string& str);
std::string utf8ToJis(const std::string& utf8_str);
#endif
