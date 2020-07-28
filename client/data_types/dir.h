#pragma once

#include <string>
#include <vector>
#include "path_info.h"

using std::string;
using std::vector;

struct Dir {
	const std::string name;
	vector<PathInfo> all_info;

	void Push(const PathInfo&);
	vector<string> GetFilesName();
};

