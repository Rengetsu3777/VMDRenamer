#include "vmd.h"

void VMD::Read(const char *filePath) {
    // ファイルのOpen
    auto fp = fopen(filePath, "rb");
    if(fp == NULL) {
        printf("File cannot be opened\n");
    }
    //VMDHeader vmdHeader;

    fread(&Header, sizeof(Header), 1, fp);
    fread(&BoneCount, sizeof(int), 1, fp);

    //MotionFrames = new std::vector<VMDMotionFrame>(BoneCount);
    MotionFrames.resize(BoneCount);
    printf("bone count: %d\n", BoneCount);
    printf("Version: %s\n", Header.Version);
    printf("Model Name: %s\n", Header.ModelName);
    for (auto &motion: MotionFrames) {
        fread(&motion.BoneName[0], sizeof(char), BONE_NAME_SIZE, fp);

        fread(&motion.FrameNo, sizeof(motion.FrameNo), 1, fp);
        fread(&motion.Location, sizeof(motion.Location), 1, fp);
        fread(&motion.Rotation, sizeof(motion.Rotation), 1, fp);
        fread(&motion.Interpolation, sizeof(motion.Interpolation), 1, fp);
        //PrintMotion(motion);//�?バッグ
    }
    fread(&MorphCount, sizeof(int), 1, fp);
    MorphFrames.resize(MorphCount);
    for (auto &morph: MorphFrames) {
        fread(&morph.SkinName, sizeof(morph.SkinName), 1, fp);
        fread(&morph.FrameNo, sizeof(morph.FrameNo), 1, fp);
        fread(&morph.Weight, sizeof(morph.Weight), 1, fp);
    }


    fread(&CameraCount, sizeof(int), 1, fp);

    // ファイルのClose
    fclose(fp);
}


bool contains(std::vector<const char *> &listOfElements, const char *element) {
    for (const auto &item: listOfElements)
        if (std::string(item) == std::string(element))
            return true;
    return false;
}


std::vector<const char *> VMD::GetMorphList() {
    std::vector<const char *> morphList;
    for (auto &morph: MorphFrames)
        if (!contains(morphList, morph.SkinName))
            morphList.push_back(morph.SkinName);
    return morphList;
}

BoneList InputBoneData(const char *filePath) {
    ifstream file(filePath);
    //displayFileContents(filePath);//UTF-8で表示!!!

    string line, frameBoneName, newBoneName, frameBoneName_Jis, newBoneName_Jis;
    BoneList boneList;
    // Skip header row
    getline(file, line);
    // Read data rows

    int j = 0;

    while (getline(file, line)) {
        //printf("DDD\n");
        stringstream ss(line);
        getline(ss, frameBoneName, ',');  // Read first column

        boneList.restrictionList.emplace_back();

        for (int i = 1; i < 4; i++) {
            //printf("EEE\n");
            getline(ss, line, ',');  // Skip columns 2-4
            if(line.empty()) {
                //printf("aaa");
                boneList.restrictionList[j].push_back(false);
                //boneList.restrictionList[j][i] = false;
            } else {
                //printf("bbb");
                boneList.restrictionList[j].push_back(true);
                //boneList.restrictionList[j][i] = true;
            }
            //printf("ccc");
        }

        getline(ss, newBoneName, ',');  // Read fifth column
        if (!frameBoneName.empty()) {
            boneList.frameBoneList.push_back(frameBoneName);//変換なしのpush_backUTF-8の文字列。
        }
        if (!newBoneName.empty()) {
            boneList.newBoneList.push_back(newBoneName);//変換なしのpush_back。UTF-8の文字列
        }
        j++;
    }

    //fclose(fp);
    return boneList;
}

int found(string target, vector<string> nameList ) {
    for(int i = 0; i<nameList.size(); i++) {        
        if(trim(target) == trim(nameList[i])) {
             return i;
        }
    }
    return -1;
}

