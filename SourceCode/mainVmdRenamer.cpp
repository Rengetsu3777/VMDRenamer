#include "mainVmdRenamer.h"

#include <iostream>
#include <math.h>

#include "vmd.h"
#include "stringEdit.h"
#include "logger.h"


VMD vmd;
Logger *Logger::instance = nullptr;

int mainVmdRenamer(const char* vmdPath, const char* csvPath, int transformScale) {

    //ログ関連初期化
    std::string logPath = getCodeFolderPath() + "/log/.log";
    std::cout << "LogPath: " << logPath << std::endl;
    Logger::Init(logPath);
    int result = Logger::GetInstance()->GetErrorFlag();

     // ログファイルを開けているか
    if(result) {
      return result;
    }

    //VMDクラス初期化
    auto vmd = VMD();

    //csvファイル読み込み
    result = vmd.InputBoneData(csvPath);
    //csvデータは正しいか？
    if(result) {
      return result;
    }

    int boneNumber = vmd.boneList.frameBoneList.size();

    //ログ（ボーン数）
    printf("Info: Bone Number in csv file: %d\n", boneNumber);
    printf("Info: csvファイル内の定義ボーン数: %d\n", boneNumber);
    Logger::GetInstance()->Info("Bone Number in csv file: %d\n", boneNumber);

    //vmdファイル情報読み込み
    result = vmd.ReadVMD(vmdPath);
    //vmdファイルは正しく開けているか判定
    if(result) {
      return result;
    }

    //モーションデータ書き換え
    vmd.RenameAndRescaleBone(transformScale);

    //モーションデータ出力
    vmd.SaveVMD(PATH_OUTPUT);

    //終了ログ
    printf("Info: Successfully End the process\n");
    printf("Info: 正常に処理を完了しました。\n");
    Logger::GetInstance()->Info("Successfully End the process (正常に処理を完了しました。）\n");

    return 0;
}

/*
//デバッグ用メイン関数
int main(int argc, char* argv[]) {
  std::string filePath =
"D:/ProjectFile/programing/C++/VMD_Renamer/20240915/boneName3_Miku330_v3.04.csv";
  std::string vmdPath =
"D:/ProjectFile/programing/C++/VMD_Renamer/20240915/motion.vmd"; int
transformScale = 100; mainVmdRenamer(vmdPath.data(), filePath.data(),
transformScale);
}
*/