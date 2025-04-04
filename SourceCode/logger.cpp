#include "logger.h"

#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include "stringEdit.h"
#include <locale>


bool Logger::IsOpenedLogFile() { return this->LogStream.is_open(); }


void Logger::OpenLogFile(const std::string &filePath) {
  if (this->IsOpenedLogFile() == false) {
    this->LogStream.open(filePath, std::ios::out);
    std::cout << "opening log file!: " << this->IsOpenedLogFile() << std::endl;
    this->error = 0;
  } else {
    this->error = -1;
  }
}


void Logger::CloseLogFile() {
  if (this->IsOpenedLogFile() == true) {
    this->LogStream.close();
  }
}


//string型で日付、時間を取得.
std::string Logger::GetTime() {
  std::time_t now = std::time(nullptr);
  return ctime(&now);
}


// 日本語基準でのログ出力のためのロケール設定
// languageは"en_US.UTF8"を使えばよい
void Logger::LogLocale(std::string language) {
  this->Locale = std::locale(language);
  this->LogStream.imbue(std::locale(language));
}


//エラーフラグクラス変数の取得
int Logger::GetErrorFlag() {
  return this->error;
}


// ログファイルデータ書き込み
void Logger::WriteLogFile(const std::string &log) {
  if (this->IsOpenedLogFile() == true) {
    //this->LogStream << utf8ToUft16(log) << std::endl;
    this->LogStream << log << std::endl;
  }
}

// Errorのログ関数
void Logger::LogError(const std::string &log) {
  if(IsOpenedLogFile()) {
    std::string now = this->GetTime();
    replaceString(now, "\n", "");
    this->LogStream << now << " [Error] " << log << std::endl;
  }
}

// Warningのログ関数
void Logger::LogWarning(const std::string &log) {
  if(IsOpenedLogFile()) {
    std::string now = this->GetTime();
    replaceString(now, "\n", "");
    this->LogStream << now << " [Warning] " << log << std::endl;
  }
}

// Infoのログ関数
void Logger::LogInfo(const std::string &log) {
  if(IsOpenedLogFile()) {
    std::string now = this->GetTime();
    replaceString(now, "\n", "");
    this->LogStream << now << " [Info] " << log << std::endl;
    this->LogStream.imbue(this->Locale);
  }
}
