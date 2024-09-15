#include "vmd.h"

void VMD::Read(const char *filePath) {
    // ファイルのOpen
    auto fp = fopen(filePath, "rb");
    if(fp == NULL) {
        printf("vmd File cannot be opened\n");
    }

    //ヘッダー読み込み
    fread(&Header, sizeof(Header), 1, fp);
    fread(&BoneCount, sizeof(int), 1, fp);

    MotionFrames.resize(BoneCount);
    printf("bone count: %d\n", BoneCount);
    printf("Version: %s\n", Header.Version);
    printf("Model Name: %s\n", Header.ModelName);

    //トランスフォームデータの読み込み。1フレーム1ボーンごとループ
    for (auto &motion: MotionFrames) {
        fread(&motion.BoneName[0], sizeof(char), BONE_NAME_SIZE, fp);

        fread(&motion.FrameNo, sizeof(motion.FrameNo), 1, fp);
        fread(&motion.Location, sizeof(motion.Location), 1, fp);
        fread(&motion.Rotation, sizeof(motion.Rotation), 1, fp);
        fread(&motion.Interpolation, sizeof(motion.Interpolation), 1, fp);
    }

    //モーフデータ読み込み
    fread(&MorphCount, sizeof(int), 1, fp);
    MorphFrames.resize(MorphCount);
    for (auto &morph: MorphFrames) {
        fread(&morph.SkinName, sizeof(morph.SkinName), 1, fp);
        fread(&morph.FrameNo, sizeof(morph.FrameNo), 1, fp);
        fread(&morph.Weight, sizeof(morph.Weight), 1, fp);
    }

    //カメラモーションデータの読み込み
    fread(&CameraCount, sizeof(int), 1, fp);

    fclose(fp);
}

//リストに含まれてるか判定
bool isContains(std::vector<const char *> &listOfElements, const char *element) {
    for (const auto &item: listOfElements)
        if (std::string(item) == std::string(element))
            return true;
    return false;
}


std::vector<const char *> VMD::GetMorphList() {
    std::vector<const char *> morphList;
    for (auto &morph: MorphFrames)
        if (!isContains(morphList, morph.SkinName))
            morphList.push_back(morph.SkinName);
    return morphList;
}

//read csv file
BoneList inputBoneData(const char *filePath) {
    ifstream file(filePath);

    string line, frameBoneName, newBoneName, frameBoneName_Jis, newBoneName_Jis;
    BoneList boneList;
    // Skip header row
    getline(file, line);

    // Read data rows

    int j = 0;

    //read the csv file.
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, frameBoneName, ',');  // Read first column

        boneList.restrictionList.emplace_back();//initialize


        for (int i = 1; i < 4; i++) {
            getline(ss, line, ',');  // scan restriction setting. (2nd-4th columns)
            if(line.empty()) {
                boneList.restrictionList[j].push_back(false);
            } else {
                boneList.restrictionList[j].push_back(true);
            }
        }

        getline(ss, newBoneName, ',');  // Read fifth column

        if (!frameBoneName.empty()) {//さっき読みこんだfirst columnのデータを配列に入れる
            boneList.frameBoneList.push_back(frameBoneName);//変換なしのpush_backUTF-8の文字列。
        } else {
            cout << "Error: Invalid Bone Name in the csv file! (column 1 line " << j+2 << ")" << endl;
            cout << " エラー：入力ボーン名が不正です！ ( " << j+2 << "行1列目)" << endl;
            exit(0);
        }

        if (!newBoneName.empty()) {//さっき読みこんだfifth columnのデータを配列に入れる
            boneList.newBoneList.push_back(newBoneName);//変換なしのpush_back。UTF-8の文字列
        } else {
            cout << "Error: Invalid New Bone Name (column 5, line " << j+2 << ")" << endl;
            cout << "エラー：入力新ボーン名が不正です！ ( " << j+2 << "行5列目)" << endl;
            exit(0);
        }
        j++;
    }

    //fclose(fp);
    return boneList;
}

