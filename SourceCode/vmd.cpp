
#include "vmd.h"

#include <cstdio>
#include <cstring>
#include <codecvt>
#include <fstream>
#include <locale>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <sys/stat.h>
#include <locale>
#include <codecvt>
#include "stringEdit.h"

#include "logger.h"


// VMDモーションデータを読み込む処理
int VMD::ReadVMD(const char* filePath) {
  
  // ファイルのOpen
  auto fp = fopen(filePath, "rb");

  //vmdファイルとして開くてるか？
  if (fp == NULL || !isMatchedExtension(filePath, "vmd")) {
    printf("Error: vmd File cannot be opened\n");
    printf("エラー: vmdファイルを開けません。\n");
    std::cout << filePath << std::endl;
    Logger::GetInstance()->Error("エラー: vmdファイルを開けません。");
    return 3;
  }


  // ヘッダー読み込み
  fread(&header, sizeof(header), 1, fp);
  fread(&boneCount, sizeof(int), 1, fp);

  motionFrames.resize(boneCount);

  //ログ出力
  printf("Model Name: %s\n", header.modelName);
  printf("Version: %s\n", header.version);
  printf("bone count: %d\n", boneCount);
  Logger::GetInstance()->Info("======================================================");
  Logger::GetInstance()->Info("Model Name（モデル名）: %s", header.modelName);
  Logger::GetInstance()->Info("Version（バージョン）: %s", header.version);
  Logger::GetInstance()->Info("bone count（ボーン数）: %d", boneCount);
  Logger::GetInstance()->Info("======================================================\n\n");

  // トランスフォームデータの読み込み。1フレーム1ボーンごとループ
  for (auto &motion : motionFrames) {
    fread(&motion.boneName[0], sizeof(char), BONE_NAME_SIZE, fp);

    fread(&motion.frameNo, sizeof(motion.frameNo), 1, fp);
    fread(&motion.location, sizeof(motion.location), 1, fp);
    fread(&motion.rotation, sizeof(motion.rotation), 1, fp);
    fread(&motion.interpolation, sizeof(motion.interpolation), 1, fp);
  }

  // モーフデータ読み込み
  fread(&morphCount, sizeof(int), 1, fp);
  morphFrames.resize(morphCount);
  for (auto &morph : morphFrames) {
    fread(&morph.skinName, sizeof(morph.skinName), 1, fp);
    fread(&morph.frameNo, sizeof(morph.frameNo), 1, fp);
    fread(&morph.weight, sizeof(morph.weight), 1, fp);
  }

  // カメラモーションデータの読み込み
  fread(&cameraCount, sizeof(int), 1, fp);

  fclose(fp);

  return 0;
}


// Boneのリネームと、ボーンの移動量をn倍
// boneListにはボーン対応表の情報をまとめた変数で、transformScaleが移動倍率を何倍にするかの値。
void VMD::RenameAndRescaleBone(int transformScale) {

  char boneName[BONE_NAME_SIZE];
  int index;
  int axisIndex = 0;//x,y,z軸のインデックス.
  for (auto &keyFrame : this->motionFrames) {

    // ボーン名をボーン対応リストから検索
    index = foundTargetNameFromList(sjisToUtf8(keyFrame.boneName), this->boneList.frameBoneList);

    // 移動量をn倍
    if (index >= 0) {

      //倍率変更制限に基づき、トランスフォームを変更。
      if (this->boneList.restrictionList[index][0]) {
        for (axisIndex = 0; axisIndex < 3; axisIndex++) {//x, y, z軸に対してループ
          keyFrame.location[axisIndex] *= transformScale;
        }
      }
      
      //新しいボーン名の欄が「-」だったら、変換先ボーンがないということで、飛ばす。
      if (strcmp(this->boneList.newBoneList[index].c_str(), "-") != 0) {
        string newBoneJis = utf8ToJis(this->boneList.newBoneList[index]);
        for (int j = 0; j < BONE_NAME_SIZE - 1; j++) {
          keyFrame.boneName[j] = newBoneJis[j];
        }
      }

    }
  }
}