VMD VMD::BoneRename(VMD vmd, BoneList boneList, int n) {
    //Boneのリネームと、ボーンの移動量をn倍
    VMD newVMD = vmd;//VMDの指すポインタをコピーして同じところを参照
    char boneName[BONE_NAME_SIZE];
    DWORD beforeFrameNo = 0;
    bool flag = false;
    int index;
    int i = 0;
    int k = 0;
    string boneListTF_str;
    for(auto& keyFrame : vmd.MotionFrames) {
        index = found(sjisToUtf8(keyFrame.BoneName), boneList.frameBoneList);
        
        //std::cout << "keyFrame->BoneName: " << sjisToUtf8(keyFrame.BoneName) << std::endl;
        //表示用

        //printf("index: %d\n", index);
        //表示用
        
        //移動量をn倍
        if(index >= 0) {
            //boneListTF_str = boneList.restrictionList[index][0];
            //boneListTF_str = stoi(boneListTF_str);
            //std::cout << "nboneList.restrictionList[index][0] : " << boneList.restrictionList[index][0] << std::endl;
            if(boneList.restrictionList[index][0]) {
                for(k = 0; k<3; k++) {
                    keyFrame.Location[k] *= n;
                }
            }
        }

        if(index >= 0) {
            if(strcmp(boneList.newBoneList[index].c_str(), "-") != 0) {
                //std::cout << "newBoneName : " << boneList.newBoneList[index] <<    std::endl;
                //表示用
                //std::cout << "BoneName1 : " << sjisToUtf8(keyFrame.BoneName) << std::endl;
                //表示用

                string newBoneJis = utf8ToJis(boneList.newBoneList[index]);
                for(int j = 0; j< BONE_NAME_SIZE-1;j++) {
                    keyFrame.BoneName[j] = newBoneJis[j];
                    //std::cout << "newBoneJis[j] : " << sjisToUtf8(newBoneJis) << std::endl;
                    //表示用
                }
            }
        }
        i++;
    }
    return vmd;
}

void saveVMD(const char* filePath, VMD vmd) {
    // VMDファイルを書き込みモードで開く
    std::ofstream ofs(filePath, std::ios::binary);
    // VMDHeaderを書き込む
    ofs.write(reinterpret_cast<const char*>(&vmd.Header), sizeof(vmd.Header));

    // ボ�?�ンモーションフレー�?数を書き込む
    int boneCount = vmd.MotionFrames.size();
    ofs.write(reinterpret_cast<const char*>(&boneCount), sizeof(int));
    
    // ボ�?�ンモーションフレー�?を書き込む
    for (const auto& frame : vmd.MotionFrames) {

        ofs.write(reinterpret_cast<const char*>(&frame.BoneName), BONE_NAME_SIZE);
        //writeShiftJIS(ofs, frame.BoneName);
        ofs.write(reinterpret_cast<const char*>(&frame.FrameNo), sizeof(DWORD));
        ofs.write(reinterpret_cast<const char*>(&frame.Location), sizeof(frame.Location));
        ofs.write(reinterpret_cast<const char*>(&frame.Rotation), sizeof(frame.Rotation));
        ofs.write(reinterpret_cast<const char*>(&frame.Interpolation), sizeof(frame.Interpolation));

    //float Location[3];
    //float Rotation[4];
    //BYTE Interpolation[64];
        //ofs.write(reinterpret_cast<const char*>(&frame), sizeof(frame));
    }

    // 表�?モーションフレー�?数を書き込む
    int morphCount = vmd.MorphFrames.size();
    ofs.write(reinterpret_cast<const char*>(&morphCount), sizeof(int));

    // 表�?モーションフレー�?を書き込む
    for (const auto& frame : vmd.MorphFrames) {
        ofs.write(reinterpret_cast<const char*>(&frame), sizeof(frame));
    }

    // カメラモーションフレー�?数を書き込む
    int cameraCount = vmd.CameraCount;
    ofs.write(reinterpret_cast<const char*>(&cameraCount), sizeof(int));

    // ファイルを閉じる
    ofs.close();
}


void PrintMotion(const VMDMotionFrame& motion) {
    //shift-JISの�?字�?�をUTF-8に変換してボ�?�ンの名前を表示する
        
    std::wstring utf16str = jisToUTF(motion.BoneName);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string utf8str = converter.to_bytes(utf16str);

    std::cout << "BoneName: " << utf8str << std::endl;
    std::cout << "FrameNo: " << motion.FrameNo << std::endl;
    std::cout << "Location: (" << motion.Location[0] << ", " << motion.Location[1] << ", " << motion.Location[2] << ")" << std::endl;
    std::cout << "Rotation: (" << motion.Rotation[0] << ", " << motion.Rotation[1] << ", " << motion.Rotation[2] << ", " << motion.Rotation[3] << ")" << std::endl;
    std::cout << "Interpolation: (" << motion.Interpolation[0] << ", " << motion.Interpolation[1] << ", " << motion.Interpolation[2] << ", " << motion.Interpolation[3] << ")" << std::endl;
}


