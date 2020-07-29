#include <Windows.h>
#include <io.h>
#include "file.h"

bool File::IsDirectory(const string& path) {
	WIN32_FIND_DATAA find_file_data;
	FindFirstFileA(path.c_str(), &find_file_data);
	return find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

bool File::IsFile(const string& path) {
	return !IsDirectory(path);
}

vector<string> File::GetPathInfoInDir(const string& dir_name) {
	if (IsDirectory(dir_name)) {
		vector<string> result;
		string match = dir_name + "\\*";
		_finddata_t file_info;
		long long find_result = _findfirst(match.c_str(), &file_info);
		if (find_result == -1) {
			_findclose(find_result);
			return vector<string>();
		}
		string file_name;
		do {
			if (file_info.attrib == _A_ARCH || 
				(file_info.attrib == _A_SUBDIR &&
                       strcmp(file_info.name, ".") != 0 &&
                       strcmp(file_info.name, "..") != 0)) {
				file_name = dir_name + "\\" + file_info.name;
				result.push_back(file_name);
			}
		} while (_findnext(find_result, &file_info) == 0);
		_findclose(find_result);
		return result;
	} else {
		return vector<string>();
	}
}