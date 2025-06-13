#include <iostream>

using namespace std;

extern string filename;

char           *get_all_files_names_within_folder(string folder);
vector<string>  Get_all_JPG_Files_in_folder      (const string& folder);
void            Move_JPG_Files(const string& sourceFolder, const string& targetFolder);
void            deleAllFile(string folder);