// 変換したモーションデータで、VMDファイルを生成する
// void saveVMD(const char *filePath, VMD vmd) {
void VMD::SaveVMD(const char *filePath) {

  // VMDファイルをバイナリモードで開く
  std::ofstream ofs(filePath, std::ios::binary);

  // VMDHeaderを書き込む
  ofs.write(reinterpret_cast<const char *>(&this->header), sizeof(this->header));

  // ボーンモーションフレーム数を書き込む
  int boneCount = this->motionFrames.size();
  ofs.write(reinterpret_cast<const char *>(&boneCount), sizeof(int));

  // ボーンモーションフレームを書き込む
  for (const auto &frame : this->motionFrames) {
    ofs.write(reinterpret_cast<const char *>(&frame.boneName), BONE_NAME_SIZE);
    ofs.write(reinterpret_cast<const char *>(&frame.frameNo), sizeof(DWORD));
    ofs.write(reinterpret_cast<const char *>(&frame.location),
              sizeof(frame.location));
    ofs.write(reinterpret_cast<const char *>(&frame.rotation),
              sizeof(frame.rotation));
    ofs.write(reinterpret_cast<const char *>(&frame.interpolation),
              sizeof(frame.interpolation));
  }

  // 表情モーションフレーム数を書き込む
  int morphCount = this->morphFrames.size();
  ofs.write(reinterpret_cast<const char *>(&morphCount), sizeof(int));

  // 表情モーションフレームを書き込む
  for (const auto &frame : this->morphFrames) {
    ofs.write(reinterpret_cast<const char *>(&frame), sizeof(frame));
  }

  // カメラモーションフレーム数を書き込む
  int cameraCount = this->cameraCount;
  ofs.write(reinterpret_cast<const char *>(&cameraCount), sizeof(int));

  ofs.close();
}




// 文字列リストに、指定の文字列が含まれてるか判定
bool isContains(std::vector<const char *> &listOfElements, const char *element) {
  for (const auto &item : listOfElements)
    if (std::string(item) == std::string(element)) return true;
  return false;
}


// csvファイルの読み込み
int VMD::InputBoneData(const char *filePath) {

  string line, frameBoneName, newBoneName;
  int boneListLineNum = 0;

  ifstream file(filePath);
  if(!file || !isMatchedExtension(filePath, "csv")) {
    //エラーログ
    cout << "cannot open the csv file!"
         << endl;

    cout << "csvファイルを開けません。"
         << endl;

    Logger::GetInstance()->Error(" csvファイルを開けません。");
    return 1;
  }

  // データの行を読み込む ---------------------------
  // ヘッダーの行のスキップ
  getline(file, line);

  //1行ごとにループ
  while (getline(file, line)) {
    
    stringstream ss(line);

    // 1列目読み込み
    getline(ss, frameBoneName, ','); 

    this->boneList.restrictionList.emplace_back(); // 初期化

    // トランスフォーム倍率変更制限の読み込み (2～4列目)
    for (int columnNum = BONE_LIST_RESTRICTION_FIRST_COLUMN; columnNum <= BONE_LIST_RESTRICTION_LAST_COLUMN; columnNum++) {

      getline(ss, line, ',');  

      if (line.empty()) {
        this->boneList.restrictionList[boneListLineNum].push_back(false);
      } else {
        this->boneList.restrictionList[boneListLineNum].push_back(true);
      }
    }

    // 5列目読み込み
    getline(ss, newBoneName, ',');  

    if (!frameBoneName.empty()) {  
      // さっき読みこんだ1列目のデータを配列に入れる
      // 変換なしのpush_back（UTF-8の文字列）
      this->boneList.frameBoneList.push_back(frameBoneName);  

    } else {

      //エラーログ
      cout << "Error: Invalid Bone Name in the csv file! (column 1 line "
           << boneListLineNum + NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT 
           << ")" 
           << endl; //boneListLineNum

      cout << " エラー：入力ボーン名が不正です！ ( "
           << boneListLineNum + NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT
           << "行1列目)"
           << endl;

      Logger::GetInstance()->Error(" エラー：入力ボーン名が不正です！ ( %d行1列目)", boneListLineNum + NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT);

      return 2;
    }

    if (!newBoneName.empty()) {  // さっき読みこんだfifth columnのデータを配列に入れる
      // 変換なしのpush_back。UTF-8の文字列
      boneList.newBoneList.push_back(newBoneName);  

    } else {

      //エラーログ
      cout << "Error: Invalid New Bone Name (column 5, line "
           << boneListLineNum + NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT 
           << ")"
           << endl;

      cout << "エラー：入力新ボーン名が不正です！ ( "
           << boneListLineNum + NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT
           << "行5列目)" 
           << endl;

      Logger::GetInstance()->Error("エラー：入力新ボーン名が不正です！ ( %d行5列目)", boneListLineNum + NUM_DIFF_INTERNAL_AND_CSV_LINE_COUNT);
      
      return 2;
    }

    boneListLineNum++;
  }

  return 0;
}


