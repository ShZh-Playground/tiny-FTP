#pragma once

#include <string>
#include <vector>
#include "path_info.h"

using std::string;
using std::vector;

class Dir {
 private:
	 const std::string name;
	 vector<PathInfo> all_info;

 public:
	 void Push(const PathInfo&);
	 vector<string> GetFilesName();
};

