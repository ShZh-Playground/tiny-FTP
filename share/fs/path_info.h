#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::stringstream;

struct PathInfo {
	string name_ = "";
  string date_ = "";
  string time_ = "";
  bool is_dir_ = false;
  unsigned int size_ = -1;
	string absolute_path = "";

	PathInfo(const string& date, const string& time, const string& dir_or_size,const string& name)
      : name_(name), date_(date), time_(time) {
		if (dir_or_size == "<DIR>") {
			is_dir_ = true;
		} else {
			is_dir_ = false;
			size_ = atoi(dir_or_size.c_str());
		}
	}
	PathInfo(stringstream& ss) { 
		vector<string> prop(4);
    ss >> prop[0] >> prop[1] >> prop[2] >> prop[3];  // 每行用空格分隔四个信息
		new (this)PathInfo(prop[0], prop[1], prop[2], prop[3]); // Placement new
	}
	~PathInfo() {}
};