// ボーン名前リストの中でターゲットのボーン名を探す。
int foundTargetNameFromList(string target, vector<string> nameList) {
  for (int i = 0; i < nameList.size(); i++) {
    if (trim(target) == trim(nameList[i])) {
      return i;
    }
  }
  return -1;
}


// パスのフォルダーの存在判定
bool existPath(const char *pathChar, std::string targetExtention) {

  struct _stat structStat;
  std::string pathString = pathChar;

   // 拡張子の文字位置を取得
  std::string::size_type extensionPosition = pathString.find_last_of('.');

  // 拡張子が無いか
  if (extensionPosition != std::string::npos) {
    
    //拡張子のみ切り出し
    std::string extension = pathString.substr(extensionPosition + 1);

    // パスにファイルがあり、拡張子が正しいか判定
    if (_stat(pathChar, &structStat) || extension != targetExtention) {
      return false;
    } else {
      return true; //拡張子通りのファイルがある場合True
    } 
  }
}


//拡張子が正しいものか判定する。pathCharにあるパスのファイルの拡張子がtargetExtensionに一致するか判定。
bool isMatchedExtension(const char *pathChar, std::string targetExtention) {
  struct _stat structStat;
  std::string pathString = pathChar;

  // 拡張子の文字位置を取得
  std::string::size_type extensionPosition = pathString.find_last_of('.');
  //拡張子のみ切り出し
  std::string extension = pathString.substr(extensionPosition + 1);

  // パスにファイルがあり、拡張子が正しいか判定
  if (extension == targetExtention) {
    return true;//拡張子通りのファイルがある場合True
  } else {
    return false; 
  } 
}


// ＝＝＝＝＝＝＝＝以下デバッグ用情報表示＝＝＝＝＝＝＝＝

void VMD::PrintMotion(const VMDMotionFrame &motion) {
  // shift-JISの文字列をUTF-8に変換してボーンの名前を表示する

  std::wstring utf16str = jisToUTF(motion.boneName);
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::string utf8str = converter.to_bytes(utf16str);

  std::cout << "BoneName: " << utf8str << std::endl;
  std::cout << "FrameNo: " << motion.frameNo << std::endl;
  std::cout << "Location: (" << motion.location[0] << ", " << motion.location[1]
            << ", " << motion.location[2] << ")" << std::endl;
  std::cout << "Rotation: (" << motion.rotation[0] << ", " << motion.rotation[1]
            << ", " << motion.rotation[2] << ", " << motion.rotation[3] << ")"
            << std::endl;
  std::cout << "Interpolation: (" << motion.interpolation[0] << ", "
            << motion.interpolation[1] << ", " << motion.interpolation[2]
            << ", " << motion.interpolation[3] << ")" << std::endl;
}
