#pragma once

#include <cstdio>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <windows.h>


// 'str'の文字列に対して、空白文字や制御文字のトリミング
// ただし、'str'文字列の先頭と末尾にしか走査しない。
std::string trim(std::string str);

//baseStringの中にあるtargetCharacterの文字列の先頭文字の位置をリストで返す。
std::vector<size_t> findCharacterPosition(std::string baseString, std::string targetCharacter); 

//baseStringの中にあるtargetStringの部分の文字列をsetStringに書き換える処理
void replaceString(std::string &baseString, std::string targetString, std::string setString); 

//現在の作業フォルダーのパス取得
std::string getCodeFolderPath();


// Shift-JIS -> UTF-8 への変換
std::wstring jisToUTF(const std::string &str);

// Shift-JIS -> UTF-8 への変換
std::string sjisToUtf8(const std::string &str);

//UTF-8 -> Jis への変換
std::string utf8ToJis(const std::string &utf8_str);

// UTF-8 -> UTF-16 への変換。
// UTF-16として読む文字列をwstring型として返す。
std::wstring utf8ToUft16(std::string utf8String);