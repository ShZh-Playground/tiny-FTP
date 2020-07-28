// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../client/client/client.h"


#pragma comment(lib, "client.lib")

using namespace std;

IClient* Initialize() {
  iostream::sync_with_stdio(false);
  IClient* client = GetClient("192.168.176.1");
  client->Login("shzh", "123456");
	return client;
}

void TestDownload(const std::string& filename) {
	auto iclient = Initialize();
	iclient->DownloadFile(filename);
	delete iclient;
}

void TestUpload(const std::string& filename) {
  auto iclient = Initialize();
	iclient->UploadFile(filename);
  delete iclient;
}

void TestListDir() {
  auto iclient = Initialize();
  auto file_names = iclient->GetDirList();
  for (auto file : file_names) {
  	cout << file << endl;
  }
  delete iclient;
}

void TestGetFileSize(const std::string& filename) {
  auto iclient = Initialize();
  cout << "FileSize: " << iclient->GetFileSize("zj.m") << endl << endl;
  delete iclient;
}

int main() {
	TestDownload("image.jpg");
  return 0;
}