//「\0」を除去する関数。trim関数で用いる。
void removeNullChar(char* str) {
    int len = strlen(str);
    if (len > 0 && str[len-1] == '\0') {
        str[len-1] = '\0';
    }
}

//空白文字や制御文字のトリミング
string trim(string str) {
    char* cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    removeNullChar(cstr); // 最後に「\0」があれば除去する
    char* head = cstr;
    char* tail = cstr + strlen(cstr) - 1;
    while (*head == ' ' || *head == '\t' || *head == '\n' || *head == '\r') {
        ++head;
    }
    while (*tail == ' ' || *tail == '\t' || *tail == '\n' || *tail == '\r') {
        --tail;
    }
    *(tail + 1) = '\0';
    string result(head);
    delete[] cstr;
    return result;
}

//Shift-JISからUTF-8への変換（旧）

std::wstring jisToUTF(const std::string& str) {
    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], wsize);
    return wstr;
}

//↓Shift-JISからUTF-8への変換（新）
std::string sjisToUtf8(const std::string& str) {
    // Shift-JISからUTF-16に変換
    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], wsize);

    // UTF-16からUTF-8に変換
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, NULL, NULL);

    return result;
}

std::string utf8ToJis(const std::string& utf8_str)
{
    // UTF-8からUTF-16に変換
    int wstr_size = ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    std::wstring wstr(wstr_size, L'\0');
    ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wstr_size);

    // UTF-16からShift-JISに変換
    int buffer_size = ::WideCharToMultiByte(932, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string buffer(buffer_size, '\0');
    ::WideCharToMultiByte(932, 0, wstr.c_str(), -1, &buffer[0], buffer_size, NULL, NULL);

    return buffer;
}

void overwriteSJIS(char sjisStr[BONE_NAME_SIZE], std::string utf8Str) {
    // UTF-8からShift-JISへの変換器を用意する
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
    std::wstring utf16Str = cv.from_bytes(utf8Str); // UTF-8をUTF-16に変換する

    // UTF-16からShift-JISへの変換器を用意する
    int sjisLen = MultiByteToWideChar(CP_ACP, 0, sjisStr, -1, NULL, 0); // Shift-JIS文字列のバイト数を取得する
    wchar_t* sjisWStr = new wchar_t[sjisLen];
    MultiByteToWideChar(CP_ACP, 0, sjisStr, -1, sjisWStr, sjisLen); // Shift-JISをUTF-16に変換する
    char* newSJIS = new char[16];
    WideCharToMultiByte(CP_ACP, 0, utf16Str.c_str(), -1, newSJIS, 16, NULL, NULL); // UTF-16をShift-JISに変換する

    // 上書きする
    std::memcpy(sjisStr, newSJIS, 16);
    delete[] sjisWStr;
    delete[] newSJIS;
}

void displayFileContents(const char* filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filePath << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
       cout << line << endl;
    }
    file.close();
}

void alignData(char* data, size_t dataSize, size_t alignment) {
    // アライメントに必要なパディングサイズを計算する
    size_t paddingSize = alignment - (dataSize % alignment);
    // データのサイズにパディングサイズを加えたサイズのメモリを確保する
    char* alignedData = new char[dataSize + paddingSize];
    // パディングサイズ分の空白を埋める
    std::memset(alignedData + dataSize, 0, paddingSize);
    // データをコピーする
    std::memcpy(alignedData, data, dataSize);
    // 元のデータを削除する
    delete[] data;
    // アライメントを調整したデータにポインタを変更する
    data = alignedData;
}

void printStringWithControlCharacters(const std::string& str) {
    for (char c : str) {
        if (c == '\n') {
            std::cout << "\\n";
        } else if (c == '\r') {
            std::cout << "\\r";
        } else if (c == '\t') {
            std::cout << "\\t";
        } else if (c == '\0') {
            std::cout << "\\0";
        } else {
            std::cout << c;
        }
    }
}

void writeShiftJIS(std::ostream& os, const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    std::wstring wideStr = conv.from_bytes(str);
    std::locale loc("ja_JP.SJIS");
    const std::codecvt<wchar_t, char, std::mbstate_t>& codec = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
    std::vector<char> buffer(wideStr.length() * codec.max_length());
    std::mbstate_t state = std::mbstate_t();
    const wchar_t* fromNext;
    char* toNext;
    codec.out(state, wideStr.data(), wideStr.data() + wideStr.length(), fromNext, buffer.data(), buffer.data() + buffer.size(), toNext);
    os.write(buffer.data(), toNext - buffer.data());
}