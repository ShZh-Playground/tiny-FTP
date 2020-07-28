// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "pch.h"
#include "../client/client/client.h"
#include <iostream>

#pragma comment(lib, "client.lib")

using namespace std;

int main()
{
	// 初始化
  iostream::sync_with_stdio(false);
  IClient* client = GetClient("192.168.176.1");
  client->Login("shzh", "123456");

	// client->DownloadFile("zj.m");
	client->UploadFile("image.jpg");

	// // 列出所有目录
	//auto file_names = client->GetDirList();
	//for (auto file : file_names) {
	//	cout << file << endl;
	//}

	// // 获取服务器文件的大小
	// cout << "FileSize: " << client->GetFileSize("zj.m") << endl;

	// 结束
	delete client;
	return 0;
}