//search for the target in the nameList
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

        //ボーン名をボーン対応リストから検索
        index = found(sjisToUtf8(keyFrame.BoneName), boneList.frameBoneList);
        
        //移動量をn倍
        if(index >= 0) {
            if(boneList.restrictionList[index][0]) {
                for(k = 0; k<3; k++) {
                    keyFrame.Location[k] *= n;
                }
            }

            //
            if(strcmp(boneList.newBoneList[index].c_str(), "-") != 0) {
                string newBoneJis = utf8ToJis(boneList.newBoneList[index]);
                for(int j = 0; j< BONE_NAME_SIZE-1;j++) {
                    keyFrame.BoneName[j] = newBoneJis[j];
                }
            }
        }
        i++;
    }
    return vmd;
}

void saveVMD(const char* filePath, VMD vmd) {
    // VMDファイルをバイナリモードで開く
    std::ofstream ofs(filePath, std::ios::binary);
    // VMDHeaderを書き込む
    ofs.write(reinterpret_cast<const char*>(&vmd.Header), sizeof(vmd.Header));

    // ボーンモーションフレーム数を書き込む
    int boneCount = vmd.MotionFrames.size();
    ofs.write(reinterpret_cast<const char*>(&boneCount), sizeof(int));
    
    // ボーンモーションフレームを書き込む
    for (const auto& frame : vmd.MotionFrames) {

        ofs.write(reinterpret_cast<const char*>(&frame.BoneName), BONE_NAME_SIZE);
        ofs.write(reinterpret_cast<const char*>(&frame.FrameNo), sizeof(DWORD));
        ofs.write(reinterpret_cast<const char*>(&frame.Location), sizeof(frame.Location));
        ofs.write(reinterpret_cast<const char*>(&frame.Rotation), sizeof(frame.Rotation));
        ofs.write(reinterpret_cast<const char*>(&frame.Interpolation), sizeof(frame.Interpolation));

    }

    // 表情モーションフレーム数を書き込む
    int morphCount = vmd.MorphFrames.size();
    ofs.write(reinterpret_cast<const char*>(&morphCount), sizeof(int));

    // 表情モーションフレームを書き込む
    for (const auto& frame : vmd.MorphFrames) {
        ofs.write(reinterpret_cast<const char*>(&frame), sizeof(frame));
    }

    // カメラモーションフレーム数を書き込む
    int cameraCount = vmd.CameraCount;
    ofs.write(reinterpret_cast<const char*>(&cameraCount), sizeof(int));

    // ファイルを閉じる
    ofs.close();
}


//「\0」を除去する関数。
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
    //removeNullChar(cstr); // 最後に「\0」があれば除去する
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



//Shift-JISからUTF-8への変換
std::wstring jisToUTF(const std::string& str) {
    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], wsize);
    return wstr;
}


//Shift-JISからUTF-8への変換（新）
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



//＝＝＝＝＝＝＝＝以下デバッグ用情報表示＝＝＝＝＝＝＝＝


void printMotion(const VMDMotionFrame& motion) {
    //shift-JISの文字列をUTF-8に変換してボーンの名前を表示する
        
    std::wstring utf16str = jisToUTF(motion.BoneName);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string utf8str = converter.to_bytes(utf16str);

    std::cout << "BoneName: " << utf8str << std::endl;
    std::cout << "FrameNo: " << motion.FrameNo << std::endl;
    std::cout << "Location: (" << motion.Location[0] << ", " << motion.Location[1] << ", " << motion.Location[2] << ")" << std::endl;
    std::cout << "Rotation: (" << motion.Rotation[0] << ", " << motion.Rotation[1] << ", " << motion.Rotation[2] << ", " << motion.Rotation[3] << ")" << std::endl;
    std::cout << "Interpolation: (" << motion.Interpolation[0] << ", " << motion.Interpolation[1] << ", " << motion.Interpolation[2] << ", " << motion.Interpolation[3] << ")" << std::endl;
}


