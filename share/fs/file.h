#pragma once

#include <string>
#include <vector>
#include "path_info.h"

// 封装Windows API得到的仿现代语言的File类
namespace File {
	using std::string;
	using std::vector;

	bool IsDirectory(const string& path);

	bool IsFile(const string& path);

	vector<string> GetPathInfoInDir(const string& dir_name);

	void CreateFolder(const string& folder_name);
}
