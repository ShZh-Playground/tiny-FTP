#pragma once

#include <string>
#include <vector>
#include "path_info.h"

// 封装Windows API得到的仿现代语言的File类
namespace FileSystem {
	namespace File {
		bool IsDirectory(const string& path);

		bool IsFile(const string& path);

		vector<string> GetPathInfoInDir(const string& dir_name);

		void CreateFolder(const string& folder_name);
	}  // namespace File
} // namespace FileSystem

