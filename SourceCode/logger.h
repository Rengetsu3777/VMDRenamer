#pragma once
#include <fstream>
#include <string>

#include "stringFormat.h"


////////////////////////////////////////////////////
// Class: Logger
//  ログレベル別のログの書き込み、オープン処理等を担当する。
//  シングルトンのクラスとして定義。
////////////////////////////////////////////////////


class Logger {

public:
  
  //外部ファイル向け初期化処理
  static void Init(std::string logPath) {
    if (instance != nullptr) {
      return;
    }
    instance = new Logger();

    instance->OpenLogFile(logPath);
  }


  // 外部ファイル向けデストラクタ処理
  static void Uninit() {
    if (instance) {
      // インスタンス削除
      delete instance;

      instance = nullptr;
    }
  }


  //シングルトンクラスオブジェクト（クラス内にインスタンスを持ち、外部ファイルでは常にこのクラスは1つしか存在できない。）
  static Logger *GetInstance() { return instance; }

  void LogInfo(const std::string& log);
  void LogWarning(const std::string &log);
  void LogError(const std::string& log);

  std::locale Locale;

public:

  //書式指定機能追加
  template<typename... Args>
  static void Error(const std::string& log, Args&&...args)
  {
    //書式フォーマットを行って、シングルトンインスタンスのErrorログの呼び出し
    instance->LogError(StringFormat(log, std::forward<Args>(args) ...));
  }

  //書式指定機能追加
  template<typename... Args>
  static void Warning(const std::string& log, Args&&...args)
  {
    //書式フォーマットを行って、シングルトンインスタンスのErrorログの呼び出し
    instance->LogWarning(StringFormat(log, std::forward<Args>(args) ...));
  }

  // 書式指定機能追加
  template<typename... Args>
  static void Info(const std::string& log, Args&&...args)
  {
    //書式フォーマットを行って、シングルトンインスタンスのErrorログの呼び出し
    instance->LogInfo(StringFormat(log, std::forward<Args>(args) ...));
  }
  
  
private:
  std::string GetTime();

private:

  // コンストラクタ
  Logger() {

  }

  // デストラクタ
  ~Logger() {

  }

public:
  
  void OpenLogFile(const std::string &filePath);

  void CloseLogFile();

  // ファイルオープンの確認
  bool IsOpenedLogFile();

  // ログファイルデータ書き込み
  void WriteLogFile(const std::string &log);

  //日本語基準でのログ出力のためのロケール設定
  void LogLocale(std::string language);
      
  //エラーフラグクラス変数の取得
  int GetErrorFlag();

private:
  static Logger* instance;
  
  // ログ// ファイル出力先パス
  const std::string LogFilePath;

  std::ofstream LogStream;
  std::filebuf* FileBuf;
  int error;
};

