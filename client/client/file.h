#pragma once

#include <string>
#include <vector>
#include "../data_types/path_info.h"

namespace File {
	using std::string;
	using std::vector;

	bool IsDirectory(const string& path);

	bool IsFile(const string& path);

	vector<string> GetPathInfoInDir(const string& dir_name);
}
