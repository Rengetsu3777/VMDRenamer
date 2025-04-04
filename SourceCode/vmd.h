#pragma once

#include <string>
#include <vector>
#include <windows.h>

using namespace std;

typedef unsigned long DWORD;
typedef unsigned char BYTE;

#define BONE_NAME_SIZE 15 //MMDではボーン名は15バイトまで
#define PATH_OUTPUT "motionOutput/result.vmd"
#define BONE_LIST_RESTRICTION_FIRST_COLUMN 1 //ボーン対応表の制約条件列の最初の列
#define BONE_LIST_RESTRICTION_LAST_COLUMN 3 //ボーン対応表の制約条件列の最後の列
#define NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT 2 // csvファイル内での行数カウントとgetlineで取得した行の内部での行数カ
// ウントとの差分。


////////////////////////////////////////////////////
// Class: VMD
//  vmdのモーションファイルに対する読み込み、編集、書き込み処理を担当する。
////////////////////////////////////////////////////


// *************  命名規則  *****************
// 変数：先頭小文字 ＋ 単語先頭大文字
// 定数：大文字 ＋ 単語で_区切り
// メソッド：先頭小文字 ＋ 単語先頭大文字
// クラス、構造体名： 先頭大文字 ＋ 単語先頭大文字
// メンバ変数：変数と同様
// メンバ関数：先頭大文字 ＋ 単語先頭大文字
// *****************************************

// vmdモーションのヘッダー情報を格納する構造体
struct VMDHeader{
    char version[30];
    char modelName[20];
};

// モーションの1ボーン・1フレームあたりのデータ
struct VMDMotionFrame{
    char boneName[BONE_NAME_SIZE];
    DWORD frameNo;
    float location[3];
    float rotation[4];
    BYTE interpolation[64];
};

// トランスフォーム制限の定義
struct Restriction {
    bool movement;//in csv file, No symbol means restricted, "1" means free
    bool rotation;
    bool scale;
};

// ボーン名対応表のcsvファイルのデータを種類別に分けて格納。
struct BoneList {
    vector<string> frameBoneList;//モーションデータに定義された変換前のボーン名のリスト
    vector<vector<bool>> restrictionList;
    vector<string> newBoneList;//変換後のボーン名のリスト
};


// 表情ヘッダー情報の構造体
struct VMDMorphHeader {
    unsigned long count; // 表情データ数
} ;


// 表情フレーム情報の構造体
struct VMDMorphFrame {
    char skinName[16]; // 表情名
    unsigned long frameNo; // フレーム番号
    float weight; // 表情の設定値(表情スライダーの値)
};


// カメラデータ数
struct VMD_CAMERA_COUNT {
    unsigned long count; // カメラデータ数
} ;


// vmdモーションの情報を格納し、モーションに関連する関数を収録
class VMD{
public:
    int boneCount; // ヘッダーにある総ボーンフレーム数
    int morphCount; // ヘッダーにある総モーフフレーム数
    int cameraCount; // ヘッダーにある総カメラフレーム数
    BoneList boneList; // csvファイルから読み込んだボーンリスト
    VMDHeader header; // ヘッダー情報
    std::vector<VMDMotionFrame> motionFrames; // モーションのキーフレームのリスト
    std::vector<VMDMorphFrame> morphFrames; // 表情モーションのキーフレームのリスト

    int ReadVMD(const char* filePath);
    void RenameAndRescaleBone(const int n);
    void SaveVMD(const char* filePath); // 'filePath'に変換して生成された'vmd'のデータを出力する。
    
    // 'filePath'にこのツール専用のcsvデータを渡す。
    // 出力は、そのボーンデータをBoneList構造体に格納したデータを返す。失敗した時は、NullPtrを返し、処理を強制終了する。
    int InputBoneData(const char *filePath);

    // vmd形式のモーションデータ（'motion'）の情報表示。
    void PrintMotion(const VMDMotionFrame& motion);
};



// 以下VMDクラス以外の操作関連の関数


// ボーン名前リストの中でターゲットのボーン名を探す。
int foundTargetNameFromList(string target, vector<string> nameList);

// パスのフォルダーの存在判定
bool existPath(const char *pathChar, std::string targetExtension);

// 拡張子が正しいものか判定する。
bool isMatchedExtension(const char *pathChar, std::string targetExtention);

