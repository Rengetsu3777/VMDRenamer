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

// *************  命名規則  *****************
//変数：先頭小文字 ＋ 単語先頭大文字
//メソッド：先頭小文字 ＋ 単語先頭大文字
//クラス、構造体名： 先頭大文字 ＋ 単語先頭大文字
//メンバ変数：変数と同様
//メンバ関数：先頭大文字 ＋ 単語先頭大文字
// *****************************************

struct VMDHeader{
    char version[30];
    char modelName[20];
};

//モーションの1ボーン・1フレームあたりのデータ
struct VMDMotionFrame{
    char boneName[BONE_NAME_SIZE];
    DWORD frameNo;
    float location[3];
    float rotation[4];
    BYTE interpolation[64];
};

//トランスフォーム制限の定義
struct Restriction {
    bool movement;//in csv file, No symbol means restricted, "1" means free
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
    unsigned long count; // 表情データ数
} ;

struct VMDMorphFrame {
    char skinName[16]; // 表情名
    unsigned long frameNo; // フレーム番号
    float weight; // 表情の設定値(表情スライダーの値)
};

// カメラデータ数
struct VMD_CAMERA_COUNT {
    unsigned long count; // カメラデータ数
} ;


class VMD{
public:
    int boneCount;
    int morphCount;
    int cameraCount;
    VMDHeader header;
    std::vector<VMDMotionFrame> motionFrames;
    std::vector<VMDMorphFrame> morphFrames;

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
