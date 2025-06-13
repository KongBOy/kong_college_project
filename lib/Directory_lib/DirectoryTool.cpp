#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// 資料夾裡所有副檔名是 .jpg 的檔案名稱抓出來（只抓到檔案名，不含路徑），然後回傳。
char *get_all_files_names_within_folder(string folder){
    string filename="";
    char * names;
    char search_path[200];
    sprintf(search_path, "%s/*.jpg*", folder.c_str());  // 把「搜尋路徑」組成 folder/*.jpg*
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path, &fd);  // 開始抓資料夾裡的檔案。
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names=fd.cFileName;
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    filename = names;
    //cout<<names<<endl;
    return names;
}

// 資料夾裡所有副檔名是 .jpg 的檔案名稱抓出來（只抓到檔案名，不含路徑），然後回傳。
vector<string> Get_all_JPG_Files_in_folder(const string& folder) {
    vector<string> file_names;
    char search_path[MAX_PATH];
    sprintf(search_path, "%s\\*.jpg", folder.c_str()); // 把 搜尋路徑 組成 folder/*.jpg*, 第一個參數是容器, 第二個參數是 格式

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search_path, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                file_names.push_back(fd.cFileName);
            }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
    return file_names;
}


void Move_JPG_Files(const string& sourceFolder, const string& targetFolder) {
    char search_path[MAX_PATH];
    sprintf(search_path, "%s\\*.jpg", sourceFolder.c_str());

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path, &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                string srcPath = sourceFolder + "\\" + fd.cFileName;
                string dstPath = targetFolder + "\\" + fd.cFileName;

                if (!MoveFileEx(srcPath.c_str(), dstPath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
                    cerr << "Failed to move: " << srcPath 
                              << " Error code: " << GetLastError() << endl;
                } else {
                    cout << "Moved: " << srcPath 
                              << " -> " << dstPath << endl;
                }
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    } else {
        cerr << "No .jpg files found in: " << sourceFolder << endl;
    }
}

//
// vector<string> get_all_jpg_files_in_folder(const string& folder) {
//     vector<string> file_names;
//     for (const auto& entry : filesystem::directory_iterator(folder)) {
//         if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
//             file_names.push_back(entry.path().filename().string());
//         }
//     }
//     return file_names;
// }

void deleAllFile(string folder){
    string filename="";
    char * names;
    char search_path[200];
    sprintf(search_path, "%s/*.jpg*", folder.c_str());
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path, &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names=fd.cFileName;
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    filename = names;
    string FilePath = folder + "\\" + filename;
    remove(FilePath.c_str());
    filename="";
    //cout<<names<<endl;


}
