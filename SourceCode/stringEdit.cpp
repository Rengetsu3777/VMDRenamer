#include "stringEdit.h"

#include <cstdio>
#include <cstring>
#include <codecvt>
#include <filesystem>
#include <locale>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <windows.h>


// 'str'の文字列に対して、空白文字や制御文字のトリミング
// ただし、'str'文字列の先頭と末尾にしか走査しない。
std::string trim(std::string str) {
    char *cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    // removeNullChar(cstr); // 最後に「\0」があれば除去する
    char *head = cstr;
    char *tail = cstr + strlen(cstr) - 1;
    while (*head == ' ' || *head == '\t' || *head == '\n' || *head == '\r') {
      ++head;
    }
    while (*tail == ' ' || *tail == '\t' || *tail == '\n' || *tail == '\r') {
      --tail;
    }
    *(tail + 1) = '\0';
    std::string result(head);
    delete[] cstr;
    return result;
  }

  
  //baseStringの中にあるtargetCharacterの文字列の先頭文字の位置をリストで返す。
  std::vector<size_t> findCharacterPosition(std::string baseString, std::string targetCharacter) {
    
    size_t stringSize = targetCharacter.size(); //探索対象の文字列のバイト数
    std::vector<size_t> positions; //探索対象の文字列がある位置のリスト
    size_t position = baseString.find(targetCharacter);
  
    while (position != std::string::npos) {//find関数で見つからなかった時にstd::string::nposになる。
      positions.push_back(position);
      position = baseString.find(targetCharacter, position + stringSize);
    }
  
    return positions;
  }


  //baseStringの中にあるtargetStringの部分の文字列をsetStringに書き換える処理
  void replaceString(std::string &baseString, std::string targetString, std::string setString) {
  
    int loopCount = 0;
    size_t targetStringLength = targetString.length();
    size_t stringLengthDiff = setString.length() - targetStringLength;

    std::vector<size_t> targetCharacterPositions =
        findCharacterPosition(baseString, targetString);
    
    for (auto position : targetCharacterPositions) {
      baseString.replace(position + loopCount * stringLengthDiff, targetStringLength, setString);
    }
  }

  
  //現在の作業フォルダーのパス取得
  std::string getCodeFolderPath() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string currentPathString = currentPath.u8string(); //string型に変換
    replaceString(currentPathString, "\\", "/");
    return currentPathString;
  }

  
  // Shift-JISからUTF-8への変換
  std::wstring jisToUTF(const std::string &str) {
    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], wsize);
    return wstr;
  }
  
  
  // Shift-JISからUTF-8への変換（新）
  std::string sjisToUtf8(const std::string &str) {

    // Shift-JISからUTF-16に変換
    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], wsize);
  
    // UTF-16からUTF-8に変換
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, NULL,NULL);
  
    return result;
  }
  
  
  std::string utf8ToJis(const std::string &utf8_str) {

    // UTF-8からUTF-16に変換
    int wstr_size = ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    std::wstring wstr(wstr_size, L'\0');
    ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wstr_size);
  
    // UTF-16からShift-JISに変換
    int buffer_size =::WideCharToMultiByte(932, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string buffer(buffer_size, '\0');
    ::WideCharToMultiByte(932, 0, wstr.c_str(), -1, &buffer[0], buffer_size,
                          NULL, NULL);
    
    return buffer;
  }

  
  // UTF-8 -> UTF-16 への変換
  // UTF-16として読む文字列をwstring型として返す。
  std::wstring utf8ToUft16(std::string utf8String) {
    
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8String);
